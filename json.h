#include <string>
#include <map>
#include <vector>
#include <fstream>

namespace skyue {

    namespace jsonh {

        // Define some aliases for convenience
        typedef std::map<std::string, std::string> JSONKeyValuePair;
        typedef std::vector<std::string> JSONArray;

        // Define a struct for a JSON object
        struct JSONObject {
            JSONKeyValuePair pairs;
        };

        // Define a class for a JSON object
        class JSON {
        private:
            JSONObject object;
            int indent_spaces = 0;

        public:
            // Constructor
            JSON() {}

            // Overload the [] operator to allow access to key-value pairs
            std::string& operator[](const std::string& key) {
                return object.pairs[key];
            }

            // Convert the JSON object to a string
            std::string json_to_string() const {
                std::string indent(indent_spaces, ' ');
                std::string result = "{\n";
                for (const auto& pair : object.pairs) {
                    result += indent + "\"" + pair.first + "\": \"" + pair.second + "\",\n";
                }
                if (!object.pairs.empty()) {
                    result.pop_back();
                    result.pop_back();
                }
                result += "\n}";
                return result;
            }

            // Save the JSON object to a file
            void json_to_file(const std::string& filename) const {
                std::ofstream file(filename);

                if (file.is_open()) {
                    std::string indent(indent_spaces, ' ');
                    file << "{\n";
                    for (const auto& pair : object.pairs) {
                        file << indent << "\"" << pair.first << "\": \"" << pair.second << "\",\n";
                    }
                    if (!object.pairs.empty()) {
                        file.seekp(-2, std::ios_base::end);
                        file << "\n";
                    }
                    file << "}";
                    file.close();
                }
            }

            // Set the number of spaces for indentation
            void indent(int spaces) {
                indent_spaces = spaces;
            }

            // Create a JSON array
            JSONArray create_json_array() {
                JSONArray array;
                return array;
            }
        };
    }
}
