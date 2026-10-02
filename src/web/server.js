const fs = require('node:fs');
const http = require('node:http');
const path = require('node:path');

const ROOT = path.resolve(__dirname, '..', '..');
const DATA = path.resolve(process.env.LIBRARY_DATA_DIR || path.join(ROOT, 'data'));
const PUBLIC = path.join(ROOT, 'src', 'presentation');
const HOST = '127.0.0.1';
const PORT = Number(process.env.PORT || 4173);
const DATA_FILES = ['books', 'members', 'loans', 'reservations', 'activities'];

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

function saveState(state, names) {
  const writes = names.map((name) => {
    const file = path.join(DATA, `${name}.json`);
    const temporary = `${file}.tmp`;
    fs.writeFileSync(temporary, `${JSON.stringify(state[name], null, 2)}\n`, 'utf8');
    return { file, temporary };
  });
  try {
    for (const { file, temporary } of writes) fs.renameSync(temporary, file);
  } catch (error) {
    for (const { temporary } of writes) {
      if (fs.existsSync(temporary)) fs.unlinkSync(temporary);
    }
    throw error;
  }
}

function nextId(rows, field, prefix) {
  let maximum = 0;
  let width = 2;
  for (const row of rows) {
    const id = String(row[field] || '');
    if (!id.startsWith(prefix)) continue;
    const suffix = id.slice(prefix.length);
    if (!/^\d+$/.test(suffix)) continue;
    maximum = Math.max(maximum, Number(suffix));
    width = Math.max(width, suffix.length);
  }
  return `${prefix}${String(maximum + 1).padStart(width, '0')}`;
}

function isDate(value) {
  if (typeof value !== 'string' || !/^\d{4}-\d{2}-\d{2}$/.test(value)) return false;
  const [year, month, day] = value.split('-').map(Number);
  const date = new Date(Date.UTC(year, month - 1, day));
  return date.getUTCFullYear() === year && date.getUTCMonth() === month - 1 && date.getUTCDate() === day;
}

function nowText() {
  const now = new Date();
  const local = new Date(now.getTime() - now.getTimezoneOffset() * 60_000);
  return local.toISOString().slice(0, 19).replace('T', ' ');
}

