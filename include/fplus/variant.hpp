// elm architecture

#include <iostream>
#include <memory>
#include <tuple>

namespace fplus
{

namespace internal
{

    // http://stackoverflow.com/a/18987405/1866775

    template <typename...>
    struct is_one_of;

    template <typename F>
    struct is_one_of<F>
    {
        static constexpr bool value = false;
    };

    template <typename F, typename S, typename... T>
    struct is_one_of<F, S, T...>
    {
        static constexpr bool value = std::is_same<F, S>::value
            || is_one_of<F, T...>::value;
    };

    template <typename F, typename... T>
    struct is_one_of<F, std::tuple<T...>>
    {
        static constexpr bool value = is_one_of<F, T...>::value;
    };

    template <typename...>
    struct is_unique;

    template <>
    struct is_unique<> {
        static constexpr bool value = true;
    };

    template<typename F, typename... T>
    struct is_unique<F, T...>
    {
        static constexpr bool value = is_unique<T...>::value
            && !is_one_of<F, T...>::value;
    };

    template<typename... T>
    struct is_unique<std::tuple<T...>>
    {
        static constexpr bool value = is_unique<T...>::value;
    };

    template <typename...>
    struct are_same;

    template <>
    struct are_same<> {
        static constexpr bool value = true;
    };

    template<typename F1, typename F2>
    struct are_same<F1, F2>
    {
        static constexpr bool value = std::is_same<F1, F2>::value;
    };

    template<typename F1, typename F2, typename... T>
    struct are_same<F1, F2, T...>
    {
        static constexpr bool value = are_same<F2, T...>::value
            && std::is_same<F1, F2>::value;
    };

    template<typename... T>
    struct are_same<std::tuple<T...>>
    {
        static constexpr bool value = are_same<T...>::value;
    };

    // http://stackoverflow.com/a/3273571/1866775
    template<template<typename...> class List,
        template<typename> class Mod,
        typename ...Args>
    struct transform_parameter_pack {
        typedef List<typename Mod<Args>::type...> type;
    };

    template<typename T>
    struct as_shared_pointer
    {
        typedef std::shared_ptr<T> type;
    };



    // http://stackoverflow.com/a/27588263/1866775

    template <typename T, typename... Ts> struct get_index;

    template <typename T, typename... Ts>
    struct get_index<T, T, Ts...> : std::integral_constant<std::size_t, 0> {};

    template <typename T, typename Tail, typename... Ts>
    struct get_index<T, Tail, Ts...> :
        std::integral_constant<std::size_t, 1 + get_index<T, Ts...>::value> {};

    template <typename T>
    struct get_index<T>
    {
        // condition is always false, but should be dependant of T
        static_assert(sizeof(T) == 0, "element not found");
    };


    template <typename T, typename ... Ts>
    struct parameter_pack_head
    {
        typedef T type;
    };


    template <typename F>
    struct function_first_input_type
    {
        typedef typename std::remove_const<
            typename std::remove_reference<
                typename utils::function_traits<
                    F>::template arg<0>::type>::type>::type
            type;
    };

    template <typename F>
    struct unary_function_result_type
    {
        static_assert(utils::function_traits<F>::arity == 1,
                "Wrong arity.");
        typedef typename function_first_input_type<F>::type T;
        typedef std::decay_t<internal::invoke_result_t<F, T>> type;
    };


    // http://stackoverflow.com/a/42493805/1866775

    template <typename T>
    struct tag { };

    template <typename... Ts>
    struct type_set_eq_helper: tag<Ts>... { };

    template <typename, typename, typename = void>
    struct type_set_eq: std::false_type { };

    template <bool...>
    struct bool_pack { };

    template <bool... Bs>
    using my_and = std::is_same<bool_pack<Bs..., true>, bool_pack<true, Bs...>>;

    template <typename... Ts1, typename... Ts2>
    struct type_set_eq<std::tuple<Ts1...>, std::tuple<Ts2...>, typename std::enable_if< (sizeof...(Ts1) == sizeof...(Ts2)) && my_and< std::is_base_of<tag<Ts2>, type_set_eq_helper<Ts1...>>::value...  >::value  >::type  >:
       std::true_type { };


    // http://stackoverflow.com/a/42581257/1866775

    template <typename Lhs, typename Rhs>
    struct is_superset_of;

    template <typename Tuple, typename T, typename... Ts>
    struct is_superset_of<Tuple, std::tuple<T, Ts...>>
    {
        static const bool value = is_one_of<T, Tuple>::value && is_superset_of<Tuple, std::tuple<Ts...>>::value;
    };

    template <typename Tuple>
    struct is_superset_of<Tuple, std::tuple<>>
    {
        static const bool value = true;
    };


