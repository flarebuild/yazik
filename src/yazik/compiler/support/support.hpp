#pragma once

#include <range/v3/view.hpp>
#include <string_view>
#include <yazik/utility/concepts.hpp>
#include <yazik/utility/utility_defs.hpp>
#include <folly/FBVector.h>
#include <folly/Function.h>
#include <folly/Unit.h>
#include <boost/noncopyable.hpp>
#include <optional>

namespace yazik::compiler::support {

    class Initializer {
        template <class Builder, class ArgPack, std::size_t... I>
        static inline Builder create_builder_detail(ArgPack&& args, std::index_sequence<I...> ) noexcept {
            return Builder { std::get<I>(std::forward<ArgPack>(args))... };
        }

    public:
        template<typename Enum>
        static inline Enum create_enum(int value) {
            return { value };
        }

        template<typename Entity>
        static inline Entity create_entity(
            void* ptr,
            const typename Entity::vtable_t* vtable
        ) noexcept {
            return { ptr, vtable };
        }

        template<typename Builder, typename... Args>
        static inline Builder create_builder(Args... args) noexcept {
            return { std::forward<Args>(args)... };
        }

    };

    template <typename T>
    using repeated_type_t = ::ranges::any_view<
        T, ::ranges::category::random_access | ::ranges::category::sized
    >;


    template<typename T, typename Parent>
    class VecLayeredBuilder;

    template<typename T>
    class VecBuilder: protected Initializer {
    public:
        using push_clbk_t = void (*)(void*, T);
        using value_t = T;
    protected:
        void* _ptr;
        push_clbk_t _pusher;

        VecBuilder(void* ptr, push_clbk_t pusher)
        : _ptr { ptr }
        , _pusher { std::move(pusher) }
        {}

        friend class Initializer;

    public:

        inline void push_back(T value) {
            _pusher(_ptr, std::move(value));
        }

        template<typename Parent>
        VecLayeredBuilder<T, Parent> layered(Parent* parent);
    };

    template<typename T, typename Parent>
    class VecLayeredBuilder final: public VecBuilder<T>  {
        using Base = VecBuilder<T>;
        using Self = VecLayeredBuilder<T, Parent>;

        Parent _parent;

        VecLayeredBuilder(
            void* ptr,
            typename Base::push_clbk_t pusher,
            Parent* parent
        )
        : Base { ptr, pusher }
        , _parent { std::move(*parent) }
        {}

        friend class Initializer;

    public:

        inline Self& push(T value) {
            Base::push_back(std::move(value));
            return *this;
        }

        inline Parent done() {
            return std::move(this->_parent);
        }
    };

    template<typename T>
    template<typename Parent>
    VecLayeredBuilder<T, Parent> VecBuilder<T>::layered(Parent* parent) {
        using Layered = VecLayeredBuilder<T, Parent>;
        return this->template create_builder<Layered>(
            this->_ptr,
            this->_pusher,
            parent
        );
    }

    template<template<class> typename Builder, typename Parent>
    class VecEntityLayeredBuilder;

    template<typename Builder>
    class VecEntityBuilder: protected Initializer {
        using Vtable = typename Builder::vtable_t;
    public:
        using builder_factory_t = void* (*)(void*);
    protected:
        void* _ptr;
        builder_factory_t _builder_factory;
        const Vtable* _vtable;

        VecEntityBuilder(
            void* ptr,
            builder_factory_t factory,
            const Vtable* vtable
        )
        : _ptr { ptr }
        , _builder_factory { std::move(factory) }
        , _vtable { vtable }
        {}

        friend class Initializer;

    public:

        inline void push_back(::folly::Function<void(Builder&)> clbk) {
            auto builder = this->create_builder<Builder>(
                this->_builder_factory(this->_ptr),
                _vtable
            );
            clbk(builder);
        }

        template<template<class> typename Layered, typename Parent>
        VecEntityLayeredBuilder<Layered, Parent> layered(Parent* parent);
    };

    template<template<class> typename Builder, typename Parent>
    class VecEntityLayeredBuilder final
    : public VecEntityBuilder<
        typename Builder<VecEntityLayeredBuilder<Builder, Parent>>::base_builder_t
    > {
        using Self = VecEntityLayeredBuilder<Builder, Parent>;
        using BaseBuilder = typename Builder<VecEntityLayeredBuilder<Builder, Parent>>::base_builder_t;
        using Base = VecEntityBuilder<BaseBuilder>;

        Parent* _parent;

    public:

        VecEntityLayeredBuilder(
            void* ptr,
            typename Base::builder_factory_t factory,
            const typename BaseBuilder::vtable_t* vtable,
            Parent* parent
        )
        : Base { ptr, factory, vtable }
        , _parent { parent }
        {}

        friend class Initializer;

        inline Builder<Self> push() {
            return Base::template create_builder<Builder<Self>>(
                this->_builder_factory(this->_ptr),
                this->_vtable,
                this
            );
        }

        inline Parent done() {
            return std::move(*this->_parent);
        }

    };

    template<typename Builder>
    template<template<class> typename Layered, typename Parent>
    VecEntityLayeredBuilder<Layered, Parent>
    VecEntityBuilder<Builder>::layered(Parent* parent) {
        return this->template create_builder<VecEntityLayeredBuilder<Layered, Parent>>(
            this->_ptr,
            this->_builder_factory,
            this->_vtable,
            parent
        );
    }

} //end of yazik::compiler::support namespace