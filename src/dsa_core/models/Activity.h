#pragma once
#include <string>
#include <cstddef>
#include <utility>
#include <vector>

inline constexpr std::size_t RECENT_ACTIVITY_CAPACITY = 1000;

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

inline void retainRecentActivity(std::vector<Activity>& activities, Activity activity)
{
    activities.push_back(std::move(activity));
    if (activities.size() > RECENT_ACTIVITY_CAPACITY)
        activities.erase(activities.begin(), activities.begin() +
                         (activities.size() - RECENT_ACTIVITY_CAPACITY));
}

inline void trimActivitiesToRecent(std::vector<Activity>& activities)
{
    if (activities.size() > RECENT_ACTIVITY_CAPACITY)
        activities.erase(activities.begin(), activities.end() - RECENT_ACTIVITY_CAPACITY);
}
