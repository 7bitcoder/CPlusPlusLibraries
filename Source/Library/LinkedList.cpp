#include <algorithm>
#include <numeric>

#include "LinkedList.hpp"

namespace sd
{
    namespace
    {
        template <class T>
        void printAll(const List<T> &list)
        {
            auto size = list.size();
            size_t i = 0;
            std::cout << "{";
            auto it = list.begin();
            for (; it != list.end(); ++it)
            {
                std::cout << *it;
                if (i != size - 1)
                {
                    std::cout << ", ";
                }
                i++;
            }
            std::cout << "}" << std::endl;
        }
    }
    void linkedMain()
    {
        List<int> list;

        list.pushBack(1);
        list.pushBack(2);
        list.pushBack(3);

        printAll(list);
        list.remove(1);
        printAll(list);

        list.remove(0);
        printAll(list);

        list.pushBack(2);
        list.pushBack(3);
        printAll(list);

        list.insert(1, 6);
        printAll(list);

        list.emplaceBack(22);
        printAll(list);

        list.emplaceBack(0);

        int sum = std::accumulate(list.begin(), list.end(), 0);
        list.clear();
        list.pushBack(sum);
        printAll(list);

        list.back();
        list.front();

        list.clear();
        list.clear();
        printAll(list);
    }
}