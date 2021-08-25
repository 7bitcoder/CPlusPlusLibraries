#include <algorithm>
#include <numeric>

#include "Map.hpp"

namespace sd
{
    void mapMain()
    {
        Map<int, std::string> m;
        m.insert({3, "halo"});
        m.insert({4, "halo"});
        m.insert({6, "halo"});
        m.insert({6, "halo"});

        auto& res = m.at(4);
        m.remove(6);

        auto it = m.begin();

        ++it;

        ++it;
    }
}