
#pragma once

#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/util/json_util.h>
#include <folly/Format.h>

#include <yazik/utility/utility_defs.hpp>
#include <yazik/utility/lambda_continuation.hpp>

template <>
class ::folly::FormatValue<::google::protobuf::Message> {
    const ::google::protobuf::Message& _msg;
public:
    explicit FormatValue(const ::google::protobuf::Message& msg)
        : _msg { msg }
    {}

    template <class FormatCallback>
    void format(::folly::FormatArg& arg, FormatCallback& cb) const {
        std::string output;
        ::google::protobuf::util::MessageToJsonString(
            _msg,
            &output,
            yaz_with(options, ::google::protobuf::util::JsonPrintOptions{}) {
                options.add_whitespace = true;
            }
        );
        FormatValue<std::string> { std::move(output) }.format(arg, cb);
    }
};

namespace yazik {
    namespace proto_utils {

        inline bool is_equal(const ::google::protobuf::Message& message1, const ::google::protobuf::Message& message2 ) {
            return ::google::protobuf::util::MessageDifferencer::Equals(message1, message2);
        }

        Result<dynamic> message_to_json(const ::google::protobuf::Message& message);
        Result<void> message_from_json_impl(const dynamic& input, ::google::protobuf::Message& msg);

        template <typename Message>
        Result<Message> message_from_json(const dynamic& input) {
            auto message = Message{};
            co_await message_from_json_impl(input, message);
            co_return std::move(message);
        }


    } // end of proto_utils namespace

} // end of ::yazik namespace
