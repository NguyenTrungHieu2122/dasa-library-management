const $ = (selector, root = document) => root.querySelector(selector);
const content = $('#content');
const dialog = $('#actionDialog');
let state = null;
let currentPage = 'dashboard';
let loanFilter = 'active';
let topKWindowDays = 30;
let activityLimit = 10;
let toastTimer;

const pageNames = {
  dashboard: ['Tổng quan', 'TỔNG QUAN'],
  books: ['Kho sách', 'KHO SÁCH'],
  members: ['Thành viên', 'THÀNH VIÊN'],
  loans: ['Mượn & trả', 'MƯỢN & TRẢ'],
  activity: ['Lịch sử hoạt động', 'LỊCH SỬ'],
};

function esc(value = '') {
  return String(value).replace(/[&<>"']/g, (char) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' })[char]);
}

function localDate(date = new Date()) {
  const year = date.getFullYear();
  const month = String(date.getMonth() + 1).padStart(2, '0');
  const day = String(date.getDate()).padStart(2, '0');
  return `${year}-${month}-${day}`;
}

function afterDays(days) {
  const date = new Date(); date.setDate(date.getDate() + days); return localDate(date);
}

function formatDate(value) {
  if (!value) return '—';
  const match = String(value).match(/^(\d{4})-(\d{2})-(\d{2})/);
  if (match) return `${match[3]}/${match[2]}/${match[1]}`;
  return esc(value);
}

function formatTime(value) {
  if (!value) return '—';
  const date = new Date(String(value).replace(' ', 'T'));
  if (Number.isNaN(date.valueOf())) return esc(value);
  return new Intl.DateTimeFormat('vi-VN', { day: '2-digit', month: '2-digit', hour: '2-digit', minute: '2-digit' }).format(date);
}

function personName(id) {
  return state.members.find((member) => member.memberId === id)?.fullname || `Thành viên ${id}`;
}

function bookName(id) {
  return state.books.find((book) => book.bookId === id)?.title || id;
}

async function api(url, options = {}) {
  const response = await fetch(url, {
    ...options,
    headers: { ...(options.body ? { 'Content-Type': 'application/json' } : {}), ...options.headers },
  });
  const data = await response.json();
  if (!response.ok) throw new Error(data.error || 'Không thể hoàn thành thao tác.');
  return data;
}

async function loadState(showLoading = false) {
  if (showLoading) content.innerHTML = '<div class="loading-state"><span class="spinner"></span>Đang tải dữ liệu thư viện…</div>';
  try {
    state = await api(`/api/state?windowDays=${topKWindowDays}`);
    $('#lastUpdated').textContent = `Cập nhật lúc ${new Intl.DateTimeFormat('vi-VN', { hour: '2-digit', minute: '2-digit' }).format(new Date())} · localhost`;
    renderPage();
  } catch (error) {
    content.innerHTML = `<div class="empty-state"><span class="empty-icon">⚠</span><strong>Chưa kết nối được máy chủ</strong><span>${esc(error.message)} Hãy kiểm tra cửa sổ chạy server rồi tải lại trang.</span></div>`;
    $('#lastUpdated').textContent = 'Không kết nối được dữ liệu';
  }
}

function showToast(message, isError = false) {
  const toast = $('#toast'); toast.textContent = message; toast.classList.toggle('error', isError); toast.classList.add('show');
  clearTimeout(toastTimer); toastTimer = setTimeout(() => toast.classList.remove('show'), 3400);
}

function metric(title, value, caption, icon, color = '') {
  return `<article class="metric-card"><div class="metric-top">${title}<span class="metric-icon ${color}">${icon}</span></div><div class="metric-value">${value}</div><div class="metric-caption">${caption}</div></article>`;
}

function activityRows(items, limit = 7) {
  if (!items.length) return '<div class="empty-state"><strong>Chưa có hoạt động</strong><span>Các giao dịch sẽ xuất hiện tại đây.</span></div>';
  return `<div class="activity-list">${items.slice(0, limit).map((item) => {
    const symbol = item.type === 'return' ? '↙' : item.type === 'reserve' ? '⌛' : '↗';
    const kind = item.type === 'return' ? 'return' : '';
    return `<div class="activity-item"><span class="activity-symbol ${kind}">${symbol}</span><div class="activity-copy"><strong>${esc(item.detail || `${item.type} · ${item.bookId}`)}</strong><small>${esc(personName(item.memberId))} · ${esc(item.bookId || '')}</small></div><span class="activity-time">${formatTime(item.time)}</span></div>`;
  }).join('')}</div>`;
}

function renderDashboard() {
  const activeLoans = state.loans.filter((loan) => !loan.returnDate);
  const overdue = activeLoans.filter((loan) => loan.dueDate && loan.dueDate < localDate()).length;
  const stock = state.books.reduce((sum, book) => sum + Number(book.availableCopies || 0), 0);
  const ranked = state.topBooks || [...state.books].sort((a, b) => (b.borrowCount || 0) - (a.borrowCount || 0)).slice(0, 5);
  const highest = Math.max(1, ...ranked.map((book) => Number(book.borrowCount || 0)));
  const today = new Intl.DateTimeFormat('vi-VN', { weekday: 'long', day: 'numeric', month: 'long', year: 'numeric' }).format(new Date());
  content.innerHTML = `
    <div class="welcome-row"><div><h2>Chào bạn, thủ thư <span aria-hidden="true">✦</span></h2><p>Tổng quan hoạt động thư viện của bạn hôm nay.</p></div><div class="date-chip">◷ &nbsp;${esc(today)}</div></div>
    <div class="metrics">
      ${metric('Đầu sách', state.books.length, 'Tổng số tựa trong thư viện', '▤')}
      ${metric('Bản sẵn sàng', stock, 'Có thể cho mượn ngay', '✓')}
      ${metric('Phiếu đang mượn', activeLoans.length, 'Tất cả phiếu chưa trả', '⇄', 'amber')}
      ${metric('Quá hạn', overdue, 'Cần được xử lý', '!', overdue ? 'rose' : '')}
    </div>
    <div class="dashboard-grid">
      <section class="panel"><div class="panel-heading"><h3>Sách được quan tâm · ${Number(state.topKWindowDays || topKWindowDays)} ngày</h3><select class="filter-select" id="topKWindowDays" aria-label="Khoảng thời gian xếp hạng"><option value="7" ${topKWindowDays === 7 ? 'selected' : ''}>7 ngày</option><option value="30" ${topKWindowDays === 30 ? 'selected' : ''}>30 ngày</option><option value="90" ${topKWindowDays === 90 ? 'selected' : ''}>90 ngày</option><option value="365" ${topKWindowDays === 365 ? 'selected' : ''}>365 ngày</option></select><button class="text-link" data-page="books">Mở kho sách →</button></div>
        <div class="top-book-list">${ranked.map((book, index) => `<div class="top-book"><span class="rank ${index === 0 ? 'first' : ''}">${String(index + 1).padStart(2, '0')}</span><div class="top-book-name"><strong>${esc(book.title)}</strong><small>${esc(book.bookId)} · ${esc(book.author || 'Chưa cập nhật tác giả')}</small></div><div class="bar-track"><div class="bar-fill" style="width:${Math.max(4, Math.round((Number(book.borrowCount || 0) / highest) * 100))}%"></div></div><span class="borrow-total">${Number(book.borrowCount || 0)} lượt</span></div>`).join('')}</div>
      </section>
      <section class="panel"><div class="panel-heading"><h3>Hoạt động mới</h3><button class="text-link" data-page="activity">Xem tất cả →</button></div>${activityRows([...state.activities].reverse(), 5)}</section>
    </div>`;
}

function stockStatus(book) {
  const reservation = state.reservations.find((item) => item.bookId === book.bookId);
  if (reservation?.holds?.length) {
    const holders = reservation.holds.map((hold) => hold.memberId).join(', ');
    return `<span class="status-pill waiting">Đang giữ cho ${esc(holders)}</span>`;
  }
  if (book.availableCopies <= 0) return '<span class="status-pill out">Hết sách</span>';
  if (book.availableCopies <= 2) return '<span class="status-pill waiting">Sắp hết</span>';
  return '<span class="status-pill available">Còn sách</span>';
}

function bookTable(books) {
  if (!books.length) return '<tr><td colspan="6" class="empty-cell">Không tìm thấy sách phù hợp.</td></tr>';
  return books.map((book) => {
    const total = Number(book.totalCopies || 0); const available = Number(book.availableCopies || 0);
    const width = total ? Math.round((available / total) * 100) : 0;
    const fill = available === 0 ? 'none' : available <= 2 ? 'low' : '';
    const reservation = state.reservations.find((item) => item.bookId === book.bookId);
    const action = available > 0 || reservation?.holds?.length
      ? `<button class="row-action" data-action="borrow" data-book="${esc(book.bookId)}">Mượn sách</button>`
      : `<button class="row-action" data-action="reserve" data-book="${esc(book.bookId)}">Đặt chờ</button>`;
    return `<tr><td><span class="book-id">${esc(book.bookId)}</span></td><td><div class="book-title">${esc(book.title)}</div><div class="book-author">${esc(book.author || 'Chưa cập nhật tác giả')}</div></td><td>${esc(book.category || '—')}</td><td>${stockStatus(book)}</td><td class="stock-cell"><div class="stock-text">${available} / ${total} bản</div><div class="stock-track"><div class="stock-fill ${fill}" style="width:${width}%"></div></div></td><td>${action}</td></tr>`;
  }).join('');
}

function renderBooks(query = '') {
  const search = query.trim().toLocaleLowerCase('vi');
  const rows = state.books.filter((book) => [book.bookId, book.title, book.author, book.category].some((value) => String(value || '').toLocaleLowerCase('vi').includes(search)));
  content.innerHTML = `
    <div class="section-head"><div><h2>Kho sách</h2><p>Tra cứu và quản lý ${state.books.length} đầu sách trong thư viện.</p></div><div class="section-actions"><label class="search-wrap"><span>⌕</span><input id="bookSearch" class="search-input" type="search" placeholder="Tìm theo tên, mã, tác giả…" value="${esc(query)}"></label></div></div>
    <div class="book-count" id="bookCount">Hiển thị ${rows.length} đầu sách</div>
    <div class="table-card table-scroll"><table class="data-table"><thead><tr><th>Mã sách</th><th>Tên sách</th><th>Thể loại</th><th>Tình trạng</th><th>Kho khả dụng</th><th>Thao tác</th></tr></thead><tbody id="bookRows">${bookTable(rows)}</tbody></table></div>`;
  $('#bookSearch').addEventListener('input', (event) => {
    const filtered = state.books.filter((book) => [book.bookId, book.title, book.author, book.category].some((value) => String(value || '').toLocaleLowerCase('vi').includes(event.target.value.trim().toLocaleLowerCase('vi'))));
    $('#bookRows').innerHTML = bookTable(filtered); $('#bookCount').textContent = `Hiển thị ${filtered.length} đầu sách`;
  });
}

function initials(name) { return String(name || '?').trim().split(/\s+/).slice(-2).map((part) => part[0]).join('').toLocaleUpperCase('vi'); }

function renderMembers() {
  const sorted = [...state.members].sort((a, b) => String(a.memberId).localeCompare(String(b.memberId)));
  content.innerHTML = `<div class="section-head"><div><h2>Thành viên</h2><p>Danh sách ${state.members.length} bạn đọc và hạn mức mượn.</p></div></div>
    <div class="table-card table-scroll"><table class="data-table"><thead><tr><th>Thành viên</th><th>Mã thành viên</th><th>Điện thoại</th><th>Ngày tham gia</th><th>Đang mượn</th><th>Trạng thái</th></tr></thead><tbody>${sorted.map((member) => `<tr><td><div class="member-cell"><span class="member-avatar">${esc(initials(member.fullname))}</span><div><div class="member-name">${esc(member.fullname)}</div><div class="member-sub">${esc(member.phone || 'Chưa có số điện thoại')}</div></div></div></td><td><span class="book-id">${esc(member.memberId)}</span></td><td>${esc(member.phone || '—')}</td><td>${formatDate(member.joinDate)}</td><td><strong>${Number(member.totalBorrowBooks || 0)}</strong><span class="member-sub"> / ${Number(member.maxBorrow || 0)} quyển</span></td><td><span class="status-pill ${member.status === 'active' ? 'active' : 'out'}">${member.status === 'active' ? 'Hoạt động' : 'Tạm khóa'}</span></td></tr>`).join('')}</tbody></table></div>`;
}

function loanTable(loans) {
  if (!loans.length) return '<tr><td colspan="6" class="empty-cell">Không có phiếu trong mục này.</td></tr>';
  return loans.map((loan) => {
    const overdue = !loan.returnDate && loan.dueDate < localDate();
    const status = loan.returnDate ? '<span class="status-pill returned">Đã trả</span>' : overdue ? '<span class="status-pill overdue">Quá hạn</span>' : '<span class="status-pill available">Đang mượn</span>';
    const action = loan.returnDate ? '—' : `<button class="row-action" data-action="return" data-loan="${esc(loan.loanId)}">Nhận trả</button>`;
    return `<tr><td><span class="book-id">${esc(loan.loanId)}</span></td><td><div class="book-title">${esc(bookName(loan.bookId))}</div><div class="book-author">${esc(loan.bookId)}</div></td><td><div class="member-name">${esc(personName(loan.memberId))}</div><div class="member-sub">${esc(loan.memberId)}</div></td><td>${formatDate(loan.borrowDate)}</td><td>${formatDate(loan.dueDate)}</td><td>${status}</td><td>${action}</td></tr>`;
  }).join('');
}

function renderLoans() {
  const activeLoans = state.dueLoans || state.loans.filter((loan) => !loan.returnDate);
  const filtered = (loanFilter === 'returned' ? state.loans : activeLoans).filter((loan) => {
    if (loanFilter === 'active') return !loan.returnDate;
    if (loanFilter === 'returned') return Boolean(loan.returnDate);
    return !loan.returnDate && loan.dueDate < localDate();
  });
  content.innerHTML = `<div class="section-head"><div><h2>Phiếu mượn &amp; trả</h2><p>Theo dõi hạn trả và xử lý các giao dịch sách.</p></div><div class="section-actions"><button class="primary-button" data-action="quick-borrow">＋ Tạo phiếu mượn</button></div></div>
    <div class="table-card"><div class="table-toolbar"><span>${filtered.length} phiếu</span><div class="tab-switch"><button class="tab-button ${loanFilter === 'active' ? 'active' : ''}" data-filter="active">Đang mượn</button><button class="tab-button ${loanFilter === 'overdue' ? 'active' : ''}" data-filter="overdue">Quá hạn</button><button class="tab-button ${loanFilter === 'returned' ? 'active' : ''}" data-filter="returned">Đã trả</button></div></div>
      <div class="table-scroll"><table class="data-table"><thead><tr><th>Mã phiếu</th><th>Sách</th><th>Thành viên</th><th>Ngày mượn</th><th>Hạn trả</th><th>Trạng thái</th><th>Thao tác</th></tr></thead><tbody>${loanTable(filtered)}</tbody></table></div></div>
    <div class="section-head queue-heading"><div><h2>Hàng chờ đặt trước</h2><p>Thành viên được phục vụ theo thứ tự đăng ký.</p></div></div>
    <div class="panel queue-list">${renderQueues()}</div>`;
}

function renderQueues() {
  const queues = state.reservations.filter((reservation) => reservation.queue?.length || reservation.holds?.length);
  if (!queues.length) return '<div class="empty-state"><strong>Chưa có hàng chờ</strong><span>Các lượt đặt trước sẽ hiển thị tại đây.</span></div>';
  return queues.map((reservation) => {
    const holder = reservation.holds.map((hold) => `<div class="queue-row queue-holder"><span class="queue-position">✓</span><div class="queue-info"><strong>${esc(personName(hold.memberId))}</strong><small>Đang được giữ bản sao ${esc(hold.copyId)} đến ${formatTime(hold.holdUntil)}</small></div><span class="queue-book">${esc(reservation.bookId)} · ${esc(bookName(reservation.bookId))}</span><button class="queue-cancel" title="Bỏ quyền nhận sách" aria-label="Bỏ quyền nhận sách của ${esc(hold.memberId)}" data-action="cancel-reservation" data-book="${esc(reservation.bookId)}" data-member="${esc(hold.memberId)}">×</button></div>`).join('');
    const waiting = reservation.queue.map((entry, index) => `<div class="queue-row"><span class="queue-position">${index + 1}</span><div class="queue-info"><strong>${esc(personName(entry.memberId))}</strong><small>Đăng ký ${formatTime(entry.reservedAt)}</small></div><span class="queue-book">${esc(reservation.bookId)} · ${esc(bookName(reservation.bookId))}</span><button class="queue-cancel" title="Bỏ lượt chờ" aria-label="Bỏ lượt chờ ${esc(entry.memberId)}" data-action="cancel-reservation" data-book="${esc(reservation.bookId)}" data-member="${esc(entry.memberId)}">×</button></div>`).join('');
    return holder + waiting;
  }).join('');
}

function renderActivity() {
  const allItems = [...state.activities].reverse();
  const items = allItems.slice(0, activityLimit);
  content.innerHTML = `<div class="section-head"><div><h2>Lịch sử hoạt động</h2><p>Đang hiển thị ${items.length} trong ${allItems.length} hoạt động gần nhất.</p></div><div class="section-actions"><label class="activity-limit-control"><span>Số hoạt động gần đây</span><input id="activityLimit" type="number" min="1" max="1000" step="1" aria-label="Số hoạt động gần đây cần hiển thị" value="${activityLimit}"></label></div></div><section class="panel">${activityRows(items, items.length || 1)}</section>`;
}

function renderPage() {
  if (!state) return;
  const [title, crumb] = pageNames[currentPage]; $('#pageTitle').textContent = title; $('#breadcrumb').textContent = crumb;
  document.title = `${title} · QueCay Library`;
  document.querySelectorAll('.nav-item').forEach((item) => item.classList.toggle('active', item.dataset.page === currentPage));
  if (currentPage === 'dashboard') renderDashboard();
  else if (currentPage === 'books') renderBooks();
  else if (currentPage === 'members') renderMembers();
  else if (currentPage === 'loans') renderLoans();
  else renderActivity();
}

function setPage(page) {
  if (!pageNames[page]) return;
  currentPage = page; $('#sidebar').classList.remove('open'); renderPage();
}

function memberOptions() {
  return state.members.filter((member) => member.status === 'active').map((member) => `<option value="${esc(member.memberId)}">${esc(member.memberId)} · ${esc(member.fullname)}</option>`).join('');
}

function eligibleBorrowMembers(reservation, book) {
  const holds = reservation?.holds || [];
  let candidates;
  if (reservation?.queue?.length) {
    const memberIds = holds.length ? holds.map((hold) => hold.memberId) : [reservation.queue[0].memberId];
    candidates = memberIds.map((memberId) => state.members.find((member) => member.memberId === memberId)).filter(Boolean);
  } else if (holds.length) {
    const heldMembers = holds.map((hold) => state.members.find((member) => member.memberId === hold.memberId)).filter(Boolean);
    const otherActiveMembers = state.members.filter((member) => member.status === 'active' && !holds.some((hold) => hold.memberId === member.memberId));
    candidates = [...heldMembers, ...otherActiveMembers];
  } else {
    candidates = state.members.filter((member) => member.status === 'active');
  }
  return candidates.filter((member) => {
    const heldCopyId = holds.find((hold) => hold.memberId === member.memberId)?.copyId;
    return heldCopyId
      ? book?.copies?.some((copy) => copy.copyId === heldCopyId && copy.status === 'reserved')
      : book?.copies?.some((copy) => copy.status === 'available');
  });
}

function memberOptionsForBorrow(reservation, book) {
  const holds = reservation?.holds || [];
  const eligible = eligibleBorrowMembers(reservation, book);
  return eligible.map((member) => {
    const held = holds.some((hold) => hold.memberId === member.memberId);
    const note = held ? ' · đang được giữ' : '';
    return `<option value="${esc(member.memberId)}">${esc(member.memberId)} · ${esc(member.fullname)}${note}</option>`;
  }).join('');
}

function borrowCopyOptions(book, reservation, memberId) {
  const hold = reservation?.holds?.find((item) => item.memberId === memberId);
  const copies = (book?.copies || []).filter((copy) => hold
    ? copy.copyId === hold.copyId && copy.status === 'reserved'
    : copy.status === 'available');
  return copies.map((copy) => `<option value="${esc(copy.copyId)}">${esc(copy.copyId)}</option>`).join('');
}

function availableBookOptions() {
  return state.books.filter((book) => book.availableCopies > 0).map((book) => `<option value="${esc(book.bookId)}">${esc(book.bookId)} · ${esc(book.title)}</option>`).join('');
}

function setDialog(title, description, fields, submitLabel = 'Xác nhận', eyebrow = 'NGHIỆP VỤ') {
  $('#dialogEyebrow').textContent = eyebrow; $('#dialogTitle').textContent = title; $('#dialogDescription').textContent = description;
  $('#dialogFields').innerHTML = fields; $('#dialogSubmit').textContent = submitLabel;
  if (!dialog.open) dialog.showModal();
}

function openBorrow(bookId = '') {
  const eligibleBooks = state.books.filter((book) => book.availableCopies > 0 ||
    state.reservations.some((item) => item.bookId === book.bookId && item.holds?.length));
  if (!eligibleBooks.length) return showToast('Hiện không có bản sách nào sẵn sàng để mượn.', true);
  const chosen = eligibleBooks.find((book) => book.bookId === bookId) || eligibleBooks[0];
  const reservation = state.reservations.find((item) => item.bookId === chosen.bookId);
  const eligibleMembers = eligibleBorrowMembers(reservation, chosen);
  const memberOptions = memberOptionsForBorrow(reservation, chosen);
  const selectedMemberId = eligibleMembers[0]?.memberId || '';
  const eligibleCopies = borrowCopyOptions(chosen, reservation, selectedMemberId);
  $('#actionForm').dataset.actionType = 'borrow';
  $('#actionForm').dataset.bookId = '';
  setDialog('Tạo phiếu mượn', 'Chọn sách, bản sao và thành viên. Hạn trả phải sau ngày mượn.', `
    <div class="field"><label for="borrowBookId">Sách</label><select id="borrowBookId" required>${eligibleBooks.map((book) => `<option value="${esc(book.bookId)}" ${book.bookId === chosen.bookId ? 'selected' : ''}>${esc(book.bookId)} · ${esc(book.title)} (${book.availableCopies} bản)</option>`).join('')}</select></div>
    <div class="field"><label for="borrowCopyId">Bản sao phù hợp với thành viên</label><select id="borrowCopyId" required>${eligibleCopies}</select></div>
    <div class="field"><label for="borrowMemberId">Thành viên</label><select id="borrowMemberId" required>${memberOptions}</select></div>
    <div class="field-row"><div class="field"><label for="borrowDate">Ngày mượn</label><input id="borrowDate" type="date" value="${localDate()}" required></div><div class="field"><label for="dueDate">Hạn trả</label><input id="dueDate" type="date" value="${afterDays(14)}" required></div></div>`, 'Tạo phiếu');
  $('#borrowBookId').addEventListener('change', (event) => {
    const book = state.books.find((item) => item.bookId === event.target.value);
    const reservation = state.reservations.find((item) => item.bookId === book?.bookId);
    const members = $('#borrowMemberId');
    members.innerHTML = memberOptionsForBorrow(reservation, book);
    $('#borrowCopyId').innerHTML = borrowCopyOptions(book, reservation, members.value);
  });
  $('#borrowMemberId').addEventListener('change', (event) => {
    const book = state.books.find((item) => item.bookId === $('#borrowBookId').value);
    const reservation = state.reservations.find((item) => item.bookId === book?.bookId);
    $('#borrowCopyId').innerHTML = borrowCopyOptions(book, reservation, event.target.value);
  });
}

function openReserve(bookId) {
  const book = state.books.find((item) => item.bookId === bookId);
  if (!book) return;
  $('#actionForm').dataset.actionType = 'reserve';
  $('#actionForm').dataset.bookId = book.bookId;
  setDialog('Đăng ký đặt trước', `Sách ${book.title} hiện đã hết bản. Thành viên sẽ được xếp theo thứ tự đăng ký.`, `
    <div class="field"><label>Sách</label><input value="${esc(book.bookId)} · ${esc(book.title)}" disabled></div>
    <div class="field"><label for="reserveMemberId">Thành viên</label><select id="reserveMemberId" required>${memberOptions()}</select></div>`, 'Thêm vào hàng chờ');
}

function openReturn(loanId) {
  const loan = state.loans.find((item) => item.loanId === loanId);
  const book = state.books.find((item) => item.bookId === loan?.bookId);
  if (!loan || !book || !loan.copyId) return showToast('Phiếu mượn chưa được gắn với bản sao sách.', true);
  $('#actionForm').dataset.actionType = 'return';
  setDialog('Nhận trả sách', `${book.title} · ${personName(loan.memberId)} · Phiếu ${loan.loanId}`, `
    <div class="field"><label>Bản sao theo phiếu</label><input value="${esc(loan.copyId)}" disabled><div class="field-hint">Hệ thống sẽ nhận đúng bản sao gắn với phiếu mượn.</div></div>
    <div class="field"><label for="returnDate">Ngày trả</label><input id="returnDate" type="date" min="${esc(loan.borrowDate || '')}" value="${localDate()}" required></div>`, 'Xác nhận trả');
  $('#actionForm').dataset.loanId = loan.loanId;
}

async function submitAction(event) {
  event.preventDefault();
  const submit = $('#dialogSubmit'); submit.disabled = true;
  try {
    let result;
    if ($('#actionForm').dataset.actionType === 'borrow') {
      result = await api('/api/borrow', { method: 'POST', body: JSON.stringify({
        bookId: $('#borrowBookId').value, copyId: $('#borrowCopyId').value, memberId: $('#borrowMemberId').value,
        borrowDate: $('#borrowDate').value, dueDate: $('#dueDate').value,
      }) });
    } else if ($('#actionForm').dataset.actionType === 'reserve') {
      result = await api('/api/reservations', { method: 'POST', body: JSON.stringify({ bookId: $('#actionForm').dataset.bookId, memberId: $('#reserveMemberId').value }) });
    } else if ($('#actionForm').dataset.actionType === 'return') {
      result = await api('/api/return', { method: 'POST', body: JSON.stringify({
        loanId: $('#actionForm').dataset.loanId, returnDate: $('#returnDate').value,
      }) });
    }
    dialog.close(); $('#actionForm').dataset.loanId = ''; $('#actionForm').dataset.bookId = ''; $('#actionForm').dataset.actionType = '';
    await loadState(); showToast(result?.message || 'Đã cập nhật dữ liệu.');
  } catch (error) {
    showToast(error.message, true);
  } finally {
    submit.disabled = false;
  }
}

document.addEventListener('click', (event) => {
  const pageButton = event.target.closest('[data-page]');
  if (pageButton) return setPage(pageButton.dataset.page);
  const nav = event.target.closest('.nav-item');
  if (nav) return setPage(nav.dataset.page);
  const filter = event.target.closest('[data-filter]');
  if (filter) { loanFilter = filter.dataset.filter; return renderLoans(); }
  const action = event.target.closest('[data-action]');
  if (!action) return;
  if (action.dataset.action === 'quick-borrow') openBorrow();
  else if (action.dataset.action === 'borrow') openBorrow(action.dataset.book);
  else if (action.dataset.action === 'reserve') openReserve(action.dataset.book);
  else if (action.dataset.action === 'return') openReturn(action.dataset.loan);
  else if (action.dataset.action === 'cancel-reservation') cancelReservation(action.dataset.book, action.dataset.member);
});

async function cancelReservation(bookId, memberId) {
  if (!window.confirm(`Bỏ lượt chờ của ${personName(memberId)} cho sách ${bookId}?`)) return;
  try {
    const result = await api('/api/reservations/cancel', { method: 'POST', body: JSON.stringify({ bookId, memberId }) });
    await loadState(); showToast(result.message);
  } catch (error) { showToast(error.message, true); }
}

$('#actionForm').addEventListener('submit', submitAction);
$('#dialogCancel').addEventListener('click', () => dialog.close());
$('#dialogClose').addEventListener('click', () => dialog.close());
$('#refreshButton').addEventListener('click', () => loadState(true));
content.addEventListener('change', (event) => {
  if (event.target.id === 'topKWindowDays') {
    topKWindowDays = Number(event.target.value);
    loadState();
  } else if (event.target.id === 'activityLimit') {
    const requested = Number.parseInt(event.target.value, 10);
    activityLimit = Number.isFinite(requested) ? Math.max(1, Math.min(1000, requested)) : 10;
    renderPage();
  }
});
$('#mobileMenu').addEventListener('click', () => $('#sidebar').classList.toggle('open'));
dialog.addEventListener('close', () => { $('#actionForm').dataset.loanId = ''; $('#actionForm').dataset.bookId = ''; $('#actionForm').dataset.actionType = ''; });

loadState(true);
