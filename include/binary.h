#ifndef __BINARY_H_
#define __BINARY_H_

#include <fstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <tuple>

#include "helper.h"

namespace detail {

template <typename T>
typename std::enable_if<!is_pair<std::remove_reference_t<T>>::value &&
                        !is_tuple<std::remove_reference_t<T>>::value &&
                        !is_smart_ptr<std::remove_reference_t<T>>::value>::type
serialize_stl(T &&val, std::fstream &fs);

template <typename T>
typename std::enable_if<is_pair<std::remove_reference_t<T>>::value>::type
serialize_stl(T &&val, std::fstream &fs);

template <typename T>
typename std::enable_if<is_tuple<std::remove_reference_t<T>>::value>::type
serialize_stl(T &&val, std::fstream &fs);

template <typename T>
typename std::enable_if<is_smart_ptr<std::remove_reference_t<T>>::value>::type
serialize_stl(T &&val, std::fstream &fs);

template <typename T1, typename T2>
void deserialize_stl(std::pair<T1, T2> &val, std::fstream &fs);

template <typename T1, typename T2>
void deserialize_stl(std::map<T1, T2> &val, std::fstream &fs);

template <typename T>
void deserialize_stl(std::vector<T> &val, std::fstream &fs);

template <typename T>
void deserialize_stl(std::set<T> &val, std::fstream &fs);

template <typename T>
void deserialize_stl(std::list<T> &val, std::fstream &fs);

template <typename... Args>
void deserialize_stl(std::tuple<Args...> &val, std::fstream &fs);

template <typename... Args>
void deserialize_tuple(std::tuple<Args...> &tuple, std::fstream &fs);

template <typename T>
void deserialize_stl(std::unique_ptr<T> &val, std::fstream &fs);

template <typename T>
void deserialize_stl(std::shared_ptr<T> &val, std::fstream &fs);

}  // namespace detail

namespace binary {
    
template <typename T>
typename std::enable_if<std::is_arithmetic_v<std::remove_reference_t<T>>>::type
serialize_helper(T &&val, std::fstream &fs) {
    fs.write(reinterpret_cast<const char *>(&val), sizeof(T));
}

template <typename T>
typename std::enable_if<detail::stl_container<std::remove_reference_t<T>>::value>::type
serialize_helper(T &&val, std::fstream &fs) {
    detail::serialize_stl(std::forward<T>(val), fs);
}

template <typename T>
typename std::enable_if<std::is_same_v<std::remove_reference_t<T>, std::string>>::type
serialize_helper(T &&val, std::fstream &fs) {
    int len = val.length();
    fs.write(reinterpret_cast<const char *>(&len), sizeof(int));
    fs.write(val.c_str(), val.length());
}

template <typename T>
typename std::enable_if<!detail::is_not_user_type<std::remove_reference_t<T>>::value &&
                        detail::has_get_all_member<std::remove_reference_t<T>>::value>::type
serialize_helper(T &val, std::fstream &fs) {
    serialize_helper(val.get_all_member(), fs);
}

template <typename T>
typename std::enable_if<detail::is_not_user_type<std::remove_reference_t<T>>::value>::type
serialize(T &&val, std::string file_name) {
    std::fstream fs(file_name, std::ios_base::out | std::ios_base::binary);
    serialize_helper(std::forward<T>(val), fs);
    fs.close();
}

template <typename T>
typename std::enable_if<!detail::is_not_user_type<std::remove_reference_t<T>>::value &&
                        detail::has_get_all_member<std::remove_reference_t<T>>::value>::type
serialize(T &&val, std::string file_name) {
    std::fstream fs(file_name, std::ios_base::out | std::ios_base::binary);
    serialize_helper(val, fs);
    fs.close();
}

template <typename T>
typename std::enable_if<std::is_arithmetic_v<std::remove_reference_t<T>>>::type
deserialize_helper(T &val, std::fstream &fs) {
    fs.read(reinterpret_cast<char *>(&val), sizeof(T));
}

template <typename T>
typename std::enable_if<!detail::is_not_user_type<std::remove_reference_t<T>>::value &&
                        detail::has_get_all_member<std::remove_reference_t<T>>::value>::type
deserialize_helper(T &val, std::fstream &fs);

template <typename T>
typename std::enable_if<std::is_same_v<std::remove_reference_t<T>, std::string>>::type
deserialize_helper(T &val, std::fstream &fs) {
    int len;
    fs.read(reinterpret_cast<char *>(&len), sizeof(int));
    char s[len + 1];
    fs.read(s, len);
    s[len] = '\0';
    val = T(s);
}

template <typename T>
typename std::enable_if<detail::stl_container<std::remove_reference_t<T>>::value>::type
deserialize_helper(T &val, std::fstream &fs);

template <typename T>
typename std::enable_if<detail::stl_container<std::remove_reference_t<T>>::value>::type
deserialize_helper(T &val, std::fstream &fs) {
    detail::deserialize_stl(val, fs);
}

template <typename T>
typename std::enable_if<!detail::is_not_user_type<std::remove_reference_t<T>>::value &&
                        detail::has_get_all_member<std::remove_reference_t<T>>::value>::type
deserialize_helper(T &val, std::fstream &fs) {
    decltype(val.get_all_member()) tuple;
    deserialize_helper(tuple, fs);
    tuple_helper::construct_object(val, tuple);
}

template <typename T>
typename std::enable_if<detail::is_not_user_type<std::remove_reference_t<T>>::value>::type
deserialize(T &val, std::string file_name) {
    std::fstream fs(file_name, std::ios_base::in | std::ios_base::binary);
    deserialize_helper(val, fs);
    fs.close();
}

template <typename T>
typename std::enable_if<!detail::is_not_user_type<std::remove_reference_t<T>>::value &&
                        detail::has_get_all_member<std::remove_reference_t<T>>::value>::type
deserialize(T &val, std::string file_name) {
    std::fstream fs(file_name, std::ios_base::in | std::ios_base::binary);
    deserialize_helper(val, fs);
    fs.close();
}

} // namespace binary

