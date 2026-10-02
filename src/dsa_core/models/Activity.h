#pragma once
#include <string>
struct Activity {
    std::string activityId;
    std::string type;          
    std::string bookId;
    std::string memberId;
    std::string loanId;        
    std::string reservationId; 
    std::string time;
    std::string detail;
};
