#include <string>
#include <map>
#include <vector>
#include <fstream>

namespace json {

    typedef std::map<std::string, std::string> JSONKeyValuePair;
    typedef std::vector<std::string> JSONArray;

    struct JSONObject {
        JSONKeyValuePair pairs;
    };

    JSONObject createJSONObject() {
        JSONObject object;
        return object;
    }

    void addKeyValuePair(JSONObject& object, const std::string& key, const std::string& value) {
        object.pairs[key] = value;
    }

    std::string toJSONString(const JSONObject& object) {
        std::string result = "{\n";
        for (const auto& pair : object.pairs) {
            result += "  \"" + pair.first + "\": \"" + pair.second + "\",\n";
        }
        if (!object.pairs.empty()) {
            result.pop_back();
            result.pop_back();
        }
        result += "\n}";
        return result;
    }

    void printJSONObject(const JSONObject& object) {
        std::cout << toJSONString(object) << std::endl;
    }

    void saveJSONObjectToFile(const JSONObject& object, const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << toJSONString(object);
            file.close();
            std::cout << "JSON object saved to file: " << filename << std::endl;
        }
        else {
            std::cout << "Failed to open file for saving." << std::endl;
        }
    }

    JSONArray createJSONArray() {
        JSONArray array;
        return array;
    }

    void addElement(JSONArray& array, const std::string& value) {
        array.push_back(value);
    }

    std::string toJSONString(const JSONArray& array) {
        std::string result = "[\n";
        for (const auto& element : array) {
            result += "  \"" + element + "\",\n";
        }
        if (!array.empty()) {
            result.pop_back();
            result.pop_back();
        }
        result += "\n]";
        return result;
    }

    void printJSONArray(const JSONArray& array) {
        std::cout << toJSONString(array) << std::endl;
    }

    void saveJSONArrayToFile(const JSONArray& array, const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << toJSONString(array);
            file.close();
            std::cout << "JSON array saved to file: " << filename << std::endl;
        }
        else {
            std::cout << "Failed to open file for saving." << std::endl;
        }
    }

}
