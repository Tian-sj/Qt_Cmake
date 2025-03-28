#include <iostream>

void add(int& a) {
    a += 2;
}

int main() {
    int x = 5;
    add(std::ref(x));
    std::cout << x << std::endl;
    return 0;
}
