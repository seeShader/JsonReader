#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cctype>
#include <memory>
#include <exception>

//控制是否输出调试信息
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
#include <iostream>
#endif

//用于简化定义
#define __LJR_GET_VALUE_DEFINE(name) \
	inline std::shared_ptr<Object_##name> getValue_##name(const std::string& key) { \
		return JsonReader::castType<Object_##name>(getValue(key));};

#define __LJR_GET_VALUE_DEFINE_ARRAY(name) \
	inline std::shared_ptr<Object_##name> getValue_##name(size_t index) { \
		return JsonReader::castType<Object_##name>(getValue(index));};

namespace JsonReader {
	enum class DataType
	{
		DT_NULL,STRING,LONG,DOUBLE,BOOL,ARRARY,OBJECT
	};

	class JsonParser;
	
	class DataObject;
	class Object_NULL;
	class Object_String;
	class Object_Long;
	class Object_Double;
	class Object_Bool;
	class Object_Array;
	//对应于花括号
	class Object_Object;

	template<typename To, typename From>
	std::shared_ptr<To> castType(const std::shared_ptr<From>& ptr);

	class JsonFormatError : public std::exception {
	public:
		// 重写 what() 方法，返回异常描述信息
		const char* what() const noexcept override {
			return "json格式错误";
		}
	};
	
	class DataObject
	{
	public:
		virtual DataType getType() = 0;
		virtual void write(std::ofstream& file,int indentNum = 1) = 0;
		//virtual void setParent(DataObject* parent) { this->parent = parent; }
		//virtual DataObject* getParent() { return parent; }
	protected:
		//DataObject* parent = nullptr;
	};

	class Object_NULL : public DataObject {
	public:
		DataType getType() override { return DataType::DT_NULL; }
		void write(std::ofstream& file, int indentNum) override;
	};

	class Object_String : public DataObject {
	public:
		Object_String(const std::string& str) : data(str) {}
		DataType getType() override { return DataType::STRING; }
		void write(std::ofstream& file, int indentNum) override;
		std::string getValue() { return data; }
	private:
		std::string data{};
	};

	class Object_Long : public DataObject {
	public:
		Object_Long(std::int64_t data) : data(data) {}
		DataType getType() override { return DataType::LONG; }
		void write(std::ofstream& file, int indentNum)  override;
		std::int64_t getValue() { return data; }
	private:
		std::int64_t data{};
	};

	class Object_Double : public DataObject {
	public:
		Object_Double(double data) : data(data) {}
		DataType getType() override { return DataType::DOUBLE; }
		void write(std::ofstream& file, int indentNum)  override;
		double getValue() { return data; }
	private:
		double data{};
	};

	class Object_Bool : public DataObject {
	public:
		Object_Bool(bool b) :value(b) {}
		DataType getType() override { return DataType::BOOL; }
		void write(std::ofstream& file, int indentNum)  override;
		inline bool getValue() { return value; }
	private:
		bool value = false;
	};

	class Object_Array : public DataObject {
	public:
		void addData(std::shared_ptr<DataObject> value) { objects.push_back(value); }
		
		DataType getType() override { return DataType::ARRARY; }
		void write(std::ofstream& file, int indentNum)  override;

		inline size_t getSize() const { return objects.size(); }

		std::vector<std::shared_ptr<DataObject>>* getValue() { return &objects; }
		inline std::shared_ptr<DataObject> getValue(size_t index) { return objects[index]; }

		//获取属性时同时转换为对应类型
		__LJR_GET_VALUE_DEFINE_ARRAY(NULL)
		__LJR_GET_VALUE_DEFINE_ARRAY(Bool)
		__LJR_GET_VALUE_DEFINE_ARRAY(Long)
		__LJR_GET_VALUE_DEFINE_ARRAY(Double)
		__LJR_GET_VALUE_DEFINE_ARRAY(String)
		__LJR_GET_VALUE_DEFINE_ARRAY(Array)
		__LJR_GET_VALUE_DEFINE_ARRAY(Object)

		std::shared_ptr<DataObject>& operator[](size_t index) {
			return objects[index];
		}

		const std::shared_ptr<DataObject>& operator[](size_t index) const {
			return objects[index];
		}

		std::shared_ptr<DataObject>& at(size_t index) { return objects.at(index); }

	private:
		std::vector <std::shared_ptr<DataObject>> objects{};
	};

	class Object_Object : public DataObject
	{
	public:
		DataType getType() override { return DataType::OBJECT; }
		void write(std::ofstream& file, int indentNum)  override;

		bool addData(const std::string& name, std::shared_ptr<DataObject> data);
		
		std::shared_ptr<DataObject> getValue(const std::string& name);

