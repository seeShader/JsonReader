//开启调试信息
#define __ENABLE_DEBUG_JSON_INFO_OUT
#include "JsonReader.hpp"

#include <iostream>

int main() {
	//加载json文件
	auto p = LJR::parseFile("testFile/sample.json");
	if (p == nullptr) {
		std::cout << "加载文件失败" << '\n';
		return -1;
	}

	//转化类型为Object_Object，即{}
	auto obj = LJR::castType<LJR::Object_Object>(p);

	//获取属性
	auto value = obj->getValue("nested_object");
	if (value == nullptr) {
		std::cout << "获取属性失败" << '\n';
		return -1;
	}

	//获取属性的同时转化类型
	std::cout << LJR::castType<LJR::Object_Object>(value)->getValue_String("nested_string")->getValue() << '\n';

	//读取整型类型
	std::cout << obj->getValue_Long("number_key")->getValue() << '\n';
	std::cout << obj->getValue_Long("number_key_nagetive")->getValue() << '\n';

	//读取浮点类型
	std::cout << obj->getValue_Double("number_key_float")->getValue() << '\n';

	//读取布尔类型
	std::cout << obj->getValue_Bool("boolean_key_true")->getValue() << '\n';
	std::cout << obj->getValue_Bool("boolean_key_false")->getValue() << '\n';

	//读取字符串类型
	std::cout << obj->getValue_String("string_key")->getValue() << '\n';

	//读取数组类型
	std::cout << obj->getValue_Array("array_key")->getSize() << '\n';

	//添加新属性
	obj->addData("newValue", std::make_shared<LJR::Object_Double>(100.2));

	//写入文件
	LJR::writeToFile("testFile/newJson.json",obj);

	return 0;
}