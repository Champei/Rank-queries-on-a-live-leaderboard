#pragma once
#include "src/structA/avl.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>
namespace leaderboard{
    // Sorted-array oracle. Library containers are permitted here, in tests.
    class Reference{
    public:
        void revise(std::int64_t id,std::int64_t score);
        void remove(std::int64_t id);
        std::size_t rank(std::int64_t id) const;
        Entry select(std::size_t pos) const;
        std::vector<Entry> positions(std::size_t first,std::size_t last) const;
        std::size_t size() const{
            return data_.size();
        }
    private:
        std::vector<Entry>data_;
    };
}
