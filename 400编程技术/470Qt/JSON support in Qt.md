JSON 是一种用于存储结构化数据的格式。它有 6 种基本数据类型：

- bool
- double
- string
- array
- object
- null

## QJsonObject

封装 JSON 对象，是键值对部分

可以使用 `json[Qstring] = ` 的方式修改 QJsonObject 对象

```cpp
void Level::write(QJsonObject &json) const
{
    json["name"] = mName;     // QString
    QJsonArray npcArray;
    for (const Character &npc : mNpcs) {
        QJsonObject npcObject;
        npc.write(npcObject);
        npcArray.append(npcObject);
    }
    json["npcs"] = npcArray;   // QJsonArray
}
```

## QJsonArray

封装 JSON 数组


## QJsonValue

封装 JSON 的值，不同于 QJsonObject ，它只保存着键值对值的部分：

> 说明

```json
{
    person : {
        name : "guo",
        age : 22
    }
}
```

上面整个部分是一个 `QJsonObject json`，  `json["person"]` 返回的是 QJsonValue。

QJsonValue 可以是多种对象：

- bool QJsonValue::Bool
- double QJsonValue::Double
- string QJsonValue::String
- array QJsonValue::Array
- object QJsonValue::Object
- null QJsonValue::Null

有一些函数可以去判断 QJsonValue 的类型，而 `json["person"]` 返回的是 `QJsonValue::Object` 类型

因此，可以使用 `json["person"].toObject()` 返回一个新的 `QJsonObject` 对象，它的内容包含

```json
{
    name : "guo",
    age : 22
}
```


## JsonDocument

QJsonDocument 是一个包装完整 JSON 文档的类，可以从基于 UTF-8 编码的文本表示以及 Qt 自己的二进制格式读取和写入此文档。

> 设计这个类的原因可能是因为 JSON 字符串可能是 JSON 对象（键值对），也可能是 JSON 数组

```json
[
    "a",
    "b"
]
```

```json
{
    "number":3,
    "age" : 4
}
```

JsonObject(JsonArray) <-> JsonDocument <-> QByteArray

