#pragma once

#include <range/v3/view.hpp>
#include <string_view>
#include <yazik/utility/concepts.hpp>
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
            const typename Entity::Vtable* vtable
        ) noexcept {
            return { ptr, vtable };
        }

        template<typename Builder, typename ArgPack>
        static inline Builder create_builder(ArgPack&& pack) noexcept {
            return create_builder_detail<Builder>(
                std::forward<ArgPack>(pack),
                std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<ArgPack>>>{}
            );
        }

        template<typename Builder, typename ArgPack, typename Parent>
        static inline Builder create_builder_with_parent(ArgPack&& pack, Parent* parent) noexcept {
            auto repack = std::tuple_cat(std::forward<ArgPack>(pack), std::make_tuple(parent));
            auto result = create_builder<Builder>(std::move(repack));
            return result;
        }

    };

    template<typename T>
    class VecBuilder {
    public:
        using push_clbk_t = void (*)(void*, T);
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

    template<
        typename Builder,
        typename Vtable = typename Builder::Vtable
    > class VecEntityBuilder: protected Initializer {
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
            auto builder = this->create_builder<Builder>(std::make_tuple(
                this->_builder_factory(this->_ptr),
                _vtable
            ));
            clbk(builder);
        }
    };

    template<
        template <class> typename Builder,
        typename BaseBuilder,
        typename Parent
    > class VecEntityLayeredBuilder final
    : public VecEntityBuilder<BaseBuilder, typename BaseBuilder::Vtable> {
        using Self = VecEntityLayeredBuilder<Builder, BaseBuilder, Parent>;
        using LayeredBuilder = Builder<Self>;
        using Base = VecEntityBuilder<BaseBuilder, typename BaseBuilder::Vtable>;

        Parent _parent;

    public:

        VecEntityLayeredBuilder(
            void* ptr,
            typename Base::builder_factory_t factory,
            const typename BaseBuilder::Vtable* vtable,
            Parent* parent
        )
        : Base { ptr, factory, vtable }
        , _parent { std::move(*parent) }
        {}

        friend class Initializer;


        inline LayeredBuilder push() {
            return Base::template create_builder_with_parent<LayeredBuilder>(
                std::make_tuple(
                    this->_builder_factory(this->_ptr),
                    this->_vtable
                ),
                this
            );
        }

        inline Parent done() {
            return std::move(this->_parent);
        }

    };

} //end of yazik::compiler::support namespace