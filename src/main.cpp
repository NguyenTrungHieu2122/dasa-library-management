#include <algorithm>
#include <ctime>
#include <iostream>
#include <limits>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif

#include "dsa_core/repositories/BookRepository.h"
#include "dsa_core/repositories/LoanRepository.h"
#include "dsa_core/services/BorrowService.h"
#include "dsa_core/services/ReservationService.h"
#include "dsa_core/services/StatisticService.h"
#include "persistence/JsonDatabase.h"

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
std::string nextId(const std::vector<Loan>& loans) {
    int maxId = 0;
    for (const auto& loan : loans) {
        const std::string id = loan.getLoanId();
        if (id.size() > 1 && id[0] == 'L') try { maxId = std::max(maxId, std::stoi(id.substr(1))); } catch (...) {}
    }
    return "L" + std::to_string(maxId + 1);
}
std::string nextActivityId(const std::vector<Activity>& activities) {
    int maxId = 0;
    for (const auto& activity : activities) {
        const std::string id = activity.activityId;
        if (id.size() > 1 && id[0] == 'A') try { maxId = std::max(maxId, std::stoi(id.substr(1))); } catch (...) {}
    }
    return "A" + std::to_string(maxId + 1);
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
    db.activities.push_back(item);
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
    const std::string dataDirectory = argc > 1 ? argv[1] : "data";
    JsonDatabase db;
    try { db.load(dataDirectory); }
    catch (const std::exception& error) {
        std::cerr << "Khong the nap du lieu: " << error.what() << "\n"
                  << "Hay chay chuong trinh tu thu muc goc du an de tim thay thu muc data/.\n";
        return 1;
    }

    BookRepository books;
    for (const auto& book : db.books) books.addBook(book);
    LoanRepository dueIndex;
    StatisticService stats;
    for (const auto& loan : db.loans) if (!loan.isReturned()) dueIndex.addLoan(loan);
    for (const auto& book : db.books) if (book.getBorrowCount() > 0) stats.recordBorrow(book.getBookId());
    // JSON book records store the lifetime total, so the statistic heap is initialized to that total.
    for (const auto& book : db.books) {
        if (book.getBorrowCount() > 1) {
            // recordBorrow starts at one; add the remaining count through repeated service calls.
            for (int i = 1; i < book.getBorrowCount(); ++i) stats.recordBorrow(book.getBookId());
        }
    }

    BorrowService borrowService;
    bool running = true;
    while (running) {
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
            Loan loan; std::string id = nextId(db.loans);
            if (!borrowService.borrowBook(*book, copyId, *member, loan, id, borrowDate, dueDate)) {
                std::cout << "Khong the muon: kiem tra ban sao, trang thai thanh vien va gioi han muon.\n"; continue;
            }
            if (waitingList && !waitingList->q.isEmpty()) waitingList->q.dequeue();
            syncBook(db, *book);
            db.loans.push_back(loan); dueIndex.addLoan(loan); stats.recordBorrow(bookId);
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
            item.memberId = memberId; item.time = nowText(); item.detail = "Dang ky cho muon " + bookId; db.activities.push_back(item);
            db.save(dataDirectory); std::cout << "Da them vao hang doi; vi tri: " << getwaitingcount(*res) << "\n";
        } else if (choice == 7) {
            int k; std::cout << "So luong top K: "; std::cin >> k;
            for (const auto& item : stats.getMostPopularBooks(k)) {
                Book* book = findBook(books, item.bookId);
                std::cout << item.bookId << " | " << (book ? book->getTitle() : "") << " | " << item.borrowCount << " luot\n";
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
