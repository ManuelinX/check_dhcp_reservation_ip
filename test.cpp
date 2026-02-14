#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iostream>

int main() {
    std::unordered_map<std::string, unsigned int> uniqueStrings{
        {"apple", 5},
        {"banana", 2},
        {"orange", 8}
    };

    // 1️⃣ Pasar a vector
    std::vector<std::pair<std::string, unsigned int>> vec(
        uniqueStrings.begin(),
        uniqueStrings.end()
    );

    // 2️⃣ Ordenar por second
    std::sort(vec.begin(), vec.end(),
        [](const auto& a, const auto& b) {
            return a.second < b.second;  // ascendente
        });

    // 3️⃣ Mostrar resultado
    for (const auto& [key, value] : vec) {
        std::cout << key << " -> " << value << "\n";
    }
}
