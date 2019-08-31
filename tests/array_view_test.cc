#include <iostream>
#include <iomanip>
#include <vector>

#include "../array_view.h"

int main(void) {
    std::vector<int> vec;
    for (int i = 0; i < 10; ++i) {
        vec.push_back(i);
    }

    ArrayView<int> view(vec);
    std::cout << "Full view: " << view.str() << std::endl;

    ArrayView<int> view2 = view[std::slice(0, 5, 2)];
    std::cout << "view[slice(0, 5, 2)]: " << view2.str() << std::endl;

    ArrayView<int> view3 = view[std::slice(0, 3, 3)];
    std::cout << "view[slice(0, 3, 3)]: " << view3.str() << std::endl;
    std::vector<int> copy = view3.copy<std::vector<int>>();
    std::cout << "view[slice(0, 3, 3)].copy(): " << std::endl;
    for (auto i = copy.begin(); i != copy.end(); ++i)
        std::cout << *i << ' ';
    std::cout << std::endl;

    view3 = std::vector<int>{77, 88, 99};
    std::cout << "Full view: " << view.str() << std::endl;
}
