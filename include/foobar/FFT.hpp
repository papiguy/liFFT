#pragma once

#include "foobar/FFT_Properties.hpp"

// Included for convenience, so only one include is required from user code
#include "foobar/FFT_Definition.hpp"
#include "foobar/FFT_DataWrapper.hpp"
#include <boost/mpl/apply.hpp>

namespace bmpl = boost::mpl;

namespace foobar {

    template< class T_Input, class T_Output >
    class FFT_Interface
    {
    public:
        virtual ~FFT_Interface(){};

        virtual void operator()(T_Input& input, T_Output& output) = 0;
        virtual void operator()(T_Input& inout) = 0;
    };

    /**
     * Assembles an FFT
     *
     * Usage:
     *      1) The constructor takes the container(s) and may modify the memory
     *         Note: An implementation may not need the memory at all
     *      2) Execute the FFT with <fftInstance>(input, output), which performs the transform
     *         from the memories in the wrappers. If the base accessors of the wrappers do not return a reference type
     *         internal memory is allocated and data is copied before/after the FFT
     *
     * Parameters:
     * \tparam T_Library FFT Library to use
     * \tparam T_InputWrapper   Input wrapped in a FFT_DataWrapper
     * \tparam T_OutputWrapper  Output wrapped in a FFT_DataWrapper
     * \tparam T_constructWithReadOnly If true, the data passed in the constructor is not overwritten. Use false for better performance
     */
    template<
            class T_Library,
            typename T_InputWrapper,
            typename T_OutputWrapper,
            bool T_constructWithReadOnly = true
            >
    class FFT: public FFT_Interface< T_InputWrapper, T_OutputWrapper >
    {
        using Library = T_Library;
        using Input = T_InputWrapper;
        using Output = T_OutputWrapper;
        static constexpr bool constructWithReadOnly = T_constructWithReadOnly;

        using FFT_Def = typename Input::FFT_Def;
        static_assert(std::is_same< FFT_Def, typename Output::FFT_Def>::value, "FFT types of input and output differs");
        using FFT_Properties = detail::FFT_Properties< FFT_Def, Input, Output, constructWithReadOnly >;
        using ActLibrary = typename bmpl::apply< Library, FFT_Properties >::type;
        static constexpr bool isInplace = FFT_Properties::isInplace;

        ActLibrary lib_;
    public:
        explicit FFT(Input& input, Output& output): lib_(input, output)
        {
            static_assert(!isInplace, "Must not be called for inplace transforms");
        }

        explicit FFT(Input& inOut): lib_(inOut)
        {
            static_assert(isInplace, "Must not be called for out-of-place transforms");
        }

        FFT(FFT&& obj): lib_(std::move(obj.lib_)){}

        void operator()(Input& input, Output& output)
        {
            if(isInplace)
                throw std::logic_error("Must not be called for inplace transforms");
            input.preProcess();
            lib_(input, output);
            output.postProcess();
        }

        void operator()(Input& inout)
        {
            if(!isInplace)
                throw std::logic_error("Must not be called for out-of-place transforms");
            inout.preProcess();
            lib_(inout);
            inout.postProcess();
        }
    };

    template<
        class T_Library,
        bool T_constructWithReadOnly = true,
        typename T_InputWrapper,
        typename T_OutputWrapper
        >
    FFT< T_Library, std::decay_t<T_InputWrapper>, std::decay_t<T_OutputWrapper>, T_constructWithReadOnly >
    makeFFT(T_InputWrapper&& input, T_OutputWrapper&& output)
    {
        return FFT< T_Library, std::decay_t<T_InputWrapper>, std::decay_t<T_OutputWrapper>, T_constructWithReadOnly >(input, output);
    }

}  // namespace foobar
