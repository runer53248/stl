#include "compression.hpp"
#include <algorithm>

std::vector<std::pair<uint8_t, uint8_t>>
compressGrayscale(std::array<std::array<uint8_t, width>, height> image) {
    std::vector<std::pair<uint8_t, uint8_t>> result;
    uint8_t value = 0;
    bool start = true;
    for (const auto& line : image) {
        start = true;
        for (const auto& color : line) {
            if (color != value || start) {
                value = color;
                result.emplace_back(value, 1);
                start = false;
            } else {
                result[result.size() - 1].second += 1;
            }
        }
    }
    return result;
}

std::array<std::array<uint8_t, width>, height> decompressGrayscale(std::vector<std::pair<uint8_t, uint8_t>> pack) {
    std::array<uint8_t, width> line;
    line.fill(0);
    std::array<std::array<uint8_t, width>, height> rect;
    rect.fill(line);
    return rect;
}

void printCode(uint8_t code) {
    if (std::isprint(code) || code > 127) {
        std::cout << static_cast<char>(code);
    } else {
        std::cout << ' ';
    }
}

void printCompresedMap(std::vector<std::pair<uint8_t, uint8_t>> compressed, size_t width) {
    int x = 0;
    for (const auto& pack : compressed) {
        for (int i = 0; i < pack.second; ++i) {
            printCode(pack.first);
            if (++x == width) {
                x = 0;
                std::cout << '\n';
            }
        }
    }
}