function addActivity(state, type, bookId, memberId, loanId, detail) {
  state.activities.push({
    activityId: nextId(state.activities, 'activityId', 'A'),
    type,
    bookId,
    memberId,
    loanId: loanId || '',
    reservationId: '',
    time: nowText(),
    detail,
  });
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
    return sendJson(response, 200, readState());
  }
  if (request.method !== 'POST') return sendJson(response, 405, { error: 'Phương thức không được hỗ trợ.' });

  const input = await readBody(request);
  const state = readState();

  if (url.pathname === '/api/borrow') {
    const book = state.books.find((item) => item.bookId === input.bookId);
    const member = state.members.find((item) => item.memberId === input.memberId);
    const copy = book?.copies?.find((item) => item.copyId === input.copyId);
    if (!book || !member || !copy) return sendJson(response, 404, { error: 'Không tìm thấy sách, bản sao hoặc thành viên.' });
    if (member.status !== 'active') return sendJson(response, 409, { error: 'Thành viên hiện không hoạt động.' });
    if (member.totalBorrowBooks >= member.maxBorrow) return sendJson(response, 409, { error: 'Thành viên đã đạt giới hạn mượn.' });
    if (book.availableCopies <= 0 || copy.status !== 'available') return sendJson(response, 409, { error: 'Bản sao này hiện không sẵn sàng.' });
    if (!isDate(input.borrowDate) || !isDate(input.dueDate) || input.dueDate <= input.borrowDate)
      return sendJson(response, 400, { error: 'Ngày mượn và hạn trả không hợp lệ.' });

    const reservation = state.reservations.find((item) => item.bookId === book.bookId);
    if (reservation?.queue?.length && reservation.queue[0].memberId !== member.memberId)
      return sendJson(response, 409, { error: `Sách đang chờ thành viên ${reservation.queue[0].memberId} ở đầu hàng.` });

    const loanId = nextId(state.loans, 'loanId', 'L');
    copy.status = 'borrowing';
    book.availableCopies -= 1;
    book.borrowCount = (book.borrowCount || 0) + 1;
    member.totalBorrowBooks += 1;
    state.loans.push({ loanId, bookId: book.bookId, memberId: member.memberId,
      borrowDate: input.borrowDate, returnDate: null, dueDate: input.dueDate, status: 'borrowing' });
    if (reservation?.queue?.length) reservation.queue.shift();
    addActivity(state, 'borrow', book.bookId, member.memberId, loanId, `Mượn sách ${book.bookId}`);
    saveState(state, ['books', 'members', 'loans', 'reservations', 'activities']);
    return sendJson(response, 201, { message: `Mượn sách thành công. Mã phiếu: ${loanId}`, loanId });
  }

  if (url.pathname === '/api/return') {
    const loan = state.loans.find((item) => item.loanId === input.loanId);
    if (!loan || loan.returnDate) return sendJson(response, 404, { error: 'Không tìm thấy phiếu đang mượn.' });
    if (!isDate(input.returnDate)) return sendJson(response, 400, { error: 'Ngày trả không hợp lệ.' });
    const book = state.books.find((item) => item.bookId === loan.bookId);
    const member = state.members.find((item) => item.memberId === loan.memberId);
    const copy = book?.copies?.find((item) => item.copyId === input.copyId);
    if (!book || !member || !copy || !['borrowing', 'borrowed'].includes(copy.status))
      return sendJson(response, 409, { error: 'Không tìm thấy bản sao đang được mượn của sách này.' });

    copy.status = 'available';
    book.availableCopies = Math.min(book.totalCopies, book.availableCopies + 1);
    member.totalBorrowBooks = Math.max(0, member.totalBorrowBooks - 1);
    loan.returnDate = input.returnDate;
    loan.status = 'returned';
    addActivity(state, 'return', book.bookId, member.memberId, loan.loanId, `Trả sách ${book.bookId}`);
    saveState(state, ['books', 'members', 'loans', 'activities']);
    return sendJson(response, 200, { message: 'Trả sách thành công.' });
  }

  if (url.pathname === '/api/reservations') {
    const book = state.books.find((item) => item.bookId === input.bookId);
    const member = state.members.find((item) => item.memberId === input.memberId);
    if (!book || !member) return sendJson(response, 404, { error: 'Không tìm thấy sách hoặc thành viên.' });
    if (member.status !== 'active') return sendJson(response, 409, { error: 'Thành viên hiện không hoạt động.' });
    if (book.availableCopies > 0) return sendJson(response, 409, { error: 'Sách vẫn còn bản sẵn sàng, chưa cần vào hàng chờ.' });
    let reservation = state.reservations.find((item) => item.bookId === book.bookId);
    if (!reservation) {
      reservation = { bookId: book.bookId, queue: [] };
      state.reservations.push(reservation);
    }
    if (reservation.queue.some((item) => item.memberId === member.memberId))
      return sendJson(response, 409, { error: 'Thành viên đã có trong hàng chờ sách này.' });
    reservation.queue.push({ memberId: member.memberId, reservedAt: nowText() });
    addActivity(state, 'reserve', book.bookId, member.memberId, '', `Đăng ký chờ mượn ${book.bookId}`);
    saveState(state, ['reservations', 'activities']);
    return sendJson(response, 201, { message: `Đã thêm vào hàng chờ, vị trí ${reservation.queue.length}.` });
  }

  if (url.pathname === '/api/reservations/cancel') {
    const reservation = state.reservations.find((item) => item.bookId === input.bookId);
    if (!reservation) return sendJson(response, 404, { error: 'Không tìm thấy hàng chờ của sách này.' });
    const index = reservation.queue.findIndex((item) => item.memberId === input.memberId);
    if (index < 0) return sendJson(response, 404, { error: 'Không tìm thấy thành viên trong hàng chờ.' });
    reservation.queue.splice(index, 1);
    saveState(state, ['reservations']);
    return sendJson(response, 200, { message: `Đã bỏ lượt chờ của thành viên ${input.memberId}.` });
  }

  return sendJson(response, 404, { error: 'Không tìm thấy API.' });
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
