#include <yazik/utility/utility_defs.hpp>

namespace yazik::detail {

    template <typename TEnum>
    class ResultErrorEnumBased {
    protected:
        TEnum _value;
        string _message;

    public:
        using Status = TEnum;

        ResultErrorEnumBased() = default;

        ResultErrorEnumBased(
            detail::UnxepectedStringTag,
            string msg
        )
        : _value { TEnum::Unexpected }
        , _message { std::move(msg) }
        {}

        ResultErrorEnumBased(
            TEnum value,
            string msg
        )
        : _value { value }
        , _message { std::move(msg) }
        {}

        TEnum value() const noexcept { return _value; }
        const string& to_error_string() const noexcept { return _message; }
    };

} //end of ::yazik::detail namespace