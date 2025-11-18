/*
#include <iostream>
#include <string>
#include <ranges>
#include <sstream>

int main(){
    // std::string s = "apple,banana,cherry";
    // C++20
    for (auto &&subrange : s | std::views::split(',')) {
        std::string part(subrange.begin(), subrange.end());
        std::cout << part << std::endl;
    }

    // C++23
    auto split_view = std::ranges::split_view(s, std::string_view(","));
    for (auto &&subrange : split_view) {
        std::string part(subrange.begin(), subrange.end());
        std::cout << part << std::endl;
    }

    // C++98
    std::istringstream iss(s);
    std::string part;
    while (std::getline(iss, part, ',')) {
        std::cout << part << std::endl;
    }

    return 0;
}
*/

#include <iostream>
#include <soci/soci.h>

int main() {
    // soci::session sql(soci::pos)
    return 0;
}
