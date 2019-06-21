#pragma once

#include <utility>
#include <cctype>

namespace llparse::concept {

    template <class T>
    struct ContainSizeField {
        template<typename U> static auto test(int) -> decltype(std::declval<U>().size(), std::size_t());
        template<typename> static std::false_type test(...);
        static constexpr bool value = std::is_same<decltype(test<T>(0)), std::size_t>::value;
    };

    template <class T>
    struct CharPushable {
        template<typename U> static auto test(int) -> decltype(std::declval<U>().push_back('c'), std::true_type());
        template<typename> static std::false_type test(...);
        static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
    };

    template <class T>
    struct StrAppendable {
        template<typename U> static auto test(int) -> decltype(std::declval<U>().append("abc", 3), std::true_type());
        template<typename> static std::false_type text(...);
        static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
    };

    // 字符串容器
    template <class T>
    using StringContainer = std::enable_if<ContainSizeField<T>::value && CharPushable<T>::value && StrAppendable<T>::value, void>::type;

}
