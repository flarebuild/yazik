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

        ResultErrorEnumBased(ResultErrorEnumBased&& other)
        : _value { std::move(other._value) }
        , _message { std::move(other._message) }
        {}

         ResultErrorEnumBased(const ResultErrorEnumBased& other)
        : _value { other._value }
        , _message { other._message }
        {}

        ResultErrorEnumBased& operator= (ResultErrorEnumBased&& other) {
            if (this == &other) {
                return *this;
            }

            _value = std::move(other._value);
            _message = std::move(other._message);

            return *this;
        }

        ResultErrorEnumBased& operator= (const ResultErrorEnumBased& other) {
            if (this == &other) {
                return *this;
            }

            _value = other._value;
            _message = other._message;

            return *this;
        }


        TEnum value() const noexcept { return _value; }
        const string& to_error_string() const noexcept { return _message; }
    };

} //end of ::yazik::detail namespace