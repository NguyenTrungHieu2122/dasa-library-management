#include "JsonDatabase.h"
#include "JsonValue.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <ctime>
#include <utility>

namespace {
std::string readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Khong mo duoc tep: " + path);
    std::ostringstream contents; contents << file.rdbuf(); return contents.str();
}
JsonValue readJson(const std::string& path) { return JsonParser(readFile(path)).parse(); }
std::ofstream outputFile(const std::string& path) {
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file) throw std::runtime_error("Khong ghi duoc tep: " + path);
    return file;
}
int parseDate(const std::string& date) {
    int a = 0, b = 0, c = 0; char first = 0, second = 0;
    std::istringstream input(date); input >> a >> first >> b >> second >> c;
    if (!input || first != second || (first != '-' && first != '/')) return 0;
    if (a > 999 && b >= 1 && b <= 12) return a * 10000 + b * 100 + c;
    return c * 10000 + b * 100 + a;
}
std::string formatDate(int date) {
    if (date <= 0) return "";
    std::ostringstream out; out << std::setfill('0') << std::setw(4) << date / 10000 << '-'
        << std::setw(2) << (date / 100) % 100 << '-' << std::setw(2) << date % 100;
    return out.str();
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
std::string value(const JsonValue& object, const std::string& key, const std::string& fallback = "") {
    return object[key].stringOr(fallback);
}
int number(const JsonValue& object, const std::string& key, int fallback = 0) {
    return object[key].intOr(fallback);
}
}

void JsonDatabase::load(const std::string& dataDirectory) {
    books.clear(); members.clear(); loans.clear(); reservations.clear(); activities.clear();
    activityHistoryNeedsSave = false;
    reservationQueueNeedsSave = false;
    const std::string base = dataDirectory + "/";

    JsonValue bookRows = readJson(base + "books.json");
    if (!bookRows.isArray()) throw std::runtime_error("books.json phai la mot JSON array");
    for (size_t i = 0; i < bookRows.size(); ++i) {
        const JsonValue& row = bookRows[i];
        Book book(value(row, "bookId"), value(row, "title"), value(row, "author"), value(row, "publisher"),
                  number(row, "publishYear"), value(row, "category"), number(row, "totalCopies"),
                  number(row, "availableCopies"), number(row, "borrowCount"));
        const JsonValue& copies = row["copies"];
        for (size_t j = 0; j < copies.size(); ++j)
            book.addCopy(BookCopy(value(copies[j], "copyId"), value(copies[j], "status", "available")));
        books.push_back(book);
    }

    JsonValue memberRows = readJson(base + "members.json");
    if (!memberRows.isArray()) throw std::runtime_error("members.json phai la mot JSON array");
    for (size_t i = 0; i < memberRows.size(); ++i) {
        const JsonValue& row = memberRows[i];
        members.emplace_back(value(row, "memberId"), value(row, "fullname"), value(row, "phone"),
            value(row, "joinDate"), value(row, "status", "active"), number(row, "totalBorrowBooks"), number(row, "maxBorrow", 5));
    }

    JsonValue loanRows = readJson(base + "loans.json");
    if (!loanRows.isArray()) throw std::runtime_error("loans.json phai la mot JSON array");
    for (size_t i = 0; i < loanRows.size(); ++i) {
        const JsonValue& row = loanRows[i];
        Loan loan(value(row, "loanId"), value(row, "bookId"), value(row, "memberId"),
                  parseDate(value(row, "borrowDate")), parseDate(value(row, "dueDate")), value(row, "copyId"));
        const std::string returned = value(row, "returnDate");
        if (!returned.empty()) loan.markAsReturned(parseDate(returned));
        loans.push_back(loan);
    }

    JsonValue reservationRows = readJson(base + "reservations.json");
    if (!reservationRows.isArray()) throw std::runtime_error("reservations.json phai la mot JSON array");
    for (size_t i = 0; i < reservationRows.size(); ++i) {
        reservation res; res.bookId = value(reservationRows[i], "bookId");
        const JsonValue& queue = reservationRows[i]["queue"];
        for (size_t j = 0; j < queue.size(); ++j)
            res.q.enqueue({value(queue[j], "memberId"), value(queue[j], "reservedAt")});
        const JsonValue& holds = reservationRows[i]["holds"];
        if (holds.isArray()) {
            for (size_t j = 0; j < holds.size(); ++j) {
                reservation::Hold hold{value(holds[j], "memberId"), value(holds[j], "copyId"), value(holds[j], "holdUntil")};
                if (!hold.memberId.empty() && !hold.copyId.empty()) res.holds.push_back(std::move(hold));
            }
        } else {
            // Migrate the previous single-holder format. Earlier data kept that
            // holder at the front of the queue; move them out of the wait list.
            std::string memberId = value(reservationRows[i], "holdMemberId");
            const std::string copyId = value(reservationRows[i], "holdCopyId");
            if (memberId.empty() && !copyId.empty() && !res.q.isEmpty()) memberId = res.q.dequeue().memberId;
            if (!memberId.empty() && !copyId.empty())
                res.holds.push_back({memberId, copyId, value(reservationRows[i], "holdUntil")});
            reservationQueueNeedsSave = true;
        }
        reservations.push_back(res);
    }

    JsonValue activityRows = readJson(base + "activities.json");
    if (!activityRows.isArray()) throw std::runtime_error("activities.json phai la mot JSON array");
    for (size_t i = 0; i < activityRows.size(); ++i) {
        const JsonValue& row = activityRows[i]; Activity activity;
        activity.activityId = value(row, "activityId"); activity.type = value(row, "type");
        activity.bookId = value(row, "bookId"); activity.memberId = value(row, "memberId");
        activity.loanId = value(row, "loanId"); activity.reservationId = value(row, "reservationId");
        activity.time = value(row, "time"); activity.detail = value(row, "detail");
        activities.push_back(activity);
    }
    const size_t loadedActivityCount = activities.size();
    trimActivitiesToRecent(activities);
    activityHistoryNeedsSave = activities.size() != loadedActivityCount;
}

