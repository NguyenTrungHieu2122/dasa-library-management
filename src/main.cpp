#define NOMINMAX
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#ifdef _WIN32
#include <windows.h>
#endif

#include "dsa_core/repositories/BookRepository.h"
#include "dsa_core/repositories/LoanRepository.h"
#include "dsa_core/services/BorrowService.h"
#include "dsa_core/services/ReservationService.h"
#include "dsa_core/services/StatisticService.h"
#include "persistence/JsonDatabase.h"
#include "persistence/JsonValue.h"

namespace {
std::string nowText() {
    std::time_t now = std::time(nullptr); std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &now);
#else
    local = *std::localtime(&now);
#endif
    char buffer[20]; std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &local); return buffer;
}
int todayDate() {
    std::time_t now = std::time(nullptr); std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &now);
#else
    local = *std::localtime(&now);
#endif
    return (local.tm_year + 1900) * 10000 + (local.tm_mon + 1) * 100 + local.tm_mday;
}
std::string nextId(const std::vector<Loan>& loans) {
    int maxId = 0;
    for (const auto& loan : loans) {
        const std::string id = loan.getLoanId();
        if (id.size() > 1 && id[0] == 'L') try { maxId = (std::max)(maxId, std::stoi(id.substr(1))); } catch (...) {}
    }
    return "L" + std::to_string(maxId + 1);
}
std::string nextActivityId(const std::vector<Activity>& activities) {
    int maxId = 0;
    for (const auto& activity : activities) {
        const std::string id = activity.activityId;
        if (id.size() > 1 && id[0] == 'A') try { maxId = (std::max)(maxId, std::stoi(id.substr(1))); } catch (...) {}
    }
    return "A" + std::to_string(maxId + 1);
}
void syncBook(JsonDatabase& db, const Book& updated);
void recordActivity(JsonDatabase& db, const std::string& type, const std::string& bookId,
                    const std::string& memberId, const std::string& loanId, const std::string& detail);
