#pragma once

#include <yazik/utility/macro.hpp>
#include <yazik/concurrency/baton.hpp>

#include <atomic>
#include <stack>

namespace yazik::concurrency {

    template<typename Fn, typename T>
    concept c_stack_sweep_fn = requires(Fn&& fn, T&& value) { fn(std::move(value)); };

    template<class T>
    class Stack {
        struct Node {
            T data;
            Node* next;
            Node(T&& data)
            : data{ std::move(data) }
            , next{ nullptr }
            {}
        };
        std::atomic<Node*> _head{nullptr};

        inline Node* steel_head() noexcept {
            Baton b;
            auto cur_head = _head.load(std::memory_order_relaxed);
            Node* to_set = nullptr;
            while(!_head.compare_exchange_weak(
                cur_head,
                to_set ,
                std::memory_order_release,
                std::memory_order_relaxed
            )) b.increment_and_check();
            return cur_head;
        }

    public:

        Stack() noexcept = default;
        Stack(std::stack<T>&& data) noexcept {
            while(!data.empty()) {
                push(std::move(data.top()));
                data.pop();
            }
        }

        ~Stack() noexcept {
            auto cur_head = _head.load();
            while (cur_head) {
                const auto prev_head = cur_head;
                cur_head = cur_head->next;
                delete prev_head;
            }
        }

        template<c_stack_sweep_fn<T> Fn>
        bool sweep(Fn&& fn) noexcept {
            auto cur = steel_head();
            if (cur == nullptr) return false;
            while (cur) {
                Node* next = cur->next;
                fn(std::move(cur->data));
                delete cur;
                cur = next;
            }
            return true;
        }

        template<c_stack_sweep_fn<T> Fn>
        bool sweep_ordered(Fn&& fn) noexcept {
            auto cur = steel_head();
            if (cur == nullptr) return false;
            Node *next, *prev = nullptr;
            while (cur) {
                next = cur->next;
                cur->next = prev;
                prev = cur;
                cur = next;
            }
            cur = prev;
            while(cur) {
                next = cur->next;
                fn(std::move(cur->data));
                delete cur;
                cur = next;
            }
            return true;
        }

        std::stack<T> steal() noexcept {
            auto cur_head = steel_head();

            std::stack<T> result;
            while(cur_head) {
                result.push(std::move(cur_head->data));
                const auto prev_head = cur_head;
                cur_head = cur_head->next;
                delete prev_head;
            };
            return result;
        }

        bool push(T&& data) noexcept {
            const auto new_node = new Node(std::move(data));

            // put the current value of head into new_node->next
            new_node->next = _head.load(std::memory_order_relaxed);

            Baton b;

            // now make new_node the new head, but if the head
            // is no longer what's stored in new_node->next
            // (some other thread must have inserted a node just now)
            // then put that new head into new_node->next and try again
            while(!_head.compare_exchange_weak(
                new_node->next,
                new_node,
                std::memory_order_release,
                std::memory_order_relaxed
            )) b.increment_and_check();

            return new_node->next == nullptr;
        }

        inline bool push(const T& data) noexcept {
            T data_copy{data};
            return push(std::move(data_copy));
        }

        bool empty() { return _head.load(std::memory_order_relaxed) == nullptr; }

    };

} // end of ::yazik::concurrency namespace