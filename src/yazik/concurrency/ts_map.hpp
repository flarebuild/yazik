#pragma once

#include <optional>
#include <vector>
#include <unordered_map>
#include <map>
#include <shared_mutex>
#include <mutex>

#include <yazik/utility/macro.hpp>
namespace yazik {
namespace concurrency {

    template <typename Key, typename Value, template <typename...> typename Map, bool IsMulti>
    class TSMapBase {
        Map<Key,Value> _map;
        mutable std::shared_mutex _sh_mutex;

    public:

        void clear() {
            std::unique_lock lock(_sh_mutex);
            _map.clear();
        }

        void clear(const Key& key) {
            std::unique_lock lock(_sh_mutex);
            _map.erase(key);
        }

        void set(const Key& key, const Value& value) {
            std::unique_lock lock(_sh_mutex);
            _map.insert({key, value});
        }

        void add_bunch(Map<Key,Value>&& values) {
            std::unique_lock lock(_sh_mutex);
            _map.insert(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end()));
        }

        auto get(const Key& key) const {
            std::shared_lock lock(_sh_mutex);
            if constexpr (IsMulti) {
                vector<Value> result;
                auto it_range = _map.equal_range(key);
                for (auto it = it_range.first; it != it_range.second; ++it)
                    result.push_back(it->second);
                return std::move(result);
            } else {
                auto it = _map.find(key);
                if (it == _map.end())
                    return std::optional<Value>{};
                return std::optional<Value>{ it->second };
            }
        }

        bool contains(const Key& key) const {
            std::shared_lock lock(_sh_mutex);
            return _map.find(key) != _map.end();
        }

    };

    template <typename Key, typename Value>
    using TSUnorderedMap = TSMapBase<Key, Value, std::unordered_map, false>;

    template <typename Key, typename Value>
    using TSUnorderedMultiMap = TSMapBase<Key, Value, std::unordered_multimap, true>;

    template <typename Key, typename Value>
    using TSMap = TSMapBase<Key, Value, std::map, false>;

    template <typename Key, typename Value>
    using TSMultiMap = TSMapBase<Key, Value, std::multimap, true>;

} // end of ::yazik::concurrency namespace
} // end of ::yazik namespace