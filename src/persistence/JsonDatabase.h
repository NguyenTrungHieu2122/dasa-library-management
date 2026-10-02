#pragma once

#include <string>
#include "nlohmann/json.hpp"


nlohmann::json readJsonFile(const std::string& filepath,
                             const nlohmann::json& defaultValue = nlohmann::json::array());

bool writeJsonFile(const std::string& filepath, const nlohmann::json& data, int indent = 4);

// Kiem tra file co ton tai tren dia hay khong.
bool jsonFileExists(const std::string& filepath);
