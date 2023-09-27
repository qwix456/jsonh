#ifndef JSONH_HPP
#define JSONH_HPP

#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <variant>
#include <fstream>

namespace jsonh {

	// Forward declaration
	class Value;

	using JSON = Value;

	class Value {
	public:
		virtual ~Value() {}
		virtual std::string indent(int indentLevel = 0) const = 0;
	};

	class Null : public Value {
	public:
		std::string indent(int indentLevel = 0) const override {
			return "null";
		}
	};

	class Boolean : public Value {
	public:
		Boolean(bool value) : value(value) {}

		std::string indent(int indentLevel = 0) const override {
			return value ? "true" : "false";
		}

		bool get() const {
			return value;
		}

	private:
		bool value;
	};

	class Number : public Value {
	public:
		Number(int value) : value(value), isDouble(false) {}
		Number(double value) : value(value), isDouble(true) {}

		std::string indent(int indentLevel = 0) const override {
			if (isDouble) {
				return std::to_string(value);
			}
			else {
				return std::to_string(static_cast<int>(value));
			}
		}

		double get() const {
			return value;
		}

		static bool isInteger(const std::string& num) {
			if (num.empty()) {
				return false;
			}

			size_t start = 0;
			if (num[0] == '-' || num[0] == '+') {
				start = 1;
			}

			for (size_t i = start; i < num.size(); ++i) {
				if (!isdigit(num[i])) {
					return false;
				}
			}

			return true;
		}

	private:
		double value;
		bool isDouble;
	};

	class String : public Value {
	public:
		String(const std::string& value) : value(value) {}

		std::string indent(int indentLevel = 0) const override {
			return value;
		}

		std::string get() const {
			return value;
		}

	private:
		std::string value;
	};

	class Object : public Value {
	public:
		void set(const std::string& key, Value* value) {
			data[key] = value;
		}

		Value* get(const std::string& key) const {
			auto it = data.find(key);
			if (it != data.end()) {
				return it->second;
			}
			return nullptr;
		}

		std::string getString(const std::string& key) const {
			Value* value = get(key);
			if (value && dynamic_cast<String*>(value)) {
				return dynamic_cast<String*>(value)->get();
			}
			return "";
		}

		int getInt(const std::string& key) const {
			Value* value = get(key);
			if (value && dynamic_cast<Number*>(value)) {
				return static_cast<int>(dynamic_cast<Number*>(value)->get());
			}
			return 0;
		}

		double getDouble(const std::string& key) const {
			Value* value = get(key);
			if (value && dynamic_cast<Number*>(value)) {
				return dynamic_cast<Number*>(value)->get();
			}
			return 0.0;
		}

		Object* getObject(const std::string& key) const {
			Value* value = get(key);
			return dynamic_cast<Object*>(value);
		}

		bool hasKey(const std::string& key) const {
			return data.find(key) != data.end();
		}

		std::string indent(int indentLevel = 0) const override {
			std::string result = "{\n";
			std::string indent(indentLevel + 1, ' ');

			bool first = true;
			for (const auto& entry : data) {
				if (!first) {
					result += ",\n";
				}
				result += indent + "\"" + entry.first + "\": ";

				// Check if value is a string
				if (dynamic_cast<String*>(entry.second)) {
					result += "\"" + entry.second->indent(indentLevel) + "\"";
				}
				else {
					result += entry.second->indent(indentLevel);
				}

				first = false;
			}

			result += "\n" + std::string(indentLevel > 0 ? indentLevel - indentLevel : 0, ' ') + "}";
			return result;
		}

	private:
		std::unordered_map<std::string, Value*> data;
	};

	class Array : public Value {
	public:
		void add(Value* value) {
			values.push_back(value);
		}

		Value* get(size_t index) const {
			if (index < values.size()) {
				return values[index];
			}
			return nullptr;
		}

		std::string getString(size_t index) const {
			Value* value = get(index);
			if (value && dynamic_cast<String*>(value)) {
				return dynamic_cast<String*>(value)->get();
			}
			return "";
		}

		int getInt(size_t index) const {
			Value* value = get(index);
			if (value && dynamic_cast<Number*>(value)) {
				return static_cast<int>(dynamic_cast<Number*>(value)->get());
			}
			return 0;
		}

		double getDouble(size_t index) const {
			Value* value = get(index);
			if (value && dynamic_cast<Number*>(value)) {
				return dynamic_cast<Number*>(value)->get();
			}
			return 0.0;
		}

		std::string indent(int indentLevel = 0) const override {
			std::string result = "[\n";
			std::string indent(indentLevel + 1, ' ');

			bool first = true;
			for (const auto& value : values) {
				if (!first) {
					result += ",\n";
				}

				// Check if value is a string
				if (dynamic_cast<String*>(value)) {
					result += indent + "\"" + value->indent(indentLevel) + "\"";
				}
				else {
					result += indent + value->indent(indentLevel);
				}

				first = false;
			}

			result += "\n" + std::string(indentLevel > 0 ? indentLevel - indentLevel : 0, ' ') + "]";
			return result;
		}

	private:
		std::vector<Value*> values;
	};