		//获取属性时同时转换为对应类型
		__LJR_GET_VALUE_DEFINE(NULL)
		__LJR_GET_VALUE_DEFINE(Bool)
		__LJR_GET_VALUE_DEFINE(Long)
		__LJR_GET_VALUE_DEFINE(Double)
		__LJR_GET_VALUE_DEFINE(String)
		__LJR_GET_VALUE_DEFINE(Array)
		__LJR_GET_VALUE_DEFINE(Object)

		std::shared_ptr<DataObject> deleteValue(const std::string& name);
		std::unordered_map<std::string, std::shared_ptr<DataObject>>* getValue() { return &objects; }
	private:
		std::unordered_map<std::string,std::shared_ptr<DataObject>> objects{};
	};

	class JsonParser
	{
	public:
		//JsonParser(const std::string& filePath);
		std::shared_ptr<DataObject> parseFile(const std::string& filePath);
		std::shared_ptr<DataObject> parseText(const std::string& content);
	private:
		JsonParser() = delete;

		std::string readFromFile(const std::string& filePath);
		std::shared_ptr<DataObject> parse(const std::string& content);
		std::shared_ptr<Object_Object> parseObject(const std::string& content, size_t& pos);
		std::shared_ptr<Object_Array> parseArray(const std::string& content, size_t& pos);
		//将给定的string中的第一个以""包裹的内容返回，并将pos移动到末尾
		std::string readString(const std::string& content, size_t& pos);
		//将给定的string中阅读一个数字并返回，并将pos移动到末尾
		std::shared_ptr<DataObject> readDigit(const std::string& content, size_t& pos);
		//从给定的string中的pos中匹配给定的字符串target
		bool match(const std::string& content, size_t pos,const std::string& target);
		//判断给定位置是无意义字符,如逗号，空格等
		bool isNonSense(const std::string& content, size_t pos);
	};

	//Object_Object
	bool JsonReader::Object_Object::addData(const std::string& name, std::shared_ptr<DataObject> data)
	{
		objects[name] = data;
		return true;
	}

