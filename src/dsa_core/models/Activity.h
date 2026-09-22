#pragma once

#include <string>
 
// Thong tin 1 hoat dong da xay ra (muon / tra / dat truoc)
struct Activity {
    std::string activityId;
    std::string type;          // "borrow", "return", "reserve"
    std::string bookId;
    std::string memberId;
    std::string loanId;        // de rong "" neu type = "reserve"
    std::string reservationId; // de rong "" neu type = "borrow"/"return"
    std::string time;
    std::string detail;
};