void JsonDatabase::save(const std::string& dataDirectory) const {
    std::filesystem::create_directories(dataDirectory);
    {
        auto out = outputFile(dataDirectory + "/books.json"); out << "[\n";
        for (size_t i = 0; i < books.size(); ++i) {
            const Book& b = books[i]; out << "  {\"bookId\":" << jsonEscape(b.getBookId()) << ",\"title\":" << jsonEscape(b.getTitle())
                << ",\"author\":" << jsonEscape(b.getAuthor()) << ",\"publisher\":" << jsonEscape(b.getPublisher())
                << ",\"publishYear\":" << b.getPublishYear() << ",\"category\":" << jsonEscape(b.getCategory())
                << ",\"totalCopies\":" << b.getTotalCopies() << ",\"availableCopies\":" << b.getAvailableCopies()
                << ",\"borrowCount\":" << b.getBorrowCount() << ",\"copies\":[";
            auto copies = b.getCopies();
            for (size_t j = 0; j < copies.size(); ++j) {
                if (j) out << ',';
                out << "{\"copyId\":" << jsonEscape(copies[j].getCopyId()) << ",\"status\":" << jsonEscape(copies[j].getStatus()) << '}';
            }
            out << "]}" << (i + 1 == books.size() ? "\n" : ",\n");
        }
        out << "]\n";
    }
    {
        auto out = outputFile(dataDirectory + "/members.json"); out << "[\n";
        for (size_t i = 0; i < members.size(); ++i) {
            const Member& m = members[i]; out << "  {\"memberId\":" << jsonEscape(m.getMemberId()) << ",\"fullname\":" << jsonEscape(m.getFullname())
                << ",\"phone\":" << jsonEscape(m.getPhone()) << ",\"joinDate\":" << jsonEscape(m.getJoinDate())
                << ",\"status\":" << jsonEscape(m.getStatus()) << ",\"totalBorrowBooks\":" << m.getTotalBorrowBooks()
                << ",\"maxBorrow\":" << m.getMaxBorrow() << '}' << (i + 1 == members.size() ? "\n" : ",\n");
        }
        out << "]\n";
    }
    {
        auto out = outputFile(dataDirectory + "/loans.json"); out << "[\n";
        for (size_t i = 0; i < loans.size(); ++i) {
            const Loan& l = loans[i]; out << "  {\"loanId\":" << jsonEscape(l.getLoanId()) << ",\"bookId\":" << jsonEscape(l.getBookId())
                << ",\"memberId\":" << jsonEscape(l.getMemberId()) << ",\"copyId\":" << jsonEscape(l.getCopyId())
                << ",\"borrowDate\":" << jsonEscape(formatDate(l.getBorrowDate()))
                << ",\"returnDate\":" << (l.isReturned() ? jsonEscape(formatDate(l.getReturnDate())) : "null")
                << ",\"dueDate\":" << jsonEscape(formatDate(l.getDueDate())) << ",\"status\":"
                << jsonEscape(l.getStatus(todayDate()) == LoanStatus::RETURNED ? "returned" :
                              l.getStatus(todayDate()) == LoanStatus::OVERDUE ? "overdue" : "borrowing")
                << '}' << (i + 1 == loans.size() ? "\n" : ",\n");
        }
        out << "]\n";
    }
    {
        auto out = outputFile(dataDirectory + "/reservations.json"); out << "[\n";
        for (size_t i = 0; i < reservations.size(); ++i) {
            out << "  {\"bookId\":" << jsonEscape(reservations[i].bookId) << ",\"holds\":[";
            for (size_t j = 0; j < reservations[i].holds.size(); ++j) {
                if (j) out << ',';
                const auto& hold = reservations[i].holds[j];
                out << "{\"memberId\":" << jsonEscape(hold.memberId) << ",\"copyId\":" << jsonEscape(hold.copyId)
                    << ",\"holdUntil\":" << jsonEscape(hold.holdUntil) << '}';
            }
            out << "],\"queue\":[";
            Queue<reservationEntry> q = reservations[i].q; bool first = true;
            while (!q.isEmpty()) { auto entry = q.dequeue(); if (!first) out << ','; first = false;
                out << "{\"memberId\":" << jsonEscape(entry.memberId) << ",\"reservedAt\":" << jsonEscape(entry.reservedAt) << '}'; }
            out << "]}" << (i + 1 == reservations.size() ? "\n" : ",\n");
        }
        out << "]\n";
    }
    {
        auto out = outputFile(dataDirectory + "/activities.json"); out << "[\n";
        for (size_t i = 0; i < activities.size(); ++i) {
            const Activity& a = activities[i]; out << "  {\"activityId\":" << jsonEscape(a.activityId) << ",\"type\":" << jsonEscape(a.type)
                << ",\"bookId\":" << jsonEscape(a.bookId) << ",\"memberId\":" << jsonEscape(a.memberId)
                << ",\"loanId\":" << jsonEscape(a.loanId) << ",\"reservationId\":" << jsonEscape(a.reservationId)
                << ",\"time\":" << jsonEscape(a.time) << ",\"detail\":" << jsonEscape(a.detail) << '}'
                << (i + 1 == activities.size() ? "\n" : ",\n");
        }
        out << "]\n";
    }
}
