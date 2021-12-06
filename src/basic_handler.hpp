#ifndef VSPP_BASIC_HANDLER_H
#define VSPP_BASIC_HANDLER_H
#include <type_traits>
#include <utility>
#include <vector>

namespace vspp {
// handler - concept
template <class T>
struct is_handler {

    template <typename U> static auto test(int) -> typename std::enable_if<
            std::is_invocable<decltype(&U::on_entry), U&, std::pair<typename U::field_type, typename U::value_type>&&>::value, // u.on_entry(std::pair<field, value>&& entry)
            std::true_type
        >::type;
    template <typename> static auto test(...) -> std::false_type;
    static constexpr bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
};

template <class T>
class entry_type;

template <class F, class V>
class entry_type<std::pair<F, V>> {
public:
    using field_type = typename std::remove_cv<F>::type;
    using value_type = typename std::remove_cv<V>::type;
};

template <class C>
class container_handler {
public:
    using container_type = typename std::decay<C>::type;
    using field_type = typename entry_type<typename container_type::value_type>::field_type;
    using value_type = typename entry_type<typename container_type::value_type>::value_type;

    explicit container_handler(C& ctr)
    : ctr_(ctr) {}

    container_handler(const C& ch)
    : ctr_(ch.ctr_) {}
    
    void on_entry(std::pair<field_type, value_type>&& entry) {
        ctr_.insert(ctr_.end(), std::move(entry));
    }
private:
    C& ctr_;
};

}
#endif // VSPP_BASIC_HANDLER_H