namespace detail {

template <typename T>
typename std::enable_if<is_tuple<std::remove_reference_t<T>>::value>::type
serialize_stl(T &&val, std::fstream &fs) {
    tuple_helper::tuple_for_each([&fs](auto &&val) { binary::serialize_helper(std::forward<decltype(val)>(val), fs); },
                                 val);
}

template <typename T>
typename std::enable_if<!is_pair<std::remove_reference_t<T>>::value &&
                        !is_tuple<std::remove_reference_t<T>>::value &&
                        !is_smart_ptr<std::remove_reference_t<T>>::value>::type
serialize_stl(T &&val, std::fstream &fs) {
    int len = val.size();
    binary::serialize_helper(len, fs);
    for (auto &v : val) {
        binary::serialize_helper(v, fs);
    }
}

template <typename T>
typename std::enable_if<is_pair<std::remove_reference_t<T>>::value>::type
serialize_stl(T &&val, std::fstream &fs) {
    binary::serialize_helper(val.first, fs);
    binary::serialize_helper(val.second, fs);
}

template <typename T>
typename std::enable_if<is_smart_ptr<std::remove_reference_t<T>>::value>::type
serialize_stl(T &&val, std::fstream &fs) {
    binary::serialize_helper(*val, fs);
}

template <typename T1, typename T2>
void deserialize_stl(std::pair<T1, T2> &val, std::fstream &fs) {
    binary::deserialize_helper(val.first, fs);
    binary::deserialize_helper(val.second, fs);
}

template <typename T1, typename T2>
void deserialize_stl(std::map<T1, T2> &val, std::fstream &fs) {
    int size;
    binary::deserialize_helper(size, fs);
    for (int i = 0; i < size; i++) {
        std::pair<T1, T2> value;
        binary::deserialize_helper(value, fs);
        val.emplace(std::move(value));
    }
}

template <typename T>
void deserialize_stl(std::set<T> &val, std::fstream &fs) {
    int size;
    binary::deserialize_helper(size, fs);
    for (int i = 0; i < size; i++) {
        T value;
        binary::deserialize_helper(value, fs);
        val.emplace(std::move(value));
    }
}

template <typename T>
void deserialize_stl(std::vector<T> &val, std::fstream &fs) {
    int size;
    binary::deserialize_helper(size, fs);
    for (int i = 0; i < size; i++) {
        T value;
        binary::deserialize_helper(value, fs);
        val.emplace_back(std::move(value));
    }
}

template <typename T>
void deserialize_stl(std::list<T> &val, std::fstream &fs) {
    int size;
    binary::deserialize_helper(size, fs);
    for (int i = 0; i < size; i++) {
        T value;
        binary::deserialize_helper(value, fs);
        val.emplace_back(std::move(value));
    }
}

template <typename... Args>
void deserialize_stl(std::tuple<Args...> &val, std::fstream &fs) {
    deserialize_tuple(val, fs);
}

template <typename Tuple, size_t N>
struct deserialize_tuple_helper {
    static void deserialize_tuple(Tuple &tuple, std::fstream &fs) {
        binary::deserialize_helper(std::get<N>(tuple), fs);
    }
};


template <int... Index, typename... Args>
void deserialize_tuple_helper_func(tuple_helper::IndexTuple<Index...>, std::tuple<Args...> &tuple, std::fstream &fs) {
    // 函数实参计算顺序从左至右
    // return std::make_tuple(deserialize_tuple_helper<std::tuple<Args...>, Index>::deserialize_tuple(tuple, fs)...);
    int a[] = {(deserialize_tuple_helper<std::tuple<Args...>, Index>::deserialize_tuple(tuple, fs), 0)...};
}

template <typename... Args>
void deserialize_tuple(std::tuple<Args...> &tuple, std::fstream &fs) {
    using tuple_index = typename tuple_helper::MakeIndex<std::tuple_size_v<std::tuple<Args...>>>::tuple_index;
    deserialize_tuple_helper_func(tuple_index(), tuple, fs);
}

template <typename T>
void deserialize_stl(std::unique_ptr<T> &val, std::fstream &fs) {
    T value;
    binary::deserialize_helper(value, fs);
    val = std::make_unique<T>(std::move(value));
}

template <typename T>
void deserialize_stl(std::shared_ptr<T> &val, std::fstream &fs) {
    T value;
    binary::deserialize_helper(value, fs);
    val = std::make_shared<T>(std::move(value));
}

} // namespace detail
#endif