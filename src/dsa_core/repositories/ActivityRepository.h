#pragma once
 
#include <string>
#include <vector>
#include "Activity.h" // duong dan thuc te: "../models/Activity.h"
 
std::vector<Activity> loadAllActivities(const std::string& filepath = "data/activities.json");
bool saveAllActivities(const std::vector<Activity>& activities, const std::string& filepath = "data/activities.json"); 
