#pragma once

#include <type_traits>
#include <variant>
#include <optional>

namespace std {
    template<typename Ta, typename Tb>
    std::string std::to_string(const std::pair<Ta, Tb>& p) {
        return "{" + std::to_string(p.first) + ", " + std::to_string(p.second) + "}";
    }
}

template<typename Targ, int n, typename T, typename... Ts>
consteval auto pack_id() {
    if constexpr (std::is_same<T, Targ>::value) {
        return n;
    } else {
        return pack_id<Targ, n + 1, Ts...>();
    }
}

template<int n, typename T, typename... Ts>
struct type_select {
    typedef type_select<n - 1, Ts...>::type type;
};

template<typename T, typename... Ts>
struct type_select<0, T, Ts...> {
    typedef T type;
};

template<typename... Ts>
consteval auto pack_size() {
    return sizeof...(Ts);
};

template<typename... Ts>
struct type_union {
    template<typename Targ, typename... Targs>
        requires (pack_size<Targs...>() > 0)
    bool is() const noexcept {
        if constexpr (pack_size<Targs...>() == 0) {
            return is<Targ>();
        } else {
            return is<Targ>() || is<Targs...>();
        }
    }
    template<typename Targ>
    bool is() const noexcept {
        if constexpr (is_non_empty) {
            return m_type == pack_id<Targ, 0, Ts...>();
        } else {
            return false;
        }
    }

    template<typename Targ>
    void cast_to() noexcept {
        if constexpr (is_non_empty) {
            m_type = pack_id<Targ, 0, Ts...>();
        }
    }

    template<typename Targ>
    void cast_to(const typename Targ::type& arg) noexcept {
        if constexpr (is_non_empty) {
            m_type = pack_id<Targ, 0, Ts...>();
            set<Targ>(arg);
        }
    }

    template<typename Targ>
    auto get() noexcept {
        if constexpr (is_non_empty) {
            if (is<Targ>()) {
                auto val = (std::get_if<pack_id<Targ, 0, Ts...>()>(&m_data));
                if (val != nullptr) {
                    return std::optional(val);
                }               
            }
        }
        return std::optional<typename Targ::type*>(std::nullopt);
    }

    template<typename Targ>
    auto get() const noexcept {
        if constexpr (is_non_empty) {
            if (is<Targ>()) {
                auto val = (std::get_if<pack_id<Targ, 0, Ts...>()>(&m_data));
                if (val != nullptr) {
                    return std::optional(val);
                }               
            }
        }
        return std::optional<const typename Targ::type*>(std::nullopt);
    }

    template<typename Targ>
    void set(const typename Targ::type& arg) {
        if constexpr (is_non_empty) {
            if (is<Targ>()) {
                m_data. template emplace<pack_id<Targ, 0, Ts...>()>(arg);
            }
        }
    }

    auto to_string() const {
        return to_string_of<0>();
    }
private:
    template<int n>
    auto to_string_of() const {
        if constexpr (is_non_empty) {
            std::string result = "";
            typedef typename type_select<n, Ts...>::type selected_type;

            if (auto ptr = get<selected_type>()) {
                result += selected_type::label;
                if constexpr (std::is_same<std::string, typename selected_type::type>::value) {
                    result += " := '" + **ptr + "'";
                } else {
                    result += " := '" + std::to_string(**ptr) + "'";
                }

                return result;
            }

            if constexpr (n < pack_size<Ts...>() - 1) {
                return to_string_of<n + 1>();
            } else {
                return result;
            }
        } else {
            return "";
        }
    }   

    int m_type = 0;
    constexpr static bool is_non_empty = pack_size<Ts...>() > 0;
    std::conditional<is_non_empty, std::variant<typename Ts::type...>, void*>::type m_data;
};
