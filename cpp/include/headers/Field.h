#pragma once
#include "Constants.h"
#include <string>
#include <vector>

class Field {
public:
    using Index = std::pair<uint32_t, uint32_t>;

    Field(size_t size, uint32_t holl_num);

    Field(const Field &);

    Index shoot_point(Index, uint32_t, bool &) const;

    Index go_point(Index, uint32_t, bool &) const;

    void explode(Index, uint32_t, bool &);

    Index next_wormholl(Index) const;

    ~Field();

    std::string render() const;

    std::vector<std::vector<uint32_t>> neighborhood(Index i,
                                                    uint32_t radius) const;

    uint32_t &operator[](const Index);

    const uint32_t &operator[](const Index) const;

    const std::vector<Index> &get_holles() const;

private:
    void generate(uint32_t);

    std::vector<std::vector<uint32_t>> data;

    std::vector<Index> wormholles;
};
