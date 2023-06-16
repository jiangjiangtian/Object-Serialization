#ifndef __XML_H_
#define __XML_H_

#include <iostream>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>

#include "helper.h"
#include "tinyxml2.h"

namespace detail {

template <typename T>
typename std::enable_if<!is_pair<std::remove_reference_t<T>>::value &&
                        !is_tuple<std::remove_reference_t<T>>::value &&
                        !is_smart_ptr<std::remove_reference_t<T>>::value>::type
serialize_xml_stl(T &&val, tinyxml2::XMLDocument *doc, tinyxml2::XMLNode *object);

template <typename T>
typename std::enable_if<is_pair<std::remove_reference_t<T>>::value>::type
serialize_xml_stl(T &&val, tinyxml2::XMLDocument *doc, tinyxml2::XMLNode *object);

template <typename T>
typename std::enable_if<is_tuple<std::remove_reference_t<T>>::value>::type
serialize_xml_stl(T &&val, tinyxml2::XMLDocument *doc, tinyxml2::XMLNode *object);

template <typename T>
typename std::enable_if<is_smart_ptr<std::remove_reference_t<T>>::value>::type
serialize_xml_stl(T &&val, tinyxml2::XMLDocument *doc, tinyxml2::XMLNode *object);

template <typename T>
void deserialize_xml_stl(std::vector<T> &val, tinyxml2::XMLElement *object);

template <typename T>
void deserialize_xml_stl(std::list<T> &val, tinyxml2::XMLElement *object);

template <typename T>
void deserialize_xml_stl(std::set<T> &val, tinyxml2::XMLElement *object);

template <typename T1, typename T2>
void deserialize_xml_stl(std::map<T1, T2> &val, tinyxml2::XMLElement *object);

template <typename T1, typename T2>
void deserialize_xml_stl(std::pair<T1, T2> &val, tinyxml2::XMLElement *object);

template <typename T>
void deserialize_xml_stl(std::shared_ptr<T> &val, tinyxml2::XMLElement *object);

template <typename T>
void deserialize_xml_stl(std::unique_ptr<T> &val, tinyxml2::XMLElement *object);

template <typename T>
typename std::enable_if<is_tuple<std::remove_reference_t<T>>::value>::type
deserialize_xml_stl(T &val, tinyxml2::XMLElement *object);

} // namespace detail

namespace xml {

template <typename T>
typename std::enable_if<detail::stl_container<std::remove_reference_t<T>>::value>::type
serialize_xml_helper(T &&val, tinyxml2::XMLDocument *doc, tinyxml2::XMLNode *element, const char *attribute_name) {
    tinyxml2::XMLNode *object = element->InsertEndChild(doc->NewElement(attribute_name));
    detail::serialize_xml_stl(val, doc, object);
}

template <typename T>
typename std::enable_if<std::is_arithmetic_v<std::remove_reference_t<T>>>::type
serialize_xml_helper(T &&val, tinyxml2::XMLDocument *doc, tinyxml2::XMLNode *object, const char *attribute_name) {
    auto attri = doc->NewElement(attribute_name);
    attri->SetAttribute("val", val);
    object->InsertEndChild(attri);
}

template <typename T>
typename std::enable_if<std::is_same_v<std::remove_reference_t<T>, std::string>>::type
serialize_xml_helper(T &&val, tinyxml2::XMLDocument *doc, tinyxml2::XMLNode *object, const char *attribute_name) {
    auto attri = doc->NewElement(attribute_name);
    attri->SetAttribute("val", val.c_str());
    object->InsertEndChild(attri);
}

template <typename T>
typename std::enable_if<!detail::is_not_user_type<std::remove_reference_t<T>>::value &&
                        detail::has_get_all_member<std::remove_reference_t<T>>::value>::type
serialize_xml_helper(T &&val, tinyxml2::XMLDocument *doc, tinyxml2::XMLNode *object, const char *attribute_name) {
    serialize_xml_helper(val.get_all_member(), doc, object, attribute_name);
}

template <typename T>
typename std::enable_if<std::is_arithmetic_v<std::remove_reference_t<T>>>::type
serialize_xml(T &&val, std::string object_name, std::string file_name) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLNode *element = doc.InsertEndChild(doc.NewElement("serialization"));
    tinyxml2::XMLNode *object = element->InsertFirstChild(doc.NewElement(object_name.c_str()));
    serialize_xml_helper(val, &doc, object, typeid(std::remove_reference_t<T>).name());
    doc.SaveFile(file_name.c_str());
}

template <typename T>
typename std::enable_if<detail::stl_container<std::remove_reference_t<T>>::value>::type
serialize_xml(T &&val, std::string object_name, std::string file_name) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLNode *element = doc.InsertEndChild(doc.NewElement("serialization"));
    serialize_xml_helper(val, &doc, element, object_name.c_str());
    doc.SaveFile(file_name.c_str());
}

template <typename T>
typename std::enable_if<std::is_same_v<std::remove_reference_t<T>, std::string>>::type
serialize_xml(T &&val, std::string object_name, std::string file_name) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLNode *element = doc.InsertEndChild(doc.NewElement("serialization"));
    tinyxml2::XMLNode *object = element->InsertFirstChild(doc.NewElement(object_name.c_str()));
    serialize_xml_helper(val, &doc, object, "str");
    doc.SaveFile(file_name.c_str());
}

template <typename T>
typename std::enable_if<!detail::is_not_user_type<std::remove_reference_t<T>>::value &&
                        detail::has_get_all_member<std::remove_reference_t<T>>::value>::type
serialize_xml(T &&val, std::string object_name, std::string file_name) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLNode *element = doc.InsertEndChild(doc.NewElement("serialization"));
    tinyxml2::XMLNode *object = element->InsertFirstChild(doc.NewElement(object_name.c_str()));
    serialize_xml_helper(val, &doc, object, "struct");
    doc.SaveFile(file_name.c_str());
}

template <typename T>
typename std::enable_if<std::is_integral_v<std::remove_reference_t<T>> &&
                                             (sizeof(T) <= 4) &&
                                             !std::is_unsigned_v<std::remove_reference_t<T>>>::type
deserialize_xml_helper(T &val, tinyxml2::XMLElement *attr) {
    int value;
    attr->QueryAttribute("val", &value);
    val = value;
}

template <typename T>
typename std::enable_if<std::is_integral_v<std::remove_reference_t<T>> &&
                                             (sizeof(T) <= 4) &&
                                             std::is_unsigned_v<std::remove_reference_t<T>>>::type
deserialize_xml_helper(T &val, tinyxml2::XMLElement *attr) {
    unsigned value;
    attr->QueryAttribute("val", &value);
    val = value;
}

template <typename T>
typename std::enable_if<std::is_integral_v<std::remove_reference_t<T>> &&
                                             (sizeof(T) > 4) &&
                                             !std::is_unsigned_v<std::remove_reference_t<T>>>::type
deserialize_xml_helper(T &val, tinyxml2::XMLElement *attr) {
    attr->QueryAttribute("val", reinterpret_cast<int64_t *>(&val));
}

template <typename T>
typename std::enable_if<std::is_integral_v<std::remove_reference_t<T>> &&
                                             (sizeof(T) > 4) &&
                                             std::is_unsigned_v<std::remove_reference_t<T>>>::type
deserialize_xml_helper(T &val, tinyxml2::XMLElement *attr) {
    attr->QueryAttribute("val", reinterpret_cast<uint64_t *>(&val));
}

template <typename T>
typename std::enable_if<std::is_floating_point_v<std::remove_reference_t<T>>>::type
deserialize_xml_helper(T &val, tinyxml2::XMLElement *attr) {
    attr->QueryAttribute("val", &val);
}

template <typename T>
typename std::enable_if<detail::stl_container<std::remove_reference_t<T>>::value>::type
deserialize_xml_helper(T &val, tinyxml2::XMLElement *object) {
    detail::deserialize_xml_stl(val, object);
}

template <typename T>
typename std::enable_if<std::is_same_v<std::remove_reference_t<T>, std::string>>::type
deserialize_xml_helper(T &val, tinyxml2::XMLElement *attr) {
    val = attr->Attribute("val");
}

template <typename T>
typename std::enable_if<!detail::is_not_user_type<std::remove_reference_t<T>>::value &&
                        detail::has_get_all_member<std::remove_reference_t<T>>::value>::type
deserialize_xml_helper(T &val, tinyxml2::XMLElement *attr) {
    decltype(val.get_all_member()) tup;
    deserialize_xml_helper(tup, attr);
    tuple_helper::construct_object(val, tup);
}

template <typename T>
typename std::enable_if<std::is_arithmetic_v<std::remove_reference_t<T>>>::type
deserialize_xml(T &val, std::string object_name, std::string file_name) {
    tinyxml2::XMLDocument doc;
    auto error = doc.LoadFile(file_name.c_str());
    if (error != tinyxml2::XMLError::XML_SUCCESS) {
        throw std::logic_error(tinyxml2::XMLDocument::ErrorIDToName(error));
    }
    auto element = doc.FirstChildElement("serialization")->FirstChildElement(object_name.c_str());
    deserialize_xml_helper(val, element->FirstChildElement(typeid(std::remove_reference_t<T>).name()));
}

template <typename T>
typename std::enable_if<detail::stl_container<std::remove_reference_t<T>>::value>::type
deserialize_xml(T &val, std::string object_name, std::string file_name) {
    tinyxml2::XMLDocument doc;
    auto error = doc.LoadFile(file_name.c_str());
    if (error != tinyxml2::XMLError::XML_SUCCESS) {
        throw std::logic_error(tinyxml2::XMLDocument::ErrorIDToName(error));
    }
    auto element = doc.FirstChildElement("serialization")->FirstChildElement(object_name.c_str());
    deserialize_xml_helper(val, element);
}

template <typename T>
typename std::enable_if<std::is_same_v<std::remove_reference_t<T>, std::string>>::type
deserialize_xml(T &val, std::string object_name, std::string file_name) {
    tinyxml2::XMLDocument doc;
    auto error = doc.LoadFile(file_name.c_str());
    if (error != tinyxml2::XMLError::XML_SUCCESS) {
        throw std::logic_error(tinyxml2::XMLDocument::ErrorIDToName(error));
    }
    auto element = doc.FirstChildElement("serialization")->FirstChildElement(object_name.c_str());
    deserialize_xml_helper(val, element->FirstChildElement("str"));
}

template <typename T>
typename std::enable_if<!detail::is_not_user_type<std::remove_reference_t<T>>::value &&
                        detail::has_get_all_member<std::remove_reference_t<T>>::value>::type
deserialize_xml(T &val, std::string object_name, std::string file_name) {
    tinyxml2::XMLDocument doc;
    auto error = doc.LoadFile(file_name.c_str());
    if (error != tinyxml2::XMLError::XML_SUCCESS) {
        throw std::logic_error(tinyxml2::XMLDocument::ErrorIDToName(error));
    }
    auto element = doc.FirstChildElement("serialization")->FirstChildElement(object_name.c_str());
    deserialize_xml_helper(val, element->FirstChildElement("struct"));
}

} // namespace xml

namespace detail {

template <typename T>
typename std::enable_if<!is_pair<std::remove_reference_t<T>>::value &&
                        !is_tuple<std::remove_reference_t<T>>::value &&
                        !is_smart_ptr<std::remove_reference_t<T>>::value>::type
serialize_xml_stl(T &&val, tinyxml2::XMLDocument *doc, tinyxml2::XMLNode *object) {
    int i = 0;
    for (auto && v : val) {
        std::string name = "element" + std::to_string(i);
        xml::serialize_xml_helper(std::forward<decltype(v)>(v), doc, object, name.c_str());
        i++;
    }
}

template <typename T>
typename std::enable_if<is_pair<std::remove_reference_t<T>>::value>::type
serialize_xml_stl(T &&val, tinyxml2::XMLDocument *doc, tinyxml2::XMLNode *object) {
    xml::serialize_xml_helper(val.first, doc, object, "first");
    xml::serialize_xml_helper(val.second, doc, object, "second");
}

template <typename T>
typename std::enable_if<is_tuple<std::remove_reference_t<T>>::value>::type
serialize_xml_stl(T &&val, tinyxml2::XMLDocument *doc, tinyxml2::XMLNode *object) {
    int i = 0;
    tuple_helper::tuple_for_each([&doc, &object, &i](auto &&val) {
        std::string name = "element" + std::to_string(i);
        xml::serialize_xml_helper(std::forward<decltype(val)>(val), doc, object, name.c_str());
        i++;
    }, std::forward<T>(val));
}

template <typename T>
typename std::enable_if<is_smart_ptr<std::remove_reference_t<T>>::value>::type
serialize_xml_stl(T &&val, tinyxml2::XMLDocument *doc, tinyxml2::XMLNode *object) {
    xml::serialize_xml_helper(*val, doc, object, "content");
}

template <typename T>
void deserialize_xml_stl(std::vector<T> &val, tinyxml2::XMLElement *object) {
    auto attri = object->FirstChild();
    for (; attri != nullptr; attri = attri->NextSibling()) {
        T value;
        xml::deserialize_xml_helper(value, attri->ToElement());
        val.emplace_back(std::move(value));
    }
}

template <typename T>
void deserialize_xml_stl(std::list<T> &val, tinyxml2::XMLElement *object) {
    auto attri = object->FirstChild();
    for (; attri != nullptr; attri = attri->NextSibling()) {
        T value;
        xml::deserialize_xml_helper(value, attri->ToElement());
        val.emplace_back(std::move(value));
    }
}

template <typename T>
void deserialize_xml_stl(std::set<T> &val, tinyxml2::XMLElement *object) {
    auto attri = object->FirstChild();
    for (; attri != nullptr; attri = attri->NextSibling()) {
        T value;
        xml::deserialize_xml_helper(value, attri->ToElement());
        val.emplace(std::move(value));
    }
}

template <typename T1, typename T2>
void deserialize_xml_stl(std::map<T1, T2> &val, tinyxml2::XMLElement *object) {
    auto attri = object->FirstChild();
    for (; attri != nullptr; attri = attri->NextSibling()) {
        std::pair<T1, T2> value;
        xml::deserialize_xml_helper(value, attri->ToElement());
        val.emplace(std::move(value));
    }
}

template <typename T1, typename T2>
void deserialize_xml_stl(std::pair<T1, T2> &val, tinyxml2::XMLElement *object) {
    xml::deserialize_xml_helper(val.first, object->FirstChildElement("first"));
    xml::deserialize_xml_helper(val.second, object->FirstChildElement("second"));
}

template <int N, typename... Args>
struct deserialize_tuple_helper {
    static void deserialize_tuple(std::tuple<Args...> &tup, tinyxml2::XMLElement *object) {
        std::string name = "element" + std::to_string(N);
        xml::deserialize_xml_helper(std::get<N>(tup), object->FirstChildElement(name.c_str()));
    }
};

template <int... Index, typename... Args>
void deserialize_tuple_helper_func(tuple_helper::IndexTuple<Index...>,
                              std::tuple<Args...> &tup,
                              tinyxml2::XMLElement *object) {
    int a[] = {(deserialize_tuple_helper<Index, Args...>::deserialize_tuple(tup, object), 0)...};                            
}

template <typename... Args>
void deserialize_tuple(std::tuple<Args...> &tup, tinyxml2::XMLElement *object) {
    using tuple_index = typename tuple_helper::MakeIndex<std::tuple_size_v<std::tuple<Args...>>>::tuple_index;
    deserialize_tuple_helper_func(tuple_index(), tup, object);
}

template <typename T>
typename std::enable_if<is_tuple<std::remove_reference_t<T>>::value>::type
deserialize_xml_stl(T &val, tinyxml2::XMLElement *object) {
    deserialize_tuple(val, object);
}

template <typename T>
void deserialize_xml_stl(std::shared_ptr<T> &val, tinyxml2::XMLElement *object) {
    T value;
    auto attri = object->FirstChildElement("content");
    xml::deserialize_xml_helper(value, attri);
    val = std::make_shared<T>(std::move(value));
}

template <typename T>
void deserialize_xml_stl(std::unique_ptr<T> &val, tinyxml2::XMLElement *object) {
    T value;
    auto attri = object->FirstChildElement("content");
    xml::deserialize_xml_helper(value, attri);
    val = std::make_unique<T>(std::move(value));
}

} // namespace detail

#endif