Member* findMember(std::vector<Member>& members, const std::string& id);
Loan* findLoan(std::vector<Loan>& loans, const std::string& id);
std::string jsonString(const JsonValue& value) { return value.stringOr(); }
bool parseIsoDate(const std::string& value, int& result) {
    if (value.size() != 10 || value[4] != '-' || value[7] != '-') return false;
    int year = 0, month = 0, day = 0;
    try {
        year = std::stoi(value.substr(0, 4));
        month = std::stoi(value.substr(5, 2));
        day = std::stoi(value.substr(8, 2));
    } catch (...) { return false; }
    if (month < 1 || month > 12 || day < 1) return false;
    const bool leapYear = year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
    const int monthLengths[] = {31, leapYear ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (day > monthLengths[month - 1]) return false;
    result = year * 10000 + month * 100 + day;
    return true;
}
std::string dateText(int value) {
    std::ostringstream out;
    out << std::setfill('0') << std::setw(4) << value / 10000 << '-'
        << std::setw(2) << (value / 100) % 100 << '-' << std::setw(2) << value % 100;
    return out.str();
}
std::string plusDaysText(int days) {
    const auto now = std::chrono::system_clock::now();
    const auto later = now + std::chrono::hours(24 * days);
    const std::time_t time = std::chrono::system_clock::to_time_t(later);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &time);
#else
    local = *std::localtime(&time);
#endif
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &local);
    return buffer;
}
reservation* findReservation(std::vector<reservation>& reservations, const std::string& bookId) {
    for (auto& item : reservations) if (item.bookId == bookId) return &item;
    return nullptr;
}
bool reconcileLoanCopies(JsonDatabase& db) {
    bool changed = false;
    std::unordered_map<std::string, std::vector<size_t>> activeLoansByBook;
    activeLoansByBook.reserve(db.loans.size());
    for (size_t i = 0; i < db.loans.size(); ++i) {
        if (!db.loans[i].isReturned()) activeLoansByBook[db.loans[i].getBookId()].push_back(i);
    }
    for (auto& book : db.books) {
        reservation* waitingList = findReservation(db.reservations, book.getBookId());
        const std::string heldCopyId = waitingList ? waitingList->holdCopyId : "";
        std::vector<std::string> assignedCopyIds;
        const auto loansForBook = activeLoansByBook.find(book.getBookId());
        auto isAssigned = [&](const std::string& copyId) {
            return copyId == heldCopyId || std::find(assignedCopyIds.begin(), assignedCopyIds.end(), copyId) != assignedCopyIds.end();
        };
        if (loansForBook != activeLoansByBook.end()) for (size_t loanIndex : loansForBook->second) {
            auto& loan = db.loans[loanIndex];
            std::string selectedCopyId;
            BookCopy* indexedCopy = loan.getCopyId().empty() ? nullptr : book.findCopyById(loan.getCopyId());
            if (indexedCopy && !isAssigned(indexedCopy->getCopyId())) selectedCopyId = indexedCopy->getCopyId();
            if (selectedCopyId.empty()) {
                for (const auto& copyValue : book.getCopies()) {
                    if ((copyValue.getStatus() == "borrowed" || copyValue.getStatus() == "borrowing") && !isAssigned(copyValue.getCopyId())) {
                        selectedCopyId = copyValue.getCopyId();
                        break;
                    }
                }
            }
            if (selectedCopyId.empty()) {
                for (const auto& copyValue : book.getCopies()) {
                    if (copyValue.getStatus() == "available" && !isAssigned(copyValue.getCopyId())) {
                        selectedCopyId = copyValue.getCopyId();
                        break;
                    }
                }
            }
            if (!selectedCopyId.empty()) {
                if (loan.getCopyId() != selectedCopyId) { loan.setCopyId(selectedCopyId); changed = true; }
                assignedCopyIds.push_back(selectedCopyId);
            }
        }
        int availableCopies = 0;
        for (const auto& copyValue : book.getCopies()) {
            const std::string copyId = copyValue.getCopyId();
            const std::string desiredStatus = copyId == heldCopyId ? "reserved" : isAssigned(copyId) ? "borrowed" : "available";
            if (copyValue.getStatus() != desiredStatus) {
                BookCopy* copy = book.findCopyById(copyId);
                if (copy) copy->setStatus(desiredStatus);
                changed = true;
            }
            if (desiredStatus == "available") ++availableCopies;
        }
        if (book.getAvailableCopies() != availableCopies) {
            book.setAvailableCopies(availableCopies);
            changed = true;
        }
    }
    return changed;
}
std::string bridgeError(int status, const std::string& message) {
    return "{\"ok\":false,\"status\":" + std::to_string(status) + ",\"error\":" + jsonEscape(message) + "}";
}
void freeHeldCopy(Book* book, reservation& waitingList) {
    if (book && !waitingList.holdCopyId.empty()) {
        BookCopy* copy = book->findCopyById(waitingList.holdCopyId);
        if (copy && copy->getStatus() == "reserved") {
            copy->setStatus("available");
            book->setAvailableCopies(book->getAvailableCopies() + 1);
        }
    }
    waitingList.holdCopyId.clear();
    waitingList.holdUntil.clear();
}
void assignNextHold(Book* book, reservation& waitingList, std::vector<Member>& members) {
    if (!book || !waitingList.holdCopyId.empty() || book->getAvailableCopies() <= 0) return;
    while (!waitingList.q.isEmpty()) {
        const std::string memberId = waitingList.q.front().memberId;
        Member* member = findMember(members, memberId);
        if (member && member->canBorrow()) break;
        waitingList.q.dequeue();
    }
    if (waitingList.q.isEmpty()) return;
    for (const auto& copyValue : book->getCopies()) {
        if (copyValue.getStatus() == "available") {
            const std::string copyId = copyValue.getCopyId();
            BookCopy* copy = book->findCopyById(copyId);
            if (!copy) return;
            copy->setStatus("reserved");
            book->setAvailableCopies(book->getAvailableCopies() - 1);
            waitingList.holdCopyId = copyId;
            waitingList.holdUntil = plusDaysText(2);
            return;
        }
    }
}
bool expireHolds(JsonDatabase& db, BookRepository& books) {
    const std::string now = nowText();
    bool changed = false;
    for (auto& waitingList : db.reservations) {
        if (waitingList.holdUntil.empty() || waitingList.holdUntil > now) continue;
        Book* book = books.findById(waitingList.bookId);
        freeHeldCopy(book, waitingList);
        if (!waitingList.q.isEmpty()) waitingList.q.dequeue();
        assignNextHold(book, waitingList, db.members);
        if (book) syncBook(db, *book);
        changed = true;
    }
    for (auto& waitingList : db.reservations) {
        if (!waitingList.holdCopyId.empty() || waitingList.q.isEmpty()) continue;
        Book* book = books.findById(waitingList.bookId);
        const std::string priorHold = waitingList.holdCopyId;
        assignNextHold(book, waitingList, db.members);
        if (waitingList.holdCopyId != priorHold) {
            if (book) syncBook(db, *book);
            changed = true;
        }
    }
    return changed;
}
std::string runBridgeRequest(const JsonValue& request, const std::string& dataDirectory,
                             JsonDatabase& db, BookRepository& books,
                             LoanRepository& dueIndex, StatisticService& stats) {
    const std::string action = jsonString(request["action"]);
    if (expireHolds(db, books)) db.save(dataDirectory);
    if (action == "refresh") {
        int topKWindowDays = request["windowDays"].intOr(30);
        if (topKWindowDays < 1) topKWindowDays = 1;
        if (topKWindowDays > 3650) topKWindowDays = 3650;
        std::string response = "{\"ok\":true,\"topKWindowDays\":" +
            std::to_string(topKWindowDays) + ",\"topBooks\":[";
        const auto topBooks = stats.getMostPopularBooks(5, topKWindowDays, todayDate());
        for (size_t i = 0; i < topBooks.size(); ++i) {
            if (i) response += ',';
            Book* book = books.findById(topBooks[i].bookId);
            response += "{\"bookId\":" + jsonEscape(topBooks[i].bookId) + ",\"title\":" +
                jsonEscape(book ? book->getTitle() : "") + ",\"borrowCount\":" + std::to_string(topBooks[i].borrowCount) + "}";
        }
        response += "],\"dueLoans\":[";
        const auto dueLoans = dueIndex.getLoansDueInRange(0, 99999999);
        for (size_t i = 0; i < dueLoans.size(); ++i) {
            if (i) response += ',';
            const Loan& loan = dueLoans[i];
            response += "{\"loanId\":" + jsonEscape(loan.getLoanId()) + ",\"bookId\":" +
                jsonEscape(loan.getBookId()) + ",\"memberId\":" + jsonEscape(loan.getMemberId()) +
                ",\"dueDate\":" + jsonEscape(dateText(loan.getDueDate())) + ",\"returnDate\":null}";
        }
        return response + "]}";
    }
    if (action == "borrow") {
        const std::string bookId = jsonString(request["bookId"]), copyId = jsonString(request["copyId"]);
        const std::string memberId = jsonString(request["memberId"]);
        int borrowDate = 0, dueDate = 0;
        if (!parseIsoDate(jsonString(request["borrowDate"]), borrowDate) ||
            !parseIsoDate(jsonString(request["dueDate"]), dueDate) || dueDate <= borrowDate)
            return bridgeError(400, "Ngày mượn và hạn trả không hợp lệ.");
        Book* book = books.findById(bookId); Member* member = findMember(db.members, memberId);
        if (!book || !member) return bridgeError(404, "Không tìm thấy sách hoặc thành viên.");
        reservation* waitingList = findReservation(db.reservations, bookId);
        const bool isHeldCopy = waitingList && !waitingList->holdCopyId.empty();
        if (waitingList && !waitingList->q.isEmpty() && waitingList->q.front().memberId != memberId)
            return bridgeError(409, "Sách đang được giữ cho thành viên " + waitingList->q.front().memberId + ".");
        if (isHeldCopy && waitingList->holdCopyId != copyId)
            return bridgeError(409, "Bản sao được giữ có mã " + waitingList->holdCopyId + ".");
        BookCopy* copy = book->findCopyById(copyId);
        if (isHeldCopy && copy && copy->getStatus() == "reserved") {
            copy->setStatus("available");
            book->setAvailableCopies(book->getAvailableCopies() + 1);
        }
        Loan loan; const std::string loanId = nextId(db.loans);
        BorrowService service;
        if (!service.borrowBook(*book, copyId, *member, loan, loanId, borrowDate, dueDate)) {
            if (isHeldCopy && copy) { copy->setStatus("reserved"); book->setAvailableCopies(book->getAvailableCopies() - 1); }
            return bridgeError(409, "Không thể mượn: kiểm tra bản sao, trạng thái thành viên và giới hạn mượn.");
        }
        if (waitingList && !waitingList->q.isEmpty()) waitingList->q.dequeue();
        if (waitingList) { waitingList->holdCopyId.clear(); waitingList->holdUntil.clear(); }
        if (waitingList && !waitingList->q.isEmpty()) assignNextHold(book, *waitingList, db.members);
        syncBook(db, *book); db.loans.push_back(loan); dueIndex.addLoan(loan);
        stats.recordBorrow(bookId, borrowDate);
        recordActivity(db, "borrow", bookId, memberId, loanId, "Mượn sách " + bookId);
        db.save(dataDirectory);
        return "{\"ok\":true,\"message\":\"Mượn sách thành công. Mã phiếu: " + loanId + "\",\"loanId\":" + jsonEscape(loanId) + "}";
    }
    if (action == "return") {
        const std::string loanId = jsonString(request["loanId"]), copyId = jsonString(request["copyId"]);
        int returnDate = 0;
        if (!parseIsoDate(jsonString(request["returnDate"]), returnDate)) return bridgeError(400, "Ngày trả không hợp lệ.");
        Loan* loan = findLoan(db.loans, loanId);
        if (!loan || loan->isReturned()) return bridgeError(404, "Không tìm thấy phiếu đang mượn.");
        Book* book = books.findById(loan->getBookId()); Member* member = findMember(db.members, loan->getMemberId());
        if (!book || !member) return bridgeError(409, "Không tìm thấy sách hoặc thành viên của phiếu mượn.");
        BorrowService service;
        if (!service.returnBook(*book, copyId, *member, *loan, returnDate))
            return bridgeError(409, "Không thể trả: kiểm tra bản sao và thông tin phiếu.");
        dueIndex.removeLoan(loan->getDueDate(), loanId);
        reservation* waitingList = findReservation(db.reservations, book->getBookId());
        std::string message = "Trả sách thành công.";
        if (waitingList && !waitingList->q.isEmpty()) {
            assignNextHold(book, *waitingList, db.members);
            if (!waitingList->holdCopyId.empty())
                message += " Bản sao " + waitingList->holdCopyId + " được giữ đến " + waitingList->holdUntil + " cho thành viên " + waitingList->q.front().memberId + ".";
        }
        syncBook(db, *book);
        recordActivity(db, "return", book->getBookId(), member->getMemberId(), loanId, "Trả sách " + book->getBookId());
        db.save(dataDirectory);
        return "{\"ok\":true,\"message\":" + jsonEscape(message) + "}";
    }
    if (action == "reserve") {
        const std::string bookId = jsonString(request["bookId"]), memberId = jsonString(request["memberId"]);
        Book* book = books.findById(bookId); Member* member = findMember(db.members, memberId);
        if (!book || !member) return bridgeError(404, "Không tìm thấy sách hoặc thành viên.");
        if (member->getStatus() != "active") return bridgeError(409, "Thành viên hiện không hoạt động.");
        if (book->getAvailableCopies() > 0) return bridgeError(409, "Sách vẫn còn bản sẵn sàng, chưa cần vào hàng chờ.");
        reservation* waitingList = findReservation(db.reservations, bookId);
        if (!waitingList) { db.reservations.emplace_back(); waitingList = &db.reservations.back(); waitingList->bookId = bookId; }
        Queue<reservationEntry> pending = waitingList->q;
        while (!pending.isEmpty()) if (pending.dequeue().memberId == memberId) return bridgeError(409, "Thành viên đã có trong hàng chờ sách này.");
        registerRes(*waitingList, memberId, nowText());
        recordActivity(db, "reserve", bookId, memberId, "", "Đăng ký chờ mượn " + bookId);
        db.save(dataDirectory);
        return "{\"ok\":true,\"message\":\"Đã thêm vào hàng chờ, vị trí " + std::to_string(getwaitingcount(*waitingList)) + ".\"}";
    }
    if (action == "cancel-reservation") {
        const std::string bookId = jsonString(request["bookId"]), memberId = jsonString(request["memberId"]);
        reservation* waitingList = findReservation(db.reservations, bookId);
        if (!waitingList) return bridgeError(404, "Không tìm thấy hàng chờ của sách này.");
        const bool wasHolder = !waitingList->q.isEmpty() && waitingList->q.front().memberId == memberId && !waitingList->holdCopyId.empty();
        std::string id = memberId;
        if (!cancelRes(*waitingList, id)) return bridgeError(404, "Không tìm thấy thành viên trong hàng chờ.");
        Book* book = books.findById(bookId);
        if (wasHolder) { freeHeldCopy(book, *waitingList); assignNextHold(book, *waitingList, db.members); if (book) syncBook(db, *book); }
        db.save(dataDirectory);
        return "{\"ok\":true,\"message\":\"Đã bỏ lượt chờ của thành viên " + jsonEscape(memberId) + "\"}";
    }
    return bridgeError(400, "Thao tác không được hỗ trợ.");
}
int runWebBridge(const std::string& dataDirectory) {
    JsonDatabase db;
    try { db.load(dataDirectory); }
    catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
    if (db.activityHistoryNeedsSave) {
        try { db.save(dataDirectory); }
        catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
    }
    if (reconcileLoanCopies(db)) db.save(dataDirectory);
    BookRepository books;
    StatisticService stats;
    for (const auto& book : db.books) { books.addBook(book); stats.registerBook(book.getBookId()); }
    LoanRepository dueIndex;
    for (const auto& loan : db.loans) if (!loan.isReturned()) dueIndex.addLoan(loan);
    for (const auto& loan : db.loans)
        stats.recordBorrow(loan.getBookId(), loan.getBorrowDate());
    std::string line;
    while (std::getline(std::cin, line)) {
        try {
            const JsonValue request = JsonParser(line).parse();
            std::cout << runBridgeRequest(request, dataDirectory, db, books, dueIndex, stats) << std::endl;
        } catch (const std::exception& error) {
            std::cout << bridgeError(400, error.what()) << std::endl;
        }
    }
    return 0;
}
Book* findBook(BookRepository& books, const std::string& id) { return books.findById(id); }
void syncBook(JsonDatabase& db, const Book& updated) {
    for (auto& book : db.books) if (book.getBookId() == updated.getBookId()) { book = updated; return; }
}
Member* findMember(std::vector<Member>& members, const std::string& id) {
    for (auto& member : members) if (member.getMemberId() == id) return &member;
    return nullptr;
}
Loan* findLoan(std::vector<Loan>& loans, const std::string& id) {
    for (auto& loan : loans) if (loan.getLoanId() == id) return &loan;
    return nullptr;
}
void recordActivity(JsonDatabase& db, const std::string& type, const std::string& bookId,
                    const std::string& memberId, const std::string& loanId, const std::string& detail) {
    Activity item; item.activityId = nextActivityId(db.activities); item.type = type; item.bookId = bookId;
    item.memberId = memberId; item.loanId = loanId; item.time = nowText(); item.detail = detail;
    retainRecentActivity(db.activities, std::move(item));
}
void showMenu() {
    std::cout << "\n===== QUAN LY THU VIEN =====\n"
              << "1. Danh sach sach\n2. Tim sach theo ma\n3. Danh sach thanh vien\n"
              << "4. Muon sach\n5. Tra sach\n6. Dang ky cho muon\n"
              << "7. Top sach duoc muon\n8. Phieu sap den han\n9. Hoat dong gan day\n0. Luu va thoat\n"
              << "Chon: ";
}
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    if (argc > 1 && std::string(argv[1]) == "--web-bridge") {
        const std::string dataDirectory = argc > 2 ? argv[2] : "data";
        return runWebBridge(dataDirectory);
    }
    const std::string dataDirectory = argc > 1 ? argv[1] : "data";
    JsonDatabase db;
    try { db.load(dataDirectory); }
    catch (const std::exception& error) {
        std::cerr << "Khong the nap du lieu: " << error.what() << "\n"
                  << "Hay chay chuong trinh tu thu muc goc du an de tim thay thu muc data/.\n";
        return 1;
    }
    if (db.activityHistoryNeedsSave) {
        try { db.save(dataDirectory); }
        catch (const std::exception& error) { std::cerr << "Loi gioi han lich su hoat dong: " << error.what() << '\n'; return 1; }
    }
    if (reconcileLoanCopies(db)) {
        try { db.save(dataDirectory); }
        catch (const std::exception& error) { std::cerr << "Loi dong bo phieu muon: " << error.what() << '\n'; return 1; }
    }

    BookRepository books;
    StatisticService stats;
    for (const auto& book : db.books) { books.addBook(book); stats.registerBook(book.getBookId()); }
    LoanRepository dueIndex;
    for (const auto& loan : db.loans) if (!loan.isReturned()) dueIndex.addLoan(loan);
    for (const auto& loan : db.loans)
        stats.recordBorrow(loan.getBookId(), loan.getBorrowDate());

    BorrowService borrowService;
    bool running = true;
    while (running) {
        if (expireHolds(db, books)) db.save(dataDirectory);
        showMenu(); int choice;
        if (!(std::cin >> choice)) {
            if (std::cin.eof()) { running = false; continue; }
            std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); continue;
        }
        if (choice == 0) { running = false; continue; }
        if (choice == 1) {
            for (const auto& book : db.books)
                std::cout << book.getBookId() << " | " << book.getTitle() << " | con " << book.getAvailableCopies() << '/' << book.getTotalCopies() << " ban\n";
        } else if (choice == 2) {
            std::string id; std::cout << "Ma sach: "; std::cin >> id; Book* book = findBook(books, id);
            if (!book) std::cout << "Khong tim thay sach.\n";
            else std::cout << book->getBookId() << " | " << book->getTitle() << " | " << book->getAuthor()
                           << " | " << book->getCategory() << " | con " << book->getAvailableCopies() << " ban\n";
        } else if (choice == 3) {
            for (const auto& member : db.members)
                std::cout << member.getMemberId() << " | " << member.getFullname() << " | " << member.getStatus()
                          << " | dang muon " << member.getTotalBorrowBooks() << '/' << member.getMaxBorrow() << " quyen\n";
        } else if (choice == 4) {
            std::string bookId, copyId, memberId; int borrowDate, dueDate;
            std::cout << "Ma sach, ma ban sao, ma thanh vien, ngay muon (YYYYMMDD), han tra (YYYYMMDD): ";
            std::cin >> bookId >> copyId >> memberId >> borrowDate >> dueDate;
            Book* book = findBook(books, bookId); Member* member = findMember(db.members, memberId);
            if (!book || !member) { std::cout << "Khong tim thay sach hoac thanh vien.\n"; continue; }
            reservation* waitingList = nullptr;
            for (auto& item : db.reservations) if (item.bookId == bookId) { waitingList = &item; break; }
            if (waitingList && !waitingList->q.isEmpty() && waitingList->q.front().memberId != memberId) {
                std::cout << "Sach dang co hang doi; thanh vien dung dau hang doi la " << waitingList->q.front().memberId << ".\n";
                continue;
            }
            const bool isHeldCopy = waitingList && !waitingList->holdCopyId.empty();
            if (isHeldCopy && waitingList->holdCopyId != copyId) {
                std::cout << "Ban sao duoc giu cho hang doi co ma " << waitingList->holdCopyId << ".\n";
                continue;
            }
            BookCopy* heldCopy = isHeldCopy ? book->findCopyById(copyId) : nullptr;
            if (heldCopy && heldCopy->getStatus() == "reserved") {
                heldCopy->setStatus("available"); book->setAvailableCopies(book->getAvailableCopies() + 1);
            }
            Loan loan; std::string id = nextId(db.loans);
            if (!borrowService.borrowBook(*book, copyId, *member, loan, id, borrowDate, dueDate)) {
                if (heldCopy && heldCopy->getStatus() == "available") {
                    heldCopy->setStatus("reserved"); book->setAvailableCopies(book->getAvailableCopies() - 1);
                }
                std::cout << "Khong the muon: kiem tra ban sao, trang thai thanh vien va gioi han muon.\n"; continue;
            }
            if (waitingList && !waitingList->q.isEmpty()) waitingList->q.dequeue();
            if (waitingList) { waitingList->holdCopyId.clear(); waitingList->holdUntil.clear(); }
            if (waitingList && !waitingList->q.isEmpty()) assignNextHold(book, *waitingList, db.members);
            syncBook(db, *book);
            db.loans.push_back(loan); dueIndex.addLoan(loan);
            stats.recordBorrow(bookId, borrowDate);
            recordActivity(db, "borrow", bookId, memberId, id, "Muon sach " + bookId);
            db.save(dataDirectory); std::cout << "Muon sach thanh cong. Ma phieu: " << id << "\n";
        } else if (choice == 5) {
            std::string loanId, copyId; int returnDate;
            std::cout << "Ma phieu muon, ma ban sao, ngay tra (YYYYMMDD): "; std::cin >> loanId >> copyId >> returnDate;
            Loan* loan = findLoan(db.loans, loanId);
            if (!loan || loan->isReturned()) { std::cout << "Khong tim thay phieu dang muon.\n"; continue; }
            Book* book = findBook(books, loan->getBookId()); Member* member = findMember(db.members, loan->getMemberId());
            if (!book || !member || !borrowService.returnBook(*book, copyId, *member, *loan, returnDate)) {
                std::cout << "Khong the tra sach: kiem tra ma ban sao va thong tin phieu.\n"; continue;
            }
            dueIndex.removeLoan(loan->getDueDate(), loanId);
            reservation* waitingList = findReservation(db.reservations, book->getBookId());
            if (waitingList && !waitingList->q.isEmpty()) {
                assignNextHold(book, *waitingList, db.members);
                if (!waitingList->holdCopyId.empty())
                    std::cout << "Da giu ban sao " << waitingList->holdCopyId << " den " << waitingList->holdUntil
                              << " cho thanh vien " << waitingList->q.front().memberId << ".\n";
            }
            syncBook(db, *book);
            recordActivity(db, "return", book->getBookId(), member->getMemberId(), loanId, "Tra sach " + book->getBookId());
            db.save(dataDirectory); std::cout << "Tra sach thanh cong.\n";
        } else if (choice == 6) {
            std::string bookId, memberId; std::cout << "Ma sach va ma thanh vien: "; std::cin >> bookId >> memberId;
            Book* book = findBook(books, bookId);
            if (!book || !findMember(db.members, memberId)) { std::cout << "Khong tim thay sach hoac thanh vien.\n"; continue; }
            if (book->getAvailableCopies() > 0) { std::cout << "Sach con ban san sang; chua can vao hang doi.\n"; continue; }
            reservation* res = nullptr;
            for (auto& item : db.reservations) if (item.bookId == bookId) { res = &item; break; }
            if (!res) { db.reservations.emplace_back(); res = &db.reservations.back(); res->bookId = bookId; }
            bool alreadyWaiting = false; Queue<reservationEntry> q = res->q;
            while (!q.isEmpty()) if (q.dequeue().memberId == memberId) alreadyWaiting = true;
            if (alreadyWaiting) { std::cout << "Thanh vien da co trong hang doi sach nay.\n"; continue; }
            registerRes(*res, memberId, nowText());
            Activity item; item.activityId = nextActivityId(db.activities); item.type = "reserve"; item.bookId = bookId;
            item.memberId = memberId; item.time = nowText(); item.detail = "Dang ky cho muon " + bookId;
            retainRecentActivity(db.activities, std::move(item));
            db.save(dataDirectory); std::cout << "Da them vao hang doi; vi tri: " << getwaitingcount(*res) << "\n";
        } else if (choice == 7) {
            int k, windowDays;
            std::cout << "So luong top K va so ngay N: "; std::cin >> k >> windowDays;
            for (const auto& item : stats.getMostPopularBooks(k, windowDays, todayDate())) {
                Book* book = findBook(books, item.bookId);
                std::cout << item.bookId << " | " << (book ? book->getTitle() : "")
                          << " | " << item.borrowCount << " luot trong " << windowDays << " ngay\n";
            }
        } else if (choice == 8) {
            int startDate, endDate; std::cout << "Tu ngay den ngay (YYYYMMDD): "; std::cin >> startDate >> endDate;
            if (startDate > endDate) { std::cout << "Khoang ngay khong hop le.\n"; continue; }
            for (const auto& loan : dueIndex.getLoansDueInRange(startDate, endDate))
                std::cout << loan.getLoanId() << " | " << loan.getBookId() << " | thanh vien " << loan.getMemberId() << " | han " << loan.getDueDate() << '\n';
        } else if (choice == 9) {
            int count = 0;
            for (auto it = db.activities.rbegin(); it != db.activities.rend() && count < 10; ++it, ++count)
                std::cout << it->time << " | " << it->detail << " | " << it->memberId << '\n';
        } else std::cout << "Lua chon khong hop le.\n";
    }
    try { db.save(dataDirectory); }
    catch (const std::exception& error) { std::cerr << "Loi luu du lieu: " << error.what() << '\n'; return 1; }
    std::cout << "Da luu du lieu. Tam biet!\n";
    return 0;
}
