#include "ActivityService.h"
#include "../repositories/ActivityRepository.h" 
 
std::string generateNextActivityId(const ActivityServiceState &state) {
    int maxNum = 0;
 
    for (int i = 0; i < (int)state.allActivities.size(); i++) {
        std::string id = state.allActivities[i].activityId; // VD "A07"
        if (id.size() > 1) {
            std::string numberPart = id.substr(1); // bo chu "A" dau tien, con lai "07"
            int num = std::stoi(numberPart);
            if (num > maxNum) {
                maxNum = num;
            }
        }
    }
 
    int nextNum = maxNum + 1;
    std::string result = "A";
    if (nextNum < 10) {
        result += "0"; // dam bao luon co it nhat 2 chu so: A01, A02,... A10, A11,...
    }
    result += std::to_string(nextNum);
    return result;
}
 

void recordActivity(ActivityServiceState &state,
                     const std::string &type,
                     const std::string &bookId,
                     const std::string &memberId,
                     const std::string &loanId,
                     const std::string &reservationId,
                     const std::string &time,
                     const std::string &detail) {
    Activity a;
    a.activityId    = generateNextActivityId(state);
    a.type          = type;
    a.bookId        = bookId;
    a.memberId      = memberId;
    a.loanId        = loanId;
    a.reservationId = reservationId;
    a.time          = time;
    a.detail        = detail;
 
    // Them vao danh sach trong bo nho
    state.allActivities.push_back(a);
 
    // Ghi ngay xuong file, tranh mat du lieu neu chuong trinh bi tat dot ngot
    saveAllActivities(state.allActivities, state.filepath);
}
 
ActivityServiceState initActivityService(const std::string &filepath) {
    ActivityServiceState state;
    state.filepath = filepath;
    state.allActivities = loadAllActivities(filepath);
    return state;
}
 
void logBorrowActivity(ActivityServiceState &state,
                        const std::string &bookId,
                        const std::string &memberId,
                        const std::string &loanId,
                        const std::string &time,
                        const std::string &detail) {
    recordActivity(state, "borrow", bookId, memberId, loanId, "", time, detail);
}
 
void logReturnActivity(ActivityServiceState &state,
                        const std::string &bookId,
                        const std::string &memberId,
                        const std::string &loanId,
                        const std::string &time,
                        const std::string &detail) {
    recordActivity(state, "return", bookId, memberId, loanId, "", time, detail);
}
 
void logReserveActivity(ActivityServiceState &state,
                         const std::string &bookId,
                         const std::string &memberId,
                         const std::string &reservationId,
                         const std::string &time,
                         const std::string &detail) {
    // type = "reserve", loanId de rong vi khong lien quan
    recordActivity(state, "reserve", bookId, memberId, "", reservationId, time, detail);
}
 
std::vector<Activity> getRecentActivityLog(const ActivityServiceState &state, int count) {
    std::vector<Activity> result;
 
    int total = (int)state.allActivities.size();
    if (total == 0) {
        return result; // khong co hoat dong nao
    }
 
    // Tinh vi tri bat dau: lay toi da "count" phan tu CUOI mang
    // (vi hoat dong moi nhat luon duoc push_back vao cuoi)
    int howMany = count;
    if (howMany > total) {
        howMany = total; // neu danh sach it hon count, lay het
    }
 
    // Duyet TU CUOI VE DAU de ket qua tra ve co MOI NHAT O DAU TIEN
    for (int i = total - 1; i >= total - howMany; i--) {
        result.push_back(state.allActivities[i]);
    }
 
    return result;
}
 
std::vector<Activity> getAllActivityLog(const ActivityServiceState &state) {
    return state.allActivities;
}
 
std::vector<Activity> findActivitiesByMember(const ActivityServiceState &state,
                                              const std::string &memberId) {
    std::vector<Activity> result;
    for (int i = 0; i < (int)state.allActivities.size(); i++) {
        if (state.allActivities[i].memberId == memberId) {
            result.push_back(state.allActivities[i]);
        }
    }
    return result;
}
 
std::vector<Activity> findActivitiesByBook(const ActivityServiceState &state,
                                            const std::string &bookId) {
    std::vector<Activity> result;
    for (int i = 0; i < (int)state.allActivities.size(); i++) {
        if (state.allActivities[i].bookId == bookId) {
            result.push_back(state.allActivities[i]);
        }
    }
    return result;
}
