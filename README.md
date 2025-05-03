# JSON 解析库使用说明

## 一、项目简介

本项目提供了一个 C++ 实现的 JSON 解析库，用于解析 JSON 数据、访问 JSON 对象中的值、添加和删除数据，以及将数据写入文件。能满足基本的 JSON 处理需求。

## 二、代码结构

`example.cpp`：包含示例代码，展示如何使用 JSON 解析库的各项功能。

`JsonReader.hpp`：定义了 JSON 解析库的核心类和函数，实现了 JSON 数据的解析、类型转换、数据操作和文件写入等功能。

## 三、使用方法

该库以单头文件形式提供，使用时只需包含`JsonReader.hpp`头文件即可：

```
\#include "JsonReader.hpp"
```

具体如何使用可查看`example.cpp`中的示例代码。