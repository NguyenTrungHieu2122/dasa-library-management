#include "ActivityRepository.h"
#include "../../persistence/JsonDatabase.h" 
#include <nlohmann/json.hpp> // Đảm bảo đã thêm thư viện này để không bị lỗi compile

using json = nlohmann::json;
 
std::vector<Activity> loadAllActivities(const std::string& filepath) {
    json data = readJsonFile(filepath);
    std::vector<Activity> activities;
 
    // Sử dụng vòng lặp for-each hiện đại của C++ để duyệt qua mảng JSON
    for (const auto& item : data) {
        Activity a;
        // Lưu ý: Nếu các ID này trong struct Activity là kiểu INT, hãy đổi "" thành 0
        a.activityId    = item.value("activityId", "");
        a.type          = item.value("type", "");
        a.bookId        = item.value("bookId", "");
        a.memberId      = item.value("memberId", "");
        a.loanId        = item.value("loanId", "");        
        a.reservationId = item.value("reservationId", "");  
        a.time          = item.value("time", "");
        a.detail        = item.value("detail", "");
 
        activities.push_back(a);
    }
 
    return activities;
}
 
bool saveAllActivities(const std::vector<Activity>& activities, const std::string& filepath) {
    json data = json::array();
 
    // Sử dụng vòng lặp for-each để duyệt qua danh sách các Activity
    for (const auto& a : activities) {
        json item;
        item["activityId"] = a.activityId;
        item["type"]       = a.type;
        item["bookId"]     = a.bookId;
        item["memberId"]   = a.memberId;
 
        // Sử dụng .empty() thay cho != "" để tối ưu hiệu năng C++
        if (!a.loanId.empty()) {
            item["loanId"] = a.loanId;
        }
        if (!a.reservationId.empty()) {
            item["reservationId"] = a.reservationId;
        }
 
        item["time"]   = a.time;
        item["detail"] = a.detail;
 
        data.push_back(item);
    }
 
    return writeJsonFile(filepath, data);
}
