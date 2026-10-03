const fs = require('node:fs');
const http = require('node:http');
const path = require('node:path');
const { spawn } = require('node:child_process');

const ROOT = path.resolve(__dirname, '..', '..');
const DATA = path.resolve(process.env.LIBRARY_DATA_DIR || path.join(ROOT, 'data'));
const PUBLIC = path.join(ROOT, 'src', 'presentation');
const HOST = '127.0.0.1';
const PORT = Number(process.env.PORT || 4173);
const DATA_FILES = ['books', 'members', 'loans', 'reservations', 'activities'];
const BRIDGE = path.resolve(process.env.LIBRARY_BRIDGE_PATH || path.join(ROOT, 'build', 'library_management.exe'));

const core = spawn(BRIDGE, ['--web-bridge', DATA], { cwd: ROOT, windowsHide: true, stdio: ['pipe', 'pipe', 'pipe'] });
const replies = [];
let replyBuffer = '';
let coreFailure = '';
let coreQueue = Promise.resolve();
core.stdout.setEncoding('utf8');
core.stdout.on('data', (chunk) => {
  replyBuffer += chunk;
  const lines = replyBuffer.split(/\r?\n/);
  replyBuffer = lines.pop();
  for (const line of lines) {
    const pending = replies.shift();
    if (!pending) continue;
    try { pending.resolve(JSON.parse(line)); }
    catch { pending.reject(new Error('DSA Core trả về dữ liệu không hợp lệ.')); }
  }
});
core.stderr.setEncoding('utf8');
core.stderr.on('data', (chunk) => { coreFailure += chunk; console.error(chunk.trim()); });
core.on('error', (error) => {
  coreFailure = error.message;
  while (replies.length) replies.shift().reject(error);
});
core.stdin.on('error', (error) => {
  coreFailure = error.message;
  while (replies.length) replies.shift().reject(error);
});
core.on('exit', (code) => {
  const error = new Error(`DSA Core đã dừng${code === null ? '' : ` (mã ${code})`}. ${coreFailure}`);
  while (replies.length) replies.shift().reject(error);
});

function requestCore(value) {
  const run = () => new Promise((resolve, reject) => {
    if (coreFailure || core.exitCode !== null) return reject(new Error(`Không kết nối được DSA Core. ${coreFailure}`));
    replies.push({ resolve, reject });
    core.stdin.write(`${JSON.stringify(value)}\n`);
  });
  const result = coreQueue.then(run);
  coreQueue = result.catch(() => {});
  return result;
}

function readState() {
  const state = {};
  for (const name of DATA_FILES) {
    const file = path.join(DATA, `${name}.json`);
    const parsed = JSON.parse(fs.readFileSync(file, 'utf8'));
    if (!Array.isArray(parsed)) throw new Error(`${name}.json phải chứa một danh sách.`);
    state[name] = parsed;
  }
  return state;
}

function sendJson(response, status, value) {
  response.writeHead(status, {
    'Content-Type': 'application/json; charset=utf-8',
    'Cache-Control': 'no-store',
    'X-Content-Type-Options': 'nosniff',
  });
  response.end(JSON.stringify(value));
}

function readBody(request) {
  return new Promise((resolve, reject) => {
    let body = '';
    request.setEncoding('utf8');
    request.on('data', (chunk) => {
      body += chunk;
      if (body.length > 100_000) {
        reject(new Error('Dữ liệu gửi lên quá lớn.'));
        request.destroy();
      }
    });
    request.on('end', () => {
      try { resolve(JSON.parse(body || '{}')); }
      catch { reject(new Error('Dữ liệu gửi lên không đúng định dạng.')); }
    });
    request.on('error', reject);
  });
}

async function handleApi(request, response, url) {
  if (request.method === 'GET' && url.pathname === '/api/state') {
    const requestedWindow = Number(url.searchParams.get('windowDays') || 30);
    const windowDays = Number.isInteger(requestedWindow) ? Math.max(1, Math.min(3650, requestedWindow)) : 30;
    const derivedState = await requestCore({ action: 'refresh', windowDays });
    return sendJson(response, 200, { ...readState(), ...derivedState });
  }
  if (request.method !== 'POST') return sendJson(response, 405, { error: 'Phương thức không được hỗ trợ.' });

  const input = await readBody(request);
  const actionByPath = {
    '/api/borrow': 'borrow',
    '/api/return': 'return',
    '/api/reservations': 'reserve',
    '/api/reservations/cancel': 'cancel-reservation',
  };
  const action = actionByPath[url.pathname];
  if (!action) return sendJson(response, 404, { error: 'Không tìm thấy API.' });
  const result = await requestCore({ ...input, action });
  if (!result.ok) return sendJson(response, result.status || 400, result);
  return sendJson(response, action === 'borrow' || action === 'reserve' ? 201 : 200, result);
}

function serveFile(request, response, url) {
  const requested = decodeURIComponent(url.pathname === '/' ? '/index.html' : url.pathname);
  const file = path.resolve(PUBLIC, `.${requested}`);
  if (!file.startsWith(`${PUBLIC}${path.sep}`) && file !== path.join(PUBLIC, 'index.html')) {
    response.writeHead(403); return response.end('Forbidden');
  }
  if (!fs.existsSync(file) || !fs.statSync(file).isFile()) {
    response.writeHead(404); return response.end('Không tìm thấy trang.');
  }
  const types = { '.html': 'text/html; charset=utf-8', '.css': 'text/css; charset=utf-8', '.js': 'text/javascript; charset=utf-8', '.svg': 'image/svg+xml' };
  response.writeHead(200, { 'Content-Type': types[path.extname(file)] || 'application/octet-stream', 'Cache-Control': 'no-cache', 'X-Content-Type-Options': 'nosniff' });
  fs.createReadStream(file).pipe(response);
}

const server = http.createServer(async (request, response) => {
  const url = new URL(request.url, `http://${HOST}:${PORT}`);
  try {
    if (url.pathname.startsWith('/api/')) await handleApi(request, response, url);
    else if (request.method === 'GET') serveFile(request, response, url);
    else sendJson(response, 405, { error: 'Phương thức không được hỗ trợ.' });
  } catch (error) {
    console.error(error);
    if (!response.headersSent) sendJson(response, 500, { error: error.message || 'Lỗi máy chủ.' });
    else response.destroy();
  }
});

server.listen(PORT, HOST, () => {
  console.log(`Web quản lý thư viện đang chạy tại http://${HOST}:${PORT}`);
  console.log('Giữ cửa sổ terminal này mở trong lúc sử dụng web. Nhấn Ctrl+C để dừng.');
});

for (const signal of ['SIGINT', 'SIGTERM']) {
  process.on(signal, () => { core.kill(); server.close(() => process.exit(0)); });
}
