#pragma once

#include <cctype>
#include <cstdlib>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

class JsonValue {
public:
    enum class Type { Null, Boolean, Number, String, Array, Object };
    Type type = Type::Null;
    bool booleanValue = false;
    double numberValue = 0;
    std::string stringValue;
    std::vector<JsonValue> arrayValue;
    std::map<std::string, JsonValue> objectValue;

    const JsonValue& operator[](const std::string& key) const {
        static const JsonValue nullValue;
        auto it = objectValue.find(key);
        return it == objectValue.end() ? nullValue : it->second;
    }
    const JsonValue& operator[](size_t index) const {
        static const JsonValue nullValue;
        return index < arrayValue.size() ? arrayValue[index] : nullValue;
    }
    std::string stringOr(const std::string& fallback = "") const {
        if (type == Type::String) return stringValue;
        if (type == Type::Number) return std::to_string(static_cast<long long>(numberValue));
        return fallback;
    }
    int intOr(int fallback = 0) const {
        return type == Type::Number ? static_cast<int>(numberValue) : fallback;
    }
    bool isArray() const { return type == Type::Array; }
    size_t size() const { return arrayValue.size(); }
};

class JsonParser {
    const std::string& input;
    size_t pos = 0;
    void whitespace() { while (pos < input.size() && std::isspace(static_cast<unsigned char>(input[pos]))) ++pos; }
    char take() { if (pos >= input.size()) throw std::runtime_error("JSON khong hop le: het du lieu"); return input[pos++]; }
    std::string parseString() {
        if (take() != '"') throw std::runtime_error("JSON khong hop le: thieu dau nhay");
        std::string out;
        while (pos < input.size()) {
            char c = take();
            if (c == '"') return out;
            if (c != '\\') { out += c; continue; }
            char escaped = take();
            switch (escaped) {
                case '"': out += '"'; break; case '\\': out += '\\'; break;
                case '/': out += '/'; break; case 'b': out += '\b'; break;
                case 'f': out += '\f'; break; case 'n': out += '\n'; break;
                case 'r': out += '\r'; break; case 't': out += '\t'; break;
                case 'u': {
                    if (pos + 4 > input.size()) throw std::runtime_error("JSON unicode khong hop le");
                    unsigned code = 0;
                    for (int i = 0; i < 4; ++i) {
                        char h = take(); code <<= 4;
                        if (h >= '0' && h <= '9') code += h - '0';
                        else if (h >= 'a' && h <= 'f') code += h - 'a' + 10;
                        else if (h >= 'A' && h <= 'F') code += h - 'A' + 10;
                        else throw std::runtime_error("JSON unicode khong hop le");
                    }
                    if (code <= 0x7f) out += static_cast<char>(code);
                    else if (code <= 0x7ff) { out += static_cast<char>(0xc0 | (code >> 6)); out += static_cast<char>(0x80 | (code & 0x3f)); }
                    else { out += static_cast<char>(0xe0 | (code >> 12)); out += static_cast<char>(0x80 | ((code >> 6) & 0x3f)); out += static_cast<char>(0x80 | (code & 0x3f)); }
                    break;
                }
                default: throw std::runtime_error("JSON escape khong hop le");
            }
        }
        throw std::runtime_error("JSON khong hop le: chuoi chua dong");
    }
    JsonValue value() {
        whitespace();
        if (pos >= input.size()) throw std::runtime_error("JSON khong hop le: thieu gia tri");
        JsonValue result;
        char c = input[pos];
        if (c == '"') { result.type = JsonValue::Type::String; result.stringValue = parseString(); return result; }
        if (c == '{') {
            ++pos; result.type = JsonValue::Type::Object; whitespace();
            if (pos < input.size() && input[pos] == '}') { ++pos; return result; }
            while (true) {
                whitespace(); std::string key = parseString(); whitespace();
                if (take() != ':') throw std::runtime_error("JSON object thieu ':'");
                result.objectValue[key] = value(); whitespace(); char sep = take();
                if (sep == '}') return result;
                if (sep != ',') throw std::runtime_error("JSON object thieu ','");
            }
        }
        if (c == '[') {
            ++pos; result.type = JsonValue::Type::Array; whitespace();
            if (pos < input.size() && input[pos] == ']') { ++pos; return result; }
            while (true) {
                result.arrayValue.push_back(value()); whitespace(); char sep = take();
                if (sep == ']') return result;
                if (sep != ',') throw std::runtime_error("JSON array thieu ','");
            }
        }
        if (input.compare(pos, 4, "null") == 0) { pos += 4; return result; }
        if (input.compare(pos, 4, "true") == 0) { pos += 4; result.type = JsonValue::Type::Boolean; result.booleanValue = true; return result; }
        if (input.compare(pos, 5, "false") == 0) { pos += 5; result.type = JsonValue::Type::Boolean; return result; }
        char* end = nullptr; result.numberValue = std::strtod(input.c_str() + pos, &end);
        if (end == input.c_str() + pos) throw std::runtime_error("JSON gia tri khong hop le");
        pos = static_cast<size_t>(end - input.c_str()); result.type = JsonValue::Type::Number; return result;
    }
public:
    explicit JsonParser(const std::string& source) : input(source) {}
    JsonValue parse() { JsonValue result = value(); whitespace(); if (pos != input.size()) throw std::runtime_error("JSON con du lieu thua"); return result; }
};

inline std::string jsonEscape(const std::string& value) {
    std::string out = "\"";
    for (unsigned char c : value) {
        switch (c) { case '"': out += "\\\""; break; case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break; case '\r': out += "\\r"; break; case '\t': out += "\\t"; break;
            default: if (c < 0x20) out += ' '; else out += static_cast<char>(c); }
    }
    return out + '"';
}
