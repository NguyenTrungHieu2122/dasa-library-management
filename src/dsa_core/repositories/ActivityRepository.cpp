#include "ActivityRepository.h"

#include "../../persistence/JsonValue.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace {
std::string readFile(const std::string& filepath) {
    std::ifstream input(filepath, std::ios::binary);
    if (!input) return "[]";
    std::ostringstream contents;
    contents << input.rdbuf();
    return contents.str();
}

std::string getString(const JsonValue& item, const std::string& key) {
    return item[key].stringOr();
}
}

std::vector<Activity> loadAllActivities(const std::string& filepath) {
    const JsonValue data = JsonParser(readFile(filepath)).parse();
    std::vector<Activity> activities;
    if (!data.isArray()) return activities;

    for (size_t i = 0; i < data.size(); ++i) {
        const JsonValue& item = data[i];
        Activity activity;
        activity.activityId = getString(item, "activityId");
        activity.type = getString(item, "type");
        activity.bookId = getString(item, "bookId");
        activity.memberId = getString(item, "memberId");
        activity.loanId = getString(item, "loanId");
        activity.reservationId = getString(item, "reservationId");
        activity.time = getString(item, "time");
        activity.detail = getString(item, "detail");
        activities.push_back(activity);
    }
    return activities;
}

bool saveAllActivities(const std::vector<Activity>& activities, const std::string& filepath) {
    const std::filesystem::path path(filepath);
    if (!path.parent_path().empty()) {
        std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream output(filepath, std::ios::binary | std::ios::trunc);
    if (!output) return false;

    output << "[\n";
    for (size_t i = 0; i < activities.size(); ++i) {
        const Activity& activity = activities[i];
        output << "  {\"activityId\":" << jsonEscape(activity.activityId)
               << ",\"type\":" << jsonEscape(activity.type)
               << ",\"bookId\":" << jsonEscape(activity.bookId)
               << ",\"memberId\":" << jsonEscape(activity.memberId)
               << ",\"loanId\":" << jsonEscape(activity.loanId)
               << ",\"reservationId\":" << jsonEscape(activity.reservationId)
               << ",\"time\":" << jsonEscape(activity.time)
               << ",\"detail\":" << jsonEscape(activity.detail) << "}"
               << (i + 1 == activities.size() ? "\n" : ",\n");
    }
    output << "]\n";
    return output.good();
}
