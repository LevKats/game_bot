#include "Field.h"
#include <algorithm>
#include <cassert>
#include <map>
#include <set>

Field::Field(size_t size, uint32_t holl_num)
    : data(size, std::vector<uint32_t>(size)) {
    generate(holl_num);
}

Field::~Field() {}

std::string Field::render() const {
    std::string result;
    std::vector<std::vector<char>> temp(
        2 * data.size() + 2, std::vector<char>(2 * data.size() + 2, ' '));

    for (uint32_t i = 0; i < data.size(); ++i) {
        for (uint32_t j = 0; j < data.size(); ++j) {
            temp[2 * i][2 * j] = temp[2 * i + 2][2 * j + 2] =
                temp[2 * i][2 * j + 2] = temp[2 * i + 2][2 * j] = '#';

            if ((*this)[{i, j}] & CellFlags::CHARACTER) {
                temp[2 * i + 1][2 * j + 1] = 'C';
            }
            if ((*this)[{i, j}] & CellFlags::TREASURE) {
                temp[2 * i + 1][2 * j + 1] = 'T';
            }

            if ((*this)[{i, j}] & CellFlags::LEFT_BORDER) {
                temp[2 * i + 1][2 * j] = '#';
            }
            if ((*this)[{i, j}] & CellFlags::RIGHT_BORDER) {
                temp[2 * i + 1][2 * j + 2] = '#';
            }
            if ((*this)[{i, j}] & CellFlags::UP_BORDER) {
                temp[2 * i][2 * j + 1] = '#';
            }
            if ((*this)[{i, j}] & CellFlags::DOWN_BORDER) {
                temp[2 * i + 2][2 * j + 1] = '#';
            }

            if ((*this)[{i, j}] & CellFlags::HOSPITAL) {
                temp[2 * i + 1][2 * j + 1] = 'H';
            }
            if ((*this)[{i, j}] & CellFlags::ARMORY) {
                temp[2 * i + 1][2 * j + 1] = 'A';
            }

            if ((*this)[{i, j}] & CellFlags::WORM_HOLL) {
                for (uint32_t p = 0; p < wormholles.size(); ++p) {
                    if (wormholles[p] == Field::Index{i, j}) {
                        temp[2 * i + 1][2 * j + 1] = '1' + p;
                    }
                }
            }

            if ((*this)[{i, j}] & CellFlags::EXIT) {
                temp[2 * i + 1][2 * j + 1] = 'E';
            }

            if ((*this)[{i, j}] & CellFlags::HIDDEN) {
            }
        }
    }

    for (auto &row : temp) {
        for (auto c : row) {
            result += c;
        }
        result += '\n';
    }

    return result;
}

const std::vector<Field::Index> &Field::get_holles() const {
    return wormholles;
}

uint32_t &Field::operator[](const Field::Index i) {
    assert(i.first < data.size() && i.second < data[i.first].size());
    return data[i.first][i.second];
}

const uint32_t &Field::operator[](const Field::Index i) const {
    assert(i.first < data.size() && i.second < data[i.first].size());
    return data[i.first][i.second];
}

std::vector<std::pair<uint32_t, uint32_t>>
minimum_spaning_tree(const std::vector<std::pair<uint32_t, uint32_t>> &graph,
                     const std::vector<uint32_t> &order) {
    // returns edges
    assert(order.size() == graph.size());

    std::map<std::pair<uint32_t, uint32_t>, uint32_t> mergedMap;

    std::transform(graph.begin(), graph.end(), order.begin(),
                   std::inserter(mergedMap, mergedMap.end()),
                   [](std::pair<uint32_t, uint32_t> a, uint32_t b) {
                       return std::make_pair(a, b);
                   });

    std::vector<bool> used(graph.size(), false); // TODO fix size

    auto cmp = [&mergedMap](std::pair<uint32_t, uint32_t> a,
                            std::pair<uint32_t, uint32_t> b) {
        return mergedMap[a] < mergedMap[b];
    };
    std::set<std::pair<uint32_t, uint32_t>, decltype(cmp)> s(cmp);

    std::vector<std::pair<uint32_t, uint32_t>> result;
    std::set<std::pair<uint32_t, uint32_t>> temp(graph.begin(), graph.end());

    uint32_t last = 0;
    used[0] = true;
    s.insert(temp.lower_bound({last, 0}),
             temp.upper_bound({last, graph.size()}));

    while (!s.empty()) {
        auto p = *s.begin();
        s.erase(s.begin());
        if (!used[p.second]) {
            result.push_back(p);
            last = p.second;
            used[last] = true;
            s.insert(temp.lower_bound({last, 0}),
                     temp.upper_bound({last, graph.size()}));
        }
    }

    return result;
}

void Field::generate(uint32_t holl_num) {
    for (auto &row : data) {
        for (auto &cell : row) {
            cell = CellFlags::LEFT_BORDER | CellFlags::RIGHT_BORDER |
                   CellFlags::UP_BORDER | CellFlags::DOWN_BORDER;
        }
    }

    const size_t size = data.size();
    auto encode = [size](Field::Index i) { return i.first * size + i.second; };
    auto decode = [size](uint32_t i) {
        return Field::Index(i / size, i % size);
    };

    std::vector<std::pair<uint32_t, uint32_t>> graph;
    std::vector<uint32_t> order;
    std::vector<uint32_t> tmp;

    for (uint32_t i = 0, t = 0; i < size; ++i) {
        for (uint32_t j = 0; j < size; ++j) {
            tmp.push_back(encode({i, j}));
            if (i != 0) {
                graph.push_back({encode({i, j}), encode({i - 1, j})});
            }
            if (i != size - 1) {
                graph.push_back({encode({i, j}), encode({i + 1, j})});
            }
            if (j != 0) {
                graph.push_back({encode({i, j}), encode({i, j - 1})});
            }
            if (j != size - 1) {
                graph.push_back({encode({i, j}), encode({i, j + 1})});
            }
            for (; order.size() < graph.size(); order.push_back(t++)) {
            }
        }
    }

    assert(size * size > 4 + holl_num);
    std::random_shuffle(tmp.begin(), tmp.end());
    (*this)[decode(tmp[0])] ^= CellFlags::EXIT;
    (*this)[decode(tmp[1])] ^= CellFlags::ARMORY;
    (*this)[decode(tmp[2])] ^= CellFlags::HOSPITAL;
    (*this)[decode(tmp[3])] ^= CellFlags::TREASURE;
    for (uint32_t i = 0; i < holl_num; ++i) {
        auto p = decode(tmp[4 + i]);
        wormholles.push_back(p);
        (*this)[p] ^= CellFlags::WORM_HOLL;
    }

    std::random_shuffle(order.begin(), order.end());
    auto tree = minimum_spaning_tree(graph, order);
    for (auto edge : tree) {
        auto from = decode(edge.first);
        auto to = decode(edge.second);
        if (from.first > to.first) {
            (*this)[from] ^= CellFlags::UP_BORDER;
            (*this)[to] ^= CellFlags::DOWN_BORDER;
        }
        if (from.first < to.first) {
            (*this)[from] ^= CellFlags::DOWN_BORDER;
            (*this)[to] ^= CellFlags::UP_BORDER;
        }
        if (from.second > to.second) {
            (*this)[from] ^= CellFlags::LEFT_BORDER;
            (*this)[to] ^= CellFlags::RIGHT_BORDER;
        }
        if (from.second < to.second) {
            (*this)[from] ^= CellFlags::RIGHT_BORDER;
            (*this)[to] ^= CellFlags::LEFT_BORDER;
        }
    }
}
