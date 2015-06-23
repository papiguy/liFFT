#pragma once

namespace std {

    template< bool B, class T = void >
    using enable_if_t = typename enable_if<B,T>::type;

    template< bool B, class T, class U >
    using conditional_t = typename conditional< B, T, U >::type;

    template< class T >
    using result_of_t = typename result_of<T>::type;

    template< class T >
    using decay_t = typename decay<T>::type;

    template< class T >
    using remove_pointer_t = typename remove_pointer<T>::type;

    template< class T >
    using make_unsigned_t = typename make_unsigned<T>::type;

    template< typename T >
    struct integral_type
    {
        using type = T;
    };

}  // namespace std
