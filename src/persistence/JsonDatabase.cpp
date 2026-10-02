#include "JsonDatabase.h"
 
#include <fstream>
#include <filesystem>
#include <iostream>
 
namespace fs = std::filesystem;
 
bool jsonFileExists(const std::string& filepath) {
    return fs::exists(filepath);
}
 
nlohmann::json readJsonFile(const std::string& filepath, const nlohmann::json& defaultValue) {
    // Buoc 1: kiem tra file co ton tai khong
    if (!jsonFileExists(filepath)) {
        std::cout << "[Thong bao] File " << filepath
                  << " chua ton tai, dung du lieu mac dinh (rong).\n";
        return defaultValue;
    }
 
    // Buoc 2: mo file de doc
    std::ifstream inFile(filepath);
    if (!inFile.is_open()) {
        std::cout << "[Loi] Khong mo duoc file de doc: " << filepath << "\n";
        return defaultValue;
    }
 
    // Buoc 3: doc noi dung JSON tu file vao bien data
    nlohmann::json data;
    try {
        inFile >> data;
    } catch (...) {
        // try/catch o day chi de "bat" loi khi file JSON bi sai dinh dang
        // (VD: thieu dau ngoac, thieu dau phay,...), tranh lam crash chuong trinh.
        std::cout << "[Loi] File JSON sai dinh dang: " << filepath << "\n";
        inFile.close();
        return defaultValue;
    }
 
    inFile.close();
    return data;
}
 
bool writeJsonFile(const std::string& filepath, const nlohmann::json& data, int indent) {
    // Buoc 1: tao thu muc cha neu chua ton tai (VD thu muc "data/")
    fs::path path(filepath);
    fs::path parentFolder = path.parent_path();
    if (!parentFolder.empty() && !fs::exists(parentFolder)) {
        fs::create_directories(parentFolder);
    }
 
    // Buoc 2: mo file de ghi (ghi de toan bo noi dung cu)
    std::ofstream outFile(filepath);
    if (!outFile.is_open()) {
        std::cout << "[Loi] Khong mo duoc file de ghi: " << filepath << "\n";
        return false;
    }
 
    // Buoc 3: ghi du lieu JSON xuong file
    if (indent >= 0) {
        outFile << data.dump(indent); // dump = chuyen json thanh chuoi text, co thut le
    } else {
        outFile << data.dump();
    }
 
    outFile.close();
    return true;
}