	void jsonToFile(const JSON& json, const std::string& filename) {
		std::ofstream file(filename);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file for writing: " + filename);
		}
		file << json.indent();
		file.close();
	}

	class Parser {
	public:
		static Value* parseValue(const std::string& content, size_t& index);

		static Object* parseObject(const std::string& content, size_t& index);

		static Array* parseArray(const std::string& content, size_t& index);

		static String* parseString(const std::string& content, size_t& index);

		static Number* parseNumber(const std::string& content, size_t& index);

		static Value* parseBooleanOrNull(const std::string& content, size_t& index);

		static Value* parseIndex(const std::string& content);

		static Value* parse(const std::string& filename);

		static void skipWhitespace(const std::string& content, size_t& index);

		static void expectCharacter(char expected, char actual);
	};

	Value* Parser::parseIndex(const std::string& content) {
		size_t index = 0;
		return parseValue(content, index);
	}

	Value* Parser::parseValue(const std::string& content, size_t& index) {
		char c = content[index];
		if (c == '{') {
			return parseObject(content, index);
		}
		else if (c == '[') {
			return parseArray(content, index);
		}
		else if (c == '\"') {
			return parseString(content, index);
		}
		else if (isdigit(c) || c == '-') {
			return parseNumber(content, index);
		}
		else if (c == 't' || c == 'f' || c == 'n') {
			return parseBooleanOrNull(content, index);
		}
		else {
			throw std::runtime_error("Invalid JSON content");
		}
	}

	Object* Parser::parseObject(const std::string& content, size_t& index) {
		Object* obj = new Object();
		index++; // skip opening brace

		while (true) {
			skipWhitespace(content, index);
			char c = content[index];
			if (c == '}') {
				index++; // skip closing brace
				break;
			}
			else if (c == ',') {
				index++; // skip comma
				continue;
			}

			std::string key = parseString(content, index)->indent();
			skipWhitespace(content, index);
			expectCharacter(':', content[index++]); // shit code
			skipWhitespace(content, index);
			Value* value = parseValue(content, index);
			obj->set(key, value);
		}

		return obj;
	}

	Array* Parser::parseArray(const std::string& content, size_t& index) {
		Array* arr = new Array();
		index++; // skip opening bracket

		while (true) {
			skipWhitespace(content, index);
			char c = content[index];
			if (c == ']') {
				index++; // skip closing bracket
				break;
			}
			else if (c == ',') {
				index++; // skip comma
				continue;
			}

			Value* value = parseValue(content, index);
			arr->add(value);
		}

		return arr;
	}

	String* Parser::parseString(const std::string& content, size_t& index) {
		std::string str = "";
		expectCharacter('\"', content[index++]);

		while (true) {
			char c = content[index++];
			if (c == '\"') {
				break;
			}
			else if (c == '\\') {
				char next = content[index++];
				switch (next) {
				case '\"':
					str += '\"';
					break;
				case '\\':
					str += '\\';
					break;
				case '/':
					str += '/';
					break;
				case 'b':
					str += '\b';
					break;
				case 'f':
					str += '\f';
					break;
				case 'n':
					str += '\n';
					break;
				case 'r':
					str += '\r';
					break;
				case 't':
					str += '\t';
					break;
				case 'u': {
					std::string hex = content.substr(index, 4);
					int codepoint = std::stoi(hex, 0, 16);
					str += static_cast<char>(codepoint);
					break;
				}
				default:
					throw std::runtime_error("Invalid escape sequence in string");
				}
			}
			else {
				str += c;
			}
		}

		return new String(str);
	}

	Number* Parser::parseNumber(const std::string& content, size_t& index) {
		std::string num;
		bool isDouble = false;

		if (content[index] == '-' || content[index] == '+') {
			num += content[index];
			++index;
		}

		while (isdigit(content[index])) {
			num += content[index];
			++index;
		}

		if (content[index] == '.') {
			isDouble = true;
			num += '.';
			++index;

			while (isdigit(content[index])) {
				num += content[index];
				++index;
			}
		}

		if (tolower(content[index]) == 'e') {
			num += 'e';
			++index;

			if (content[index] == '-' || content[index] == '+') {
				num += content[index];
				++index;
			}

			while (isdigit(content[index])) {
				num += content[index];
				++index;
			}
		}

		if (isDouble) {
			return new Number(std::stod(num));
		}
		else {
			return new Number(std::stoi(num));
		}
	}

	Value* Parser::parseBooleanOrNull(const std::string& content, size_t& index) {
		const std::string& subContent = content.substr(index);

		if (subContent.compare(0, 4, "true") == 0) {
			index += 4;
			return new Boolean(true);
		}
		else if (subContent.compare(0, 5, "false") == 0) {
			index += 5;
			return new Boolean(false);
		}
		else if (subContent.compare(0, 4, "null") == 0) {
			index += 4;
			return new Null();
		}
		else {
			throw std::runtime_error("Invalid boolean or null value");
		}
	}

	Value* Parser::parse(const std::string& filename) {
		std::ifstream file(filename);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file");
		}

		std::string content((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());

		Parser parser;
		return parser.parseIndex(content);
	}

	void Parser::skipWhitespace(const std::string& content, size_t& index) {
		while (index < content.size() && isspace(content[index])) {
			index++;
		}
	}

	void Parser::expectCharacter(char expected, char actual) {
		if (expected != actual) {
			throw std::runtime_error("Expected character '" + std::string(1, expected) + "' but found '" + std::string(1, actual) + "'");
		}
	}

	Value* jsonFromFile(const std::string& filename) {
		Parser parser;
		return parser.parse(filename);
	}
} // namespace jsonh

#endif // JSONH_HPP