	inline std::shared_ptr<DataObject> Object_Object::getValue(const std::string& name)
	{
		auto it = objects.find(name);
		if (it != objects.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	inline std::shared_ptr<DataObject> Object_Object::deleteValue(const std::string& name)
	{
		auto it = objects.find(name);
		if (it != objects.end()) {
			auto value = it->second;
			objects.erase(it);
			return value;
		}
		else {
			return nullptr;
		}
	}

	/*JsonReader::JsonParser::JsonParser(const std::string& filePath)
	{
		std::ifstream file(filePath);
		if (!file) {
			std::cerr << "无法打开文件: " << filePath << std::endl;
			return;
		}

		std::stringstream buffer;
		buffer << file.rdbuf();
		parse(buffer.str());
	}*/

	inline std::shared_ptr<DataObject> JsonParser::parseFile(const std::string& filePath)
	{
		try
		{
			return parse(readFromFile(filePath));
		}
		catch (const std::exception& e)
		{
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
			std::cout << e.what() << '\n';
#endif
			return nullptr;
		}
	}

	inline std::shared_ptr<DataObject> JsonParser::parseText(const std::string& content)
	{
		try
		{
			return parse(content);
		}
		catch (const std::exception& e)
		{
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
			std::cout << e.what() << '\n';
#endif
			return nullptr;
		}
	}

	inline std::string JsonParser::readFromFile(const std::string& filePath)
	{
		std::ifstream file(filePath);
		if (!file) {
			throw std::runtime_error("无法打开文件: " + filePath);
		}

		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}

	inline std::shared_ptr<DataObject> JsonReader::JsonParser::parse(const std::string& content)
	{
		size_t pos = 0;
		std::shared_ptr<DataObject> obj{};

		for (; pos < content.size();pos++) {
			switch (content[pos])
			{
			case '{': {
				obj = parseObject(content, pos);
				break;
			}
			case '[':{
				obj = parseArray(content, pos);
				break;
			}
			default:
				break;
			}
		}
		return obj;
	}
	inline std::shared_ptr<Object_Object> JsonParser::parseObject(const std::string& content, size_t& pos)
	{
		if (content[pos] != '{') return nullptr;
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
		std::cout << "开始解析新Object" << '\n';
#endif
		pos++;
		
		auto obj = std::make_shared<Object_Object>();
		//用于判断现在是在解析Key还是Value
		bool isNowReadKey = true;
		std::string key{};

		for (; pos < content.size(); pos++) {
			switch (content[pos])
			{
			case '{': {
				if (isNowReadKey == true) {
					throw JsonFormatError();
				}
				auto son = parseObject(content, pos);
				obj->addData(key, son);
				isNowReadKey = true;
				break;
			}
			case '[': {
				if (isNowReadKey == true) {
					throw JsonFormatError();
				}
				auto son = parseArray(content, pos);
				obj->addData(key,son);
				isNowReadKey = true;
				break;
			}
			case '}': {
				pos++;
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
				std::cout << "解析Object完成" << '\n';
#endif
				return obj;
			}
			case ':': {
				isNowReadKey = false;
				break;
			}
			case '"': {
				if (isNowReadKey) {
					key = readString(content, pos);
					if (pos >= content.size()) {
						throw JsonFormatError();
					}
					isNowReadKey = false;
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
					std::cout << "读取到key:" << key << '\n';
#endif
				}
				else {
					auto value = readString(content, pos);
					obj->addData(key,std::make_shared<Object_String>(value));
					isNowReadKey = true;
					if (pos >= content.size()) {
						throw JsonFormatError();
					}
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
					std::cout << "读取到value:" << value << '\n';
#endif
				}
				break;
			}
			case 't': {
				if (isNowReadKey == true) {
					throw JsonFormatError();
				}
				if (match(content, pos, "true")) {
					if (isNonSense(content,pos+4)){
						obj->addData(key,std::make_shared<Object_Bool>(true));
						isNowReadKey = true;
						pos += 4;
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
						std::cout << "读取到值true" << '\n';
#endif
					}
					else {
						throw JsonFormatError();
					}
				}
				break;
			}
			case 'f': {
				if (isNowReadKey == true) {
					throw JsonFormatError();
				}
				if (match(content, pos, "false")) {
					if (isNonSense(content, pos + 5)) {
						obj->addData(key, std::make_shared<Object_Bool>(false));
						isNowReadKey = true;
						pos += 5;
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
						std::cout << "读取到值false" << '\n';
#endif
					}
					else {
						throw JsonFormatError();
					}
				}
				break;
			}
			case 'n': {
				if (isNowReadKey == true) {
					throw JsonFormatError();
				}
				if (match(content, pos, "null")) {
					if (isNonSense(content, pos+4)) {
						obj->addData(key,std::make_shared<Object_NULL>());
						isNowReadKey = true;
						pos += 4;
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
						std::cout << "读取到值null" << '\n';
#endif
					}
					else {
						throw JsonFormatError();
					}
				}
				break;
			}
			default: {
				if (std::isdigit(content[pos])) {
					if (isNowReadKey == true) {
						throw JsonFormatError();
						return obj;
					}
					auto value = readDigit(content, pos);
					obj->addData(key, value);
					isNowReadKey = true;
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
					std::cout << "读取到value:" << value << '\n';
#endif
				}
				break;
			}
			}
		}
		throw JsonFormatError();
		return nullptr;
	}
	
	inline std::shared_ptr<Object_Array> JsonParser::parseArray(const std::string& content, size_t& pos)
	{
		if (content[pos] != '[') return nullptr;
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
		std::cout << "开始解析新Array" << '\n';
#endif
		pos++;

		auto obj = std::make_shared<Object_Array>();

		for (; pos < content.size(); pos++) {
			switch (content[pos])
			{
			case '{': {
				auto son = parseObject(content, pos);
				obj->addData(son);
				break;
			}
			case '[': {
				auto son = parseArray(content, pos);
				obj->addData(son);
				break;
			}
			case ']': {
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
				std::cout << "解析array完成" << '\n';
#endif
				return obj;
			}
			case '"': {		
					auto value = readString(content, pos);
					obj->addData(std::make_shared<Object_String>(value));
					if (pos >= content.size()) {
						throw JsonFormatError();
						return obj;
					}
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
					std::cout << "读取到value:" << value << '\n';
#endif
				
				break;
			}
			case 't': {
				if (match(content, pos, "true")) {
					if (isNonSense(content, pos + 4)) {
						obj->addData(std::make_shared<Object_Bool>(true));
						pos += 4;
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
						std::cout << "读取到值true" << '\n';
#endif
					}
					else {
						throw JsonFormatError();
						return obj;
					}
				}
				break;
			}
			case 'f': {
				if (match(content, pos, "false")) {
					if (isNonSense(content, pos + 5)) {
						obj->addData(std::make_shared<Object_Bool>(false));
						pos += 5;
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
						std::cout << "读取到值false" << '\n';
#endif
					}
					else {
						throw JsonFormatError();
						return obj;
					}
				}
				break;
			}
			case 'n': {
				if (match(content, pos, "null")) {
					if (isNonSense(content, pos + 4)) {
						obj->addData(std::make_shared<Object_NULL>());
						pos += 4;
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
						std::cout << "读取到值null" << '\n';
#endif
					}
					else {
						throw JsonFormatError();
						return obj;
					}
				}
				break;
			}
			default: {
				if (std::isdigit(content[pos])) {
					auto value = readDigit(content, pos);
					obj->addData(value);
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
					std::cout << "读取到value:" << value << '\n';
#endif
				}
				break;
			}
			}
		}
		throw JsonFormatError();
		return nullptr;
	}

	inline std::string JsonParser::readString(const std::string& content, size_t& pos)
	{
		if (content[pos] != '"') {
			return "";
		}
		else {
			pos++;
		}
		size_t beginPos = pos;
		
		for (; content[pos] != '"'; pos++) {
			if (pos >= content.size()) {
				throw std::runtime_error("文件string格式有误");
			}
		}
		pos++;
		return content.substr(beginPos,pos - beginPos - 1);
	}
	inline std::shared_ptr<DataObject> JsonParser::readDigit(const std::string& content, size_t& pos)
	{
		if (std::isdigit(content.at(pos)) == false) return nullptr;
		std::uint64_t intPart = 0;
		for (; pos <content.size() && std::isdigit(content[pos]); pos++) {
			intPart *= 10;
			intPart += content[pos] - '0';
		}
		if (content[pos] == '.') {
			double decPart = 0.0;
			int wei = 0;
			pos++;
			if (pos >= content.size()) {
				throw JsonFormatError();
				return std::make_shared<Object_Long>(intPart);
			}
			for (; pos < content.size() && isdigit(content[pos]); pos++,wei++) {
				decPart *= 10;
				decPart += content[pos] - '0';
			}
			decPart /= std::pow(10, wei);
			decPart += intPart;
			return std::make_shared<Object_Double>(decPart);
		}
		else {
			return std::make_shared<Object_Long>(intPart);
		}
		return nullptr;
	}
	inline bool JsonParser::match(const std::string& content, size_t pos, const std::string& target)
	{
		if (pos >= content.size() || pos + target.size() >= content.size()) return false;
		for (size_t i = 0; i < target.size(); i++) {
			if (content[pos + i] != target[i]) {
				return false;
			}
		}
		return true;
	}
	inline bool JsonParser::isNonSense(const std::string& content, size_t pos)
	{
		return pos < content.size() &&
			std::isalpha(content[pos]) == false &&
			std::isdigit(content[pos]) == false;
	}

	template<typename To, typename From>
	std::shared_ptr<To> castType(const std::shared_ptr<From>& ptr) {
		auto result = std::dynamic_pointer_cast<To>(ptr);
		if (!result) {
			throw std::runtime_error("Dynamic cast failed");
		}
		return result;
	}

	//write

	//输出num个缩进
	void outIndentation(std::ofstream& file,int num) {
		for (int i = 0; i < num; i++) {
			//file << '\t';
			file << "    ";
		}
	}

	void JsonReader::Object_NULL::write(std::ofstream& file, int indentNum)
	{
		file << "null";
	}

	void JsonReader::Object_String::write(std::ofstream& file, int indentNum)
	{
		file << '\"' << getValue() << '\"';
	}

	void JsonReader::Object_Long::write(std::ofstream& file, int indentNum)
	{
		file << getValue();
	}

	void JsonReader::Object_Double::write(std::ofstream& file, int indentNum)
	{
		file << getValue();
	}

	void JsonReader::Object_Bool::write(std::ofstream& file, int indentNum)
	{
		file << (getValue() == true ? "true" : "false");
	}

	void JsonReader::Object_Array::write(std::ofstream& file, int indentNum)
	{
		file << '[' << '\n';
		for (int i = 0; i < objects.size(); i++) {
			outIndentation(file,indentNum);
			objects[i]->write(file, indentNum + 1);
			if (i != objects.size() - 1) {
				file << ',';
			}
			file << '\n';
		}
		outIndentation(file, indentNum-1);
		file << ']';
	}

	inline void JsonReader::Object_Object::write(std::ofstream& file, int indentNum)
	{
		file << '{' << '\n';
		int i = 0;
		for (auto& it : objects) {
			outIndentation(file, indentNum);
			file << '\"' << it.first << '\"' << ": ";
			it.second->write(file, indentNum + 1);
			if (i != objects.size() - 1) {
				file << ',';
			}
			file << '\n';
			i++;
		}
		outIndentation(file, indentNum-1);
		file << '}';
	}

	bool writeToFile(const std::string& fileName, std::shared_ptr<DataObject> obj) {
		std::ofstream outFile(fileName);

		if (outFile.is_open()) {
			obj->write(outFile);
			outFile.close();
#ifdef __ENABLE_DEBUG_JSON_INFO_OUT
			std::cout << "数据已成功保存到文件: " << fileName  << '\n';
#endif
			return true;
		}
		else {
			throw std::runtime_error("打开文件失败: " + fileName);
			return false;
		}
	}
}

namespace LJR = JsonReader;