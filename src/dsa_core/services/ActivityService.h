#pragma once
 
#include <string>
#include <vector>
#include "../models/Activity.h" 
 
const int DEFAULT_RECENT_COUNT = 10;
 
struct ActivityServiceState {
    std::vector<Activity> allActivities;
    std::string filepath;
};
 
ActivityServiceState initActivityService(const std::string &filepath = "data/activities.json");
 
void logBorrowActivity(ActivityServiceState &state,
                        const std::string &bookId,
                        const std::string &memberId,
                        const std::string &loanId,
                        const std::string &time,
                        const std::string &detail);
 
void logReturnActivity(ActivityServiceState &state,
                        const std::string &bookId,
                        const std::string &memberId,
                        const std::string &loanId,
                        const std::string &time,
                        const std::string &detail);
 
void logReserveActivity(ActivityServiceState &state,
                         const std::string &bookId,
                         const std::string &memberId,
                         const std::string &reservationId,
                         const std::string &time,
                         const std::string &detail);
 
std::vector<Activity> getRecentActivityLog(const ActivityServiceState &state,
                                            int count = DEFAULT_RECENT_COUNT);
 
std::vector<Activity> getAllActivityLog(const ActivityServiceState &state);
 
std::vector<Activity> findActivitiesByMember(const ActivityServiceState &state,
                                              const std::string &memberId);
 
std::vector<Activity> findActivitiesByBook(const ActivityServiceState &state,
                                            const std::string &bookId);
 
