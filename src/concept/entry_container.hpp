#pragma once

namespace llparser::concept {
    template <class T, class V>
	struct EntryInsertable {
		template<typename U> static auto test(int) -> decltype(std::declval<U>().insert(std::move(std::declval<V>())), std::true_type());
		template<typename> static std::false_type test(...);
		static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
	};
	template <class T, class V>
	struct EntryPushable {
		template<typename U> static auto test(int) -> decltype(std::declval<U>().push_back(std::move(std::declval<V>())), std::true_type());
		template<typename> static std::false_type test(...);
		static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
	};

    template <class T, class V>
    using EntryContainer = std::enable_if<EntryInsertable<T, V>::value || EntryPushable<T, V>::value, void>::type;
}