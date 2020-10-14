#pragma once

#include <yazik/concurrency/task.hpp>
#include <yazik/concurrency/channel.hpp>
#include <yazik/utility/result.hpp>

namespace yazik::rpc {

    enum class RpcStatusEnum {
        Ok,
        Cancelled,
        InvalidArgument,
        NotFound,
        NonAuthorized,
        ResourceExhausted,
        FailedPrecondition,
        DataCorrupted,
        Internal,
        OutOfRange,
        Unimplemented,
        Unexpected
    };

    class RpcStatus final : public detail::ResultErrorEnumBased<RpcStatusEnum> {
        using Super = detail::ResultErrorEnumBased<RpcStatusEnum>;
    public:
        using Super::Super;
        using Super::Status;

        string to_error_string() const {
            return do_format("rpc: status: {}", Super::to_error_string());
        }

        static inline RpcStatus ok() noexcept {
            return { Status::Ok, "Ok" };
        }

        inline bool is_ok() noexcept { return Super::_value == Status::Ok; }

        static inline RpcStatus cancelled() noexcept {
            return { Status::Cancelled, "cancelled" };
        }

        static inline RpcStatus invalid_argument(string error) noexcept {
            return { Status::InvalidArgument, std::move(error) };
        }

        static inline RpcStatus not_found(string error = "") noexcept {
            return { Status::NotFound, do_format("NotFound: {}", error) };
        }

        inline bool is_not_found() noexcept { return Super::_value == Status::NotFound; }

        inline static RpcStatus unauthorized(string error) noexcept {
            return { Status::NonAuthorized, std::move(error) };
        }

        inline static RpcStatus resource_exhausted(string error) noexcept {
            return { Status::ResourceExhausted, std::move(error) };
        }

        inline static RpcStatus failed_precondition(string error) noexcept {
            return { Status::FailedPrecondition, std::move(error) };
        }

        inline static RpcStatus data_corrupted(string error) noexcept {
            return {Status::DataCorrupted, std::move(error)};
        }

        inline bool is_data_corrupted() noexcept { return Super::_value == Status::DataCorrupted; }

        inline static RpcStatus internal(string error) noexcept {
            return {Status::Internal, std::move(error)};
        }

        inline static RpcStatus out_of_range(string error) noexcept {
            return {Status::OutOfRange, std::move(error)};
        }

        inline static RpcStatus unimplemenmted() noexcept {
            return {Status::Unimplemented, ""};
        }

        inline static RpcStatus unexpected(string error) noexcept {
            return {::yazik::detail::UnxepectedStringTag{}, std::move(error)};
        }

        template<typename T = void>
        Result<T, RpcStatus> as_result() const {
            return yaz_fail(*this);
        }

        template<typename T = void>
        Result<T, RpcStatus> as_broken() const {
            return yaz_fail(*this);
        }

        ::folly::Unexpected<RpcStatus> as_unexpected() const {
            return yaz_fail<RpcStatus>(*this);
        }

        template<typename T = void>
        Channel<T, RpcStatus> as_broken_channel() const {
            return { as_unexpected() };
        }

        template<typename T = void>
        Task<T, RpcStatus> as_broken_task() const {
            return yaz_fail_t<T, RpcStatus>(*this);
        }

    };

} // end of ::yazik::rpc namespace