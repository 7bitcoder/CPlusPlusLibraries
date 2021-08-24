#include <algorithm>
#include <numeric>

#include "Map.hpp"

namespace sd
{
    void mapMain()
    {
        Map<int, std::string> m;
        m.pushBack(3, "halo");
        m.pushBack(4, "halo");
        m.pushBack(6, "halo");
        m.pushBack(6, "halo");
        m.remove(6);
    }
}