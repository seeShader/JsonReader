//����������Ϣ
#define __ENABLE_DEBUG_JSON_INFO_OUT
#include "JsonReader.hpp"

#include <iostream>

int main() {
	//����json�ļ�
	auto p = LJR::parseFile("testFile/sample.json");
	if (p == nullptr) {
		std::cout << "�����ļ�ʧ��" << '\n';
		return -1;
	}

	//ת������ΪObject_Object����{}
	auto obj = LJR::castType<LJR::Object_Object>(p);

	//��ȡ����
	auto value = obj->getValue("nested_object");
	if (value == nullptr) {
		std::cout << "��ȡ����ʧ��" << '\n';
		return -1;
	}

	//��ȡ���Ե�ͬʱת������
	std::cout << LJR::castType<LJR::Object_Object>(value)->getValue_String("nested_string")->getValue() << '\n';

	//��ȡ��������
	std::cout << obj->getValue_Long("number_key")->getValue() << '\n';
	std::cout << obj->getValue_Long("number_key_nagetive")->getValue() << '\n';

	//��ȡ��������
	std::cout << obj->getValue_Double("number_key_float")->getValue() << '\n';

	//��ȡ��������
	std::cout << obj->getValue_Bool("boolean_key_true")->getValue() << '\n';
	std::cout << obj->getValue_Bool("boolean_key_false")->getValue() << '\n';

	//��ȡ�ַ�������
	std::cout << obj->getValue_String("string_key")->getValue() << '\n';

	//��ȡ��������
	std::cout << obj->getValue_Array("array_key")->getSize() << '\n';

	//���������
	obj->addData("newValue", std::make_shared<LJR::Object_Double>(100.2));

	//д���ļ�
	LJR::writeToFile("testFile/newJson.json",obj);

	return 0;
}