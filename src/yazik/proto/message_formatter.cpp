#include "message_formatter.hpp"

namespace yazik::proto_utils {
    Result<dynamic> message_to_json(const ::google::protobuf::Message& message) {
        std::string output;
        ::google::protobuf::util::MessageToJsonString(
            message,
            &output,
            {}
        );
        co_return ::folly::parseJson(std::move(output));
    }

    Result<void> message_from_json_impl(const dynamic& input, ::google::protobuf::Message& msg) {
        auto sts = ::google::protobuf::util::JsonStringToMessage(
            ::folly::toJson(input),
            &msg,
            {}
        );
        if (!sts.ok()) co_await yaz_fail<string>(sts.error_message().ToString() );
        co_return;
    }
} // end of ::yazik::proto_utils namespace