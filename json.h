#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <fstream>

namespace skyue {

    namespace jsonh {

        // Define some aliases for convenience
        typedef std::map<std::string, std::string> JSONKeyValuePair;
        typedef std::vector<std::string> JSONArray;

        // Define a struct for a JSON object
        struct JSONObject {
            JSONKeyValuePair pairs;
            std::map<std::string, JSONArray> objects;
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
                std::string result;
								result.reserve(2 + object.pairs.size() * (4 + indent_spaces) + object.pairs.size() * 5);
								result.push_back('{');
								result.push_back('\n');
								std::string indent(indent_spaces, ' ');
								for (const auto& pair : object.pairs) {
									result += indent + "\"" + pair.first + "\": \"" + pair.second + "\",\n";
								}
								if (!object.pairs.empty()) {
									result.pop_back();
									result.pop_back();
								}
								result.push_back('\n');
								result.push_back('}');
								return result;
            }

            // Save the JSON object to a file
            void json_to_file(const std::string& filename) const {
                std::ofstream file(filename);

                if (file.is_open()) {
                    std::string indent(indent_spaces, ' ');
                    file << "{\n";
                    bool first_pair = true;

                    for (const auto& pair : object.pairs) {
                        if (!first_pair) {
                            file << ",\n";
                        }
                        else {
                            first_pair = false;
                        }
                        file << indent << "\"" << pair.first << "\": \"" << pair.second << "\"";
                    }

                    file << "\n}";
                    file.close();
                }
            }

            // Set the number of spaces for indentation
            void indent(int spaces) {
                indent_spaces = spaces;
            }

            /*
            The skip_whitespace function skips all whitespace characters in the json string,
            starting from position pos. This function uses a while loop to check each character,
            starting from the current position, and increases the position until the
            current character is a whitespace. After the function is completed, pos points to
            the first non-whitespace character in the json string or to the end of the string
            if all characters were whitespace.
            */
            void skip_whitespace(const std::string& json, size_t& pos) {
                while (pos < json.size() && std::isspace(json[pos])) {
                    pos++;
                }
            }
            std::string parse_string(const std::string& json, size_t& pos) {
                std::string result;

								if (json[pos] != '"') {
									throw std::runtime_error("A string in JSON is expected.");
								}

								pos++;

								while (pos < json.size() && json[pos] != '"') {
									if (json[pos] == '\\') {
										pos++;
									}

									result.push_back(json[pos]);
									pos++;
								}

								if (pos >= json.size() || json[pos] != '"') {
									throw std::runtime_error("Invalid string in JSON.");
								}

								pos++;

								return result;
						}

            std::map<std::string, std::string> parse_object(const std::string& json, size_t& pos) {
                std::map<std::string, std::string> result;
                if (json[pos] == '{') {
                    pos++;
                    skip_whitespace(json, pos);

                    if (json[pos] == '}') {
                        pos++;
                        return result;
                    }

                    while (pos < json.size()) {
                        std::string key = parse_string(json, pos);
                        skip_whitespace(json, pos);

												if (json[pos] != ':') {
													throw std::runtime_error("A colon is expected in a JSON object.");
												}
												pos++;
												skip_whitespace(json, pos);

												std::string value = parse_string(json, pos);
												result[key] = value;
												skip_whitespace(json, pos);

												if (json[pos] == ',') {
													pos++;
												}
												else if (json[pos] == '}') {
													pos++;
													break;
												}
												else {
													throw std::runtime_error("Incorrect object format in JSON.");
												}
										}
								} else {
									throw std::runtime_error("Expected object in JSON.");
								}
								return result;
            }

            std::vector<std::string> parse_array(const std::string& json, size_t& pos) {
              std::vector<std::string> result;
              if (json[pos] != '[') {
								throw std::runtime_error("Array in JSON expected.");
							}
							pos++;
							skip_whitespace(json, pos);
							if (json[pos] == ']') {
								pos++;
								return result;
							}
							while (pos < json.size()) {
								std::string value = parse_string(json, pos);
								result.push_back(value);
								skip_whitespace(json, pos);
								if (json[pos] == ',') {
									pos++;
								}
								else if (json[pos] == ']') {
									pos++;
									break;
								}
								else {
									throw std::runtime_error("Incorrect array format in JSON.");
								}
							}
							return result;
            }

            void json_parse(const std::string& filename) {
                std::ifstream file(filename);
                if (!file.is_open()) {
                    throw std::runtime_error("Failed to open file.");
                }

                std::stringstream buffer;
                buffer << file.rdbuf();
                std::string contents = buffer.str();

                JSON json;

                try {
                    size_t pos = 0;
                    skip_whitespace(contents, pos);
                    if (pos < contents.size()) {
                        if (contents[pos] == '{') {
                            json.object.pairs = parse_object(contents, pos);
                        }
                        else if (contents[pos] == '[') {
                            json.object.objects["array"] = parse_array(contents, pos);
                        }
                        else {
                            throw std::runtime_error("Incorrect JSON file format.");
                        }
                        skip_whitespace(contents, pos);
                        if (pos < contents.size()) {
                            throw std::runtime_error("Incorrect JSON file format.");
                        }
                    }
                    else {
                        throw std::runtime_error("Empty JSON file.");
                    }
                }
                catch (const std::exception& e) {
                    throw std::runtime_error("Failed to parse JSON file. Error: " + std::string(e.what()));
                }
            }

            // Create a JSON array
            JSONArray create_json_array() {
                return JSONArray();
            }
        };
    } // namespace jsonh
} // namespace skyue