    // http://stackoverflow.com/a/36934374/1866775
    template<bool... bs>
    using all_true = std::is_same<bool_pack<bs..., true>, bool_pack<true, bs...>>;

} // namespace internal


template<typename ... Types>
struct variant
{
    static_assert(internal::is_unique<Types...>::value, "Types must be unique.");
    static_assert(internal::all_true<(!std::is_reference<Types>::value)...>::value, "No reference types allowed.");
    static_assert(internal::all_true<(!std::is_const<Types>::value)...>::value, "No const types allowed.");
    static_assert(sizeof...(Types) >= 1, "Please provide at least one type.");

    template <typename T>
    variant(const T& val) : shared_ptrs_({})
    {
        std::get<internal::get_index<T, Types...>::value>(shared_ptrs_) =
            std::make_shared<T>(val);
    }

    template <typename T>
    bool is() const
    {
        static_assert(
            internal::is_one_of<T, Types...>::value
            , "Type must match one possible variant type.");

        const auto ptr =
            std::get<internal::get_index<T, Types...>::value>(shared_ptrs_);

        return static_cast<bool>(ptr);
    }

    friend bool operator== (
        const variant<Types...>& a, const variant<Types...>& b)
    {
        return a.shared_ptrs_ == b.shared_ptrs_;
    }

    friend bool operator!= (
        const variant<Types...>& a, const variant<Types...>& b)
    {
        return a.shared_ptrs_ != b.shared_ptrs_;
    }

    template <typename F>
    auto visit_one(F f) const
    {
        using T = typename internal::function_first_input_type<F>::type;
        using Ret = internal::invoke_result_t<F, T>;
        internal::trigger_static_asserts<internal::unary_function_tag, F, T>();

        static_assert(
            internal::is_one_of<
                typename internal::function_first_input_type<F>::type,
                Types...>::value
            , "Function input must match one variant type.");

        static_assert(!std::is_same<std::decay_t<Ret>, void>::value,
                      "Function must return non-void type.");

        const auto ptr =
            std::get<internal::get_index<T, Types...>::value>(shared_ptrs_);

        if (ptr)
        {
            return just(internal::invoke(f, *ptr));
        }

        return nothing<std::decay_t<Ret>>();
    }

    template <typename ...Fs>
    auto visit(Fs ... fs) const ->
        typename internal::unary_function_result_type<
            typename internal::parameter_pack_head<Fs...>::type>::type
    {
        typedef typename internal::unary_function_result_type<
            typename internal::parameter_pack_head<Fs...>::type>::type
            Res;

        static_assert(
            sizeof...(Fs) >= std::tuple_size<shared_ptr_pack>::value,
            "Too few functions provided.");

        static_assert(
            sizeof...(Fs) <= std::tuple_size<shared_ptr_pack>::value,
            "Too many functions provided.");

        typedef typename internal::transform_parameter_pack<
            std::tuple,
            internal::unary_function_result_type,
            Fs...
            >::type return_types_tuple;

        typedef typename internal::transform_parameter_pack<
            std::tuple,
            internal::function_first_input_type,
            Fs...
            >::type function_first_input_types_tuple;

        static_assert(
            internal::is_unique<function_first_input_types_tuple>::value,
            "Only one function per input type allowed.");

        static_assert(
            internal::are_same<return_types_tuple>::value,
            "All Functions must return the same type.");

        static_assert(
            internal::type_set_eq<function_first_input_types_tuple, std::tuple<Types...>>::value,
            "Functions do not cover all possible types.");

        const auto results = justs(visit_helper<Res>(fs...));
        assert(size_of_cont(results) == 1);
        return head(results);
    }

    template <typename ...Fs>
    variant<Types...> transform(Fs ... fs) const
    {
        static_assert(
            sizeof...(Fs) >= std::tuple_size<shared_ptr_pack>::value,
            "Too few functions provided.");

        static_assert(
            sizeof...(Fs) <= std::tuple_size<shared_ptr_pack>::value,
            "Too many functions provided.");

        typedef typename internal::transform_parameter_pack<
            std::tuple,
            internal::unary_function_result_type,
            Fs...
            >::type return_types_tuple;

        typedef typename internal::transform_parameter_pack<
            std::tuple,
            internal::function_first_input_type,
            Fs...
            >::type function_first_input_types_tuple;

        static_assert(
            internal::type_set_eq<function_first_input_types_tuple, std::tuple<Types...>>::value,
            "Functions do not cover all possible types.");

        static_assert(
            internal::is_superset_of<std::tuple<Types...>, return_types_tuple>::value,
            "All Functions must return a possible variant type.");

        return visit(fs...);
    }

private:
    template <typename Res, typename F>
    std::vector<fplus::maybe<Res>> visit_helper(F f) const
    {
        return {visit_one(f)};
    }

    template <typename Res, typename F, typename ...Fs>
    std::vector<fplus::maybe<Res>> visit_helper(F f, Fs ... fs) const
    {
        return fplus::append(visit_helper<Res>(f), visit_helper<Res>(fs...));
    }

    typedef typename internal::transform_parameter_pack<
        std::tuple,
        internal::as_shared_pointer,
        Types...
    >::type shared_ptr_pack;
    shared_ptr_pack shared_ptrs_;
};

} // namespace fplus
