---
tags:
  - qt/core
  - type/reference
mastery: 5
---
## 相关

- [[QVarient]] -- JSON 值在 Qt 中常用 QVariant 包装传递

JSON 是一种用于存储结构化数据的格式。它有 6 种基本数据类型：

- bool
- double
- string
- array
- object
- null

## QJsonObject

封装 JSON 对象，是键值对的集合。可以使用 `json[QString] =` 的方式修改 QJsonObject 对象。

```cpp
void Level::write(QJsonObject &json) const
{
    json["name"] = mName;     // QString -> QJsonValue
    QJsonArray npcArray;
    for (const Character &npc : mNpcs) {
        QJsonObject npcObject;
        npc.write(npcObject);
        npcArray.append(npcObject);
    }
    json["npcs"] = npcArray;   // QJsonArray -> QJsonValue
}
```

## QJsonArray

封装 JSON 数组，是有序的值列表。可以嵌套 QJsonObject 和 QJsonValue。

```cpp
QJsonArray array;
array.append(1);
array.append("hello");
array.append(QJsonObject{{"key", "value"}});
```

## QJsonValue

封装 JSON 的值，不同于 QJsonObject，它只保存键值对中值的部分：

> 说明

```json
{
    "person": {
        "name": "guo",
        "age": 22
    }
}
```

上面整个部分是一个 `QJsonObject json`，`json["person"]` 返回的是 QJsonValue。

QJsonValue 可以是多种类型：

- `QJsonValue::Bool` -- bool
- `QJsonValue::Double` -- double
- `QJsonValue::String` -- string
- `QJsonValue::Array` -- QJsonArray
- `QJsonValue::Object` -- QJsonObject
- `QJsonValue::Null` -- null

可以使用 `isString()`、`isObject()` 等函数判断 QJsonValue 的类型。`json["person"]` 返回的是 `QJsonValue::Object` 类型，因此可以使用 `json["person"].toObject()` 返回一个新的 `QJsonObject` 对象，包含：

```json
{
    "name": "guo",
    "age": 22
}
```

## QJsonDocument

QJsonDocument 是一个包装完整 JSON 文档的类，可以从基于 UTF-8 编码的文本表示以及 Qt 自己的二进制格式读取和写入此文档。

> 设计这个类的原因：JSON 文档的顶层可以是 JSON 对象（键值对），也可以是 JSON 数组，QJsonDocument 统一封装了这两种情况。

```json
["a", "b"]
```

```json
{
    "number": 3,
    "age": 4
}
```

**转换关系**：

```
QJsonObject / QJsonArray  <-->  QJsonDocument  <-->  QByteArray
```

```cpp
// 从 JSON 字符串解析
QJsonDocument doc = QJsonDocument::fromJson(jsonBytes);
QJsonObject obj = doc.object();

// 序列化为 JSON 字符串
QJsonDocument doc(obj);
QByteArray bytes = doc.toJson(QJsonDocument::Compact);
```
