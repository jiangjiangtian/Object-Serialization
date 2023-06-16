/**
 * helper.h - the helper methods of serialization and deserialization, like type traits and
 * tuple helper methods
 */

#ifndef __HELPER_H_
#define __HELPER_H_

#include <fstream>
#include <utility>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <tuple>
#include <memory>

namespace detail {  // support string type and STL containers
template <typename T>
struct stl_container : std::false_type {};

template <typename T>
struct stl_container<std::vector<T>> : std::true_type {};

template <typename T>
struct stl_container<std::list<T>> : std::true_type {};

template <typename T1, typename T2>
struct stl_container<std::pair<T1, T2>> : std::true_type {
    using pair = typename std::pair<T1, T2>;
};

template <typename T1, typename T2>
struct stl_container<std::map<T1, T2>> : std::true_type {};

template <typename T>
struct stl_container<std::set<T>> : std::true_type {};

template <typename... Args>
struct stl_container<std::tuple<Args...>> : std::true_type {
    using tuple = std::tuple<Args...>;
};

template <typename T>
struct stl_container<std::shared_ptr<T>> : std::true_type {
    using pointer = std::shared_ptr<T>;
};

template <typename T>
struct stl_container<std::unique_ptr<T>> : std::true_type {
    using pointer = std::unique_ptr<T>;
};

template <typename T, typename = void>
struct is_pair : std::false_type {};

template <typename T>
struct is_pair<T, std::void_t<typename stl_container<T>::pair>> : std::true_type {};

template <typename T, typename = void>
struct is_tuple : std::false_type {};

template <typename T>
struct is_tuple<T, std::void_t<typename stl_container<T>::tuple>> : std::true_type {};

template <typename T, typename = void>
struct is_smart_ptr : std::false_type {};

template <typename T>
struct is_smart_ptr<T, std::void_t<typename stl_container<T>::pointer>> : std::true_type {};

template <typename T, typename = void>
struct is_not_user_type : std::false_type {};

template <typename T>
struct is_not_user_type<T, typename std::enable_if<stl_container<T>::value ||
                                                   std::is_arithmetic_v<T> ||
                                                   std::is_same_v<T, std::string>>::type>
        : std::true_type {};

template <typename T, typename = void>
struct has_get_all_member : std::false_type {};

template <typename T>
struct has_get_all_member<T,
    std::void_t<typename std::enable_if<is_tuple<decltype(std::declval<T>().get_all_member())>::value>::type>>
        : std::true_type {};

} // namespace detail

namespace tuple_helper {

template <int... Index>
struct IndexTuple {};

template <int N, int... Index>
struct MakeIndex;

template <int N, int... Index>
struct MakeIndex : MakeIndex<N - 1, N - 1, Index...> {};

template <int ...Index>
struct MakeIndex<0, Index...> {
    using tuple_index = IndexTuple<Index...>;
};

template <typename Func, typename Last>
void for_each_impl(Func &&f, Last &&val) {
    f(std::forward<Last>(val));
}

template <typename Func, typename First, typename... Args>
void for_each_impl(Func &&f, First &&val, Args&&... args) {
    f(std::forward<First>(val));
    for_each_impl(std::forward<Func>(f), std::forward<Args>(args)...);
}

template <typename Func, int... Index, typename... Args>
void tuple_for_each_helper(Func &&f, IndexTuple<Index...>, std::tuple<Args...> &&t) {
    for_each_impl(std::forward<Func>(f), std::forward<Args>(std::get<Index>(t))...);
}

template <typename Func, typename Tuple>
void tuple_for_each(Func &&f, Tuple &&tup) {
    using tuple_index = typename MakeIndex<std::tuple_size_v<std::remove_reference_t<Tuple>>>::tuple_index;
    tuple_for_each_helper(std::forward<Func>(f), tuple_index(), std::forward<Tuple>(tup));
}

template <typename Func, typename Tuple>
void tuple_for_each(Func &&f, Tuple &tup) {
    using tuple_index = typename MakeIndex<std::tuple_size_v<std::remove_reference_t<Tuple>>>::tuple_index;
    tuple_for_each_helper(std::forward<Func>(f), tuple_index(), std::forward<Tuple>(tup));
}

template <typename T, int... Index, typename... Args>
void construct_object_helper(T &val, IndexTuple<Index...>, std::tuple<Args...> &tup) {
    val = T(std::get<Index>(tup)...);
}

template <typename T, typename... Args>
void construct_object(T &val, std::tuple<Args...> &tup) {
    using tuple_index = typename MakeIndex<std::tuple_size_v<std::tuple<Args...>>>::tuple_index;
    construct_object_helper(val, tuple_index(), tup);
}

} // namespace tuple_helper

#endif