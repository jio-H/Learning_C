#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>

class MetaField {
public:
    enum Type { INT32, STRING };

    MetaField() : type(INT32), offset(0) {} // 添加默认构造函数
    MetaField(Type type, size_t offset) : type(type), offset(offset) {}

    Type type;
    size_t offset;  // 字段在类中的偏移量
};

class MetaClass {
public:
    std::unordered_map<std::string, MetaField> fields;

    void addField(const std::string& name, MetaField::Type type, size_t offset) {
        fields[name] = MetaField(type, offset);
    }
};

#define OFFSET_OF(type, member) offsetof(type, member)

class Person {
public:
    int32_t id;
    std::string name;
    std::string email;

    Person(int32_t id, const std::string& name, const std::string& email)
        : id(id), name(name), email(email) {}
};

MetaClass createPersonMetaClass() {
    MetaClass metaClass;
    metaClass.addField("id", MetaField::INT32, OFFSET_OF(Person, id));
    metaClass.addField("name", MetaField::STRING, OFFSET_OF(Person, name));
    metaClass.addField("email", MetaField::STRING, OFFSET_OF(Person, email));
    return metaClass;
}

std::vector<uint8_t> serializeObject(const void* object, const MetaClass& metaClass) {
    std::vector<uint8_t> data;

    for (const auto& field : metaClass.fields) {
        const std::string& name = field.first;
        const MetaField& metaField = field.second;

        if (metaField.type == MetaField::INT32) {
            int32_t value = *reinterpret_cast<const int32_t*>(
                reinterpret_cast<const uint8_t*>(object) + metaField.offset);
            data.insert(
                data.end(), 
                reinterpret_cast<const uint8_t*>(&value),
                reinterpret_cast<const uint8_t*>(&value) + sizeof(int32_t));
        } else if (metaField.type == MetaField::STRING) {
            const std::string& value = *reinterpret_cast<const std::string*>(
                reinterpret_cast<const uint8_t*>(object) + metaField.offset);
            int32_t length = value.size();
            data.insert(
                data.end(), 
                reinterpret_cast<const uint8_t*>(&length), 
                reinterpret_cast<const uint8_t*>(&length) + sizeof(int32_t));
            data.insert(data.end(), value.begin(), value.end());
        }
    }

    return data;
}

void deserializeObject(void* object, const MetaClass& metaClass, const std::vector<uint8_t>& data) {
    size_t offset = 0;

    for (const auto& field : metaClass.fields) {
        const std::string& name = field.first;
        const MetaField& metaField = field.second;

        if (metaField.type == MetaField::INT32) {
            int32_t value = *reinterpret_cast<const int32_t*>(&data[offset]);
            *reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(object) + metaField.offset) = value;
            offset += sizeof(int32_t);
        } else if (metaField.type == MetaField::STRING) {
            int32_t length = *reinterpret_cast<const int32_t*>(&data[offset]);
            offset += sizeof(int32_t);
            std::string value(data.begin() + offset, data.begin() + offset + length);
            *reinterpret_cast<std::string*>(reinterpret_cast<uint8_t*>(object) + metaField.offset) = value;
            offset += length;
        }
    }
}

int main() {
    // 创建Person对象
    Person person(123, "John Doe", "johndoe@example.com");

    // 创建元数据类
    MetaClass personMetaClass = createPersonMetaClass();

    // 序列化Person对象
    std::vector<uint8_t> serializedData = serializeObject(&person, personMetaClass);

    // 打印序列化后的二进制数据
    std::cout << "Serialized binary data: ";
    for (uint8_t byte : serializedData) {
        std::cout << std::hex << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    // 反序列化Person对象
    Person deserializedPerson(0, "", "");
    deserializeObject(&deserializedPerson, personMetaClass, serializedData);

    // 打印反序列化后的对象数据
    std::cout << "Deserialized Person:" << std::endl;
    std::cout << "ID: " << deserializedPerson.id << std::endl;
    std::cout << "Name: " << deserializedPerson.name << std::endl;
    std::cout << "Email: " << deserializedPerson.email << std::endl;

    return 0;
}