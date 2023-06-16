#include <assert.h>
#include <memory>
#include <vector>
#include "../include/xml.h"

struct MyStruct {
    int a;
    double b;
    std::string c;

    MyStruct() {}

    MyStruct(int a1, double b1, std::string c1) : a(a1), b(b1), c(std::move(c1)) {}

    auto get_all_member() -> decltype(auto) {
        return std::make_tuple(a, b, c);
    }
};

bool operator==(const MyStruct &lhs, const MyStruct &rhs) {
    return lhs.a == rhs.a && lhs.b == rhs.b && lhs.c == rhs.c;
}

struct UserDefinedType {
    int idx;
    std::string name;
    std::vector<double> data;

    UserDefinedType() {}

    UserDefinedType(int i, std::string n, std::vector<double> d) : idx(i), name(std::move(n)), data(std::move(d)) {}

    auto get_all_member() -> decltype(auto) {
        return std::make_tuple(idx, name, data);
    }
};

bool operator==(const UserDefinedType &lhs, const UserDefinedType &rhs) {
    return lhs.idx == rhs.idx && lhs.name == rhs.name && lhs.data == rhs.data;
}

/**
 * test_arithmetic - test the serialization and deserialization of arithmetic types,
 * like int, double, short, etc.
 */
template <typename T>
void test_arithmetic(T val, std::string object_name, std::string file_name) {
    T n0 = val, n1;
    // serialize object n0 to an XML file file_name with the name object_name
    xml::serialize_xml(n0, object_name, file_name);
    // reconstruct object n1 to an XML file file_name with the name object_name
    xml::deserialize_xml(n1, object_name, file_name);
    std::cout << "Serialize: " << n0 << std::endl;
    std::cout << "Deserialize: " << n1 << std::endl;
    if (n0 == n1) {
        std::cout << "[true]\n";
    } else {
        std::cout << "[false]\n";
    }
}

/**
 * test_stl - test the serialization and deserialization of some stl types,
 * like vector, list, set
 */
template <typename T>
void test_stl(const T &val, std::string object_name, std::string file_name) {
    T n0 = val, n1;
    // serialize object n0 to an XML file file_name with the name object_name
    xml::serialize_xml(n0, object_name, file_name);
    // reconstruct object n1 to an XML file file_name with the name object_name
    xml::deserialize_xml(n1, object_name, file_name);
    std::cout << "Serialize: ";
    for (auto &v : n0) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    std::cout << "Deserialize: ";
    for (auto &v : n1) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    if (n0 == n1) {
        std::cout << "[true]\n";
    } else {
        std::cout << "[false]\n";
    }
}

int main() {
    std::cout << "Test for serializing int: \n";
    test_arithmetic(1, "int", "n.xml");

    std::cout << "Test for serializing short: \n";
    test_arithmetic(static_cast<short>(10), "short", "short.xml");

    std::cout << "Test for serializing char: \n";
    test_arithmetic('a', "char", "char.xml");

    std::cout << "Test for serializing long long: \n";
    //test_arithmetic((1ll << 32), "longlong", "longlong.xml");

    std::cout << "Test for serializing double: \n";
    test_arithmetic(2.500, "double", "d.xml");

    std::cout << "Test for serializing float: \n";
    test_arithmetic(2.500f, "float", "float.xml");

    std::cout << "Test for serializing std::vector<int>: \n";
    test_stl(std::vector<int>{1, 2, 3, 4, 5}, "std_vector", "v.xml");
    std::vector<int> v1{1, 2, 3, 4, 5}, v2;

    std::cout << "Test for serializing std::vector<std::vector<int>>: \n";
    std::vector<std::vector<int>> vv1 = {v1, v1}, vv2;
    xml::serialize_xml(vv1, "std_vector_vector", "vv.xml");
    xml::deserialize_xml(vv2, "std_vector_vector", "vv.xml");
    std::cout << "Serialize:\n";
    for (auto &vv : vv1) {
        for (auto &v : vv) {
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "Deserialize:\n";
    for (auto &vv : vv2) {
        for (auto &v : vv) {
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }
    if (vv1 == vv2) {
        std::cout << "[true]\n";
    } else {
        std::cout << "[false]\n";
    }

    std::cout << "Test for serializing std::pair<int, double>: \n";
    std::pair<int, double> p1 {1, 2.54}, p2;
    xml::serialize_xml(p1, "std_pair", "p.xml");
    xml::deserialize_xml(p2, "std_pair", "p.xml");
    std::cout << "Serialize: " << p1.first << " " << p1.second << std::endl;
    std::cout << "Deserialize: " << p2.first << " " << p2.second << std::endl;

    std::cout << "Test for serializing std::set<int>: \n";
    test_stl(std::set<int>{1, 2, 3, 4, 5, 9, 6}, "std_set", "s.xml");

    std::cout << "Test for serializing std::map<int, double>: \n";
    std::map<int, double> m1{{1, 2.5}, {2, 3.5}, {3, 4.5}}, m2;
    xml::serialize_xml(m1, "std_map", "m.xml");
    xml::deserialize_xml(m2, "std_map", "m.xml");
    std::cout << "Serialize:\n";
    for (auto &m : m1) {
        std::cout << m.first << " " << m.second << std::endl;
    }
    std::cout << "Deserialize:\n";
    for (auto &m : m2) {
        std::cout << m.first << " " << m.second << std::endl;
    }
    if (m1 == m2) {
        std::cout << "[true]\n";
    } else {
        std::cout << "[false]\n";
    }

    std::cout << "Test for serializing std::map<int, std::vector<int>>: \n";
    std::map<int, std::vector<int>> mv1{{1, v1}, {2, v1}, {3, v1}}, mv2;
    xml::serialize_xml(mv1, "std_map", "mv.xml");
    xml::deserialize_xml(mv2, "std_map", "mv.xml");
    std::cout << "Serialize:\n";
    for (auto &m : mv1) {
        std::cout << m.first << " ";
        for (auto &v : m.second) {
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "Deserialize:\n";
    for (auto &m : mv2) {
        std::cout << m.first << " ";
        for (auto &v : m.second) {
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }
    if (m1 == m2) {
        std::cout << "[true]\n";
    } else {
        std::cout << "[false]\n";
    }

    std::cout << "Test for serializing std::list<int>: \n";
    test_stl(std::list<int>{1, 2, 3, 4, 5}, "std_list", "l.xml");

    std::cout << "Test for serializing std::string: \n";
    std::string str1("Hello, World!"), str2;
    xml::serialize_xml(str1, "std_string", "str.xml");
    xml::deserialize_xml(str2, "std_string", "str.xml");
    std::cout << "Serialize: " << str1 << std::endl << "Deserialize: " << str2 << std::endl;

    std::cout << "Test for serializing std::vector<std::string>: \n";
    test_stl(std::vector<std::string>{"Hello, world!", "Hello", "World"}, "std_vector", "ves.xml");

    std::cout << "Test for serializing std::tuple<int, std::string, double>:\n";
    auto f = [](auto &&val) { std::cout << val << std::endl; };
    std::tuple<int, std::string, double> t1{1, "Hello, world", 2.5}, t2;
    xml::serialize_xml(t1, "std_tuple", "t.xml");
    std::cout << "Serialize:\n";
    tuple_helper::tuple_for_each(f, t1);
    xml::deserialize_xml(t2, "std_tuple", "t.xml");
    std::cout << "Deserialize:\n";
    tuple_helper::tuple_for_each(f, t2);
    if (t1 == t2) {
        std::cout << "[true]\n";
    } else {
        std::cout << "[false]\n";
    }

    std::cout << "Test for serializing MyStruct: \n";
    MyStruct struct1 {1, 2.5, "Hello, world!"}, struct2;
    xml::serialize_xml(struct1, "MyStruct", "struct.xml");
    xml::deserialize_xml(struct2, "MyStruct", "struct.xml");
    std::cout << "Serialize: " << struct1.a << " " << struct1.b << " " << struct1.c << std::endl;
    std::cout << "Deserialize: " << struct2.a << " " << struct2.b << " " << struct2.c << std::endl;
    if (struct1 == struct2) {
        std::cout << "[true]\n";
    } else {
        std::cout << "[false]\n";
    }

    // vector with MyStruct
    std::cout << "Test for serializing std::vector<MyStruct>: \n";
    std::vector<MyStruct> vs1 {struct1, struct2}, vs2;
    xml::serialize_xml(vs1, "std_vector", "vs.xml");
    xml::deserialize_xml(vs2, "std_vector", "vs.xml");
    std::cout << "Serialize:\n";
    for (auto &v : vs1) {
        std::cout << v.a << " " << v.b << " " << v.c << std::endl;
    }
    std::cout << "Deserialize:\n";
    for (auto &v : vs2) {
        std::cout << v.a << " " << v.b << " " << v.c << std::endl;
    }
    if (vs1 == vs2) {
        std::cout << "[true]\n";
    } else {
        std::cout << "[false]\n";
    }

    std::cout << "Test for serializing UserDefinedType: \n";
    UserDefinedType u1{1, "user1", {2.5, 3.41, 3.45}}, u2;
    xml::serialize_xml(u1, "UserDefinedType", "u.xml");
    xml::deserialize_xml(u2, "UserDefinedType", "u.xml");
    std::cout << "Serialize:\n";
    std::cout << u1.idx << " " << u1.name << " ";
    for (auto &v : u1.data) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    std::cout << "Deserialize:\n";
    std::cout << u2.idx << " " << u2.name << " ";
    for (auto &v : u2.data) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    if (u1 == u2) {
        std::cout << "[true]\n";
    } else {
        std::cout << "[false]\n";
    }

    std::cout << "Test for serializing std::unique_ptr<int>: \n";
    std::unique_ptr<int> up1(new int(1)), up2;
    xml::serialize_xml(up1, "std_unique_ptr", "up.xml");
    xml::deserialize_xml(up2, "std_unique_ptr", "up.xml");
    std::cout << "Serialize: " << *up1 << std::endl << "Deserialize: " << *up2 << std::endl;
    if (*up1 == *up2) {
        std::cout << "[true]\n";
    } else {
        std::cout << "[false]\n";
    }

    std::cout << "Test for serializing std::shared_ptr<int>: \n";
    std::unique_ptr<int> sh1(new int(1)), sh2;
    xml::serialize_xml(sh1, "std_shared_ptr", "up.xml");
    xml::deserialize_xml(sh2, "std_shared_ptr", "up.xml");
    std::cout << "Serialize: " << *sh1 << std::endl << "Deserialize: " << *sh2 << std::endl;
    if (*sh1 == *sh2) {
        std::cout << "[true]\n";
    } else {
        std::cout << "[false]\n";
    }

    std::cout << "Test for serializing std::vector<std::unique_ptr<double>>: \n";
    const int n = 5;
    std::vector<std::unique_ptr<double>> uv1(n), uv2;
    for (int i = 1; i <= n; i++) {
        uv1[i - 1] = std::make_unique<double>(i * 1.1);
    }
    xml::serialize_xml(uv1, "std_vector", "uv.xml");
    xml::deserialize_xml(uv2, "std_vector", "uv.xml");
    std::cout << "Serialize: ";
    for (auto &v : uv1) {
        std::cout << *v << " ";
    }
    std::cout << std::endl;
    std::cout << "Deserialize: ";
    for (auto &v : uv2) {
        std::cout << *v << " ";
    }
    std::cout << std::endl;
    int i;
    for (i = 0; i < n; i++) {
        if (*uv1[i] != *uv2[i]) {
            break;
        }
    }
    if (i == n) {
        std::cout << "[true]\n";
    } else {
        std::cout << "[false]\n";
    }

    std::cout << "Test for serializing std::unique_ptr<std::vector<int>>: \n";
    std::unique_ptr<std::vector<int>> univ1 = std::make_unique<std::vector<int>>(std::vector<int>{1, 2, 3, 4, 5});
    std::unique_ptr<std::vector<int>> univ2;
    xml::serialize_xml(univ1, "std_unique_ptr", "univ.xml");
    xml::deserialize_xml(univ2, "std_unique_ptr", "univ.xml");
    std::cout << "Serialize: ";
    for (auto &v : *univ1) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    std::cout << "Deserialize: ";
    for (auto &v : *univ2) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    if (*univ1 == *univ2) {
        std::cout << "[true]\n";
    } else {
        std::cout << "[false]\n";
    }
    return 0;
}