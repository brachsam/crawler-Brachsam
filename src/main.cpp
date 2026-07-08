#include <iostream>
#include "DynamicArray/Dynamicarray.h"

int main()
{
    DynamicArray<int> arr;

    arr.append(10);
    arr.append(20);

    std::cout << arr.get(0) << std::endl;
    std::cout << arr.get(1) << std::endl;

    return 0;
}