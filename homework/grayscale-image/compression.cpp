#include "compression.hpp"
#include <algorithm>

class ImageCompression_insert_iterator {
public:
    explicit ImageCompression_insert_iterator(CompressedImage& compressed)
        : container(compressed) {}

    ImageCompression_insert_iterator& operator=(PixelType value) {
        if (width == maxWidth) {
            width = 0;
        }

        if (width == 0 || container.back().first != value) {
            container.push_back(CompressPair(value, 1u));
        } else {
            ++container.back().second;
        }
        ++width;
        return *this;
    }

    ImageCompression_insert_iterator& operator=(const ImageLine& line) {
        std::transform(cbegin(line), cend(line), *this,
                       [](PixelType pixel) {
                           return pixel;
                       });
        return *this;
    }

    ImageCompression_insert_iterator& operator*() { return *this; }
    ImageCompression_insert_iterator& operator++() { return *this; }

private:
    CompressedImage& container;
    size_t width = 0;
};

CompressedImage compressGrayscale(const Image& image) {
    CompressedImage compress;
    std::transform(cbegin(image), cend(image), ImageCompression_insert_iterator(compress),
                   [](const ImageLine& line) {
                       return line;
                   });
    return compress;
}

class ImageDecompression_insert_iterator {
public:
    explicit ImageDecompression_insert_iterator(Image& image)
        : container(image) {}

    ImageDecompression_insert_iterator& operator=(const CompressPair& pair) {
        auto [code, count] = pair;
        std::fill_n(&container[height][width], count, code);
        width += count;
        if (width == maxWidth) {
            width = 0;
            ++height;
        }
        return *this;
    }

    ImageDecompression_insert_iterator& operator*() { return *this; }
    ImageDecompression_insert_iterator& operator++() { return *this; }

private:
    Image& container;
    size_t width = 0;
    size_t height = 0;
};

Image decompressGrayscale(const CompressedImage& pack) {
    Image result;
    std::transform(cbegin(pack), cend(pack), ImageDecompression_insert_iterator(result),
                   [](const auto& pair) {
                       return pair;
                   });
    return result;
}

#include <iterator>
namespace {

enum class PrintVersion {
    FUNCTION,
    LAMBDA,
    ITERATOR,
    NONE
};
constexpr auto print_version = PrintVersion::ITERATOR;

PixelType convert(PixelType code) {
    constexpr uint8_t additionalPrintRangeFrom = 127;
    if (std::isprint(code) || code >= additionalPrintRangeFrom) {
        return code;
    }
    return PixelType{' '};
}

void printLine(Image::const_iterator lineBegin, Image::const_iterator lineEnd) {
    if (lineBegin == lineEnd) {
        return;
    }
    std::transform(cbegin(*lineBegin), cend(*lineBegin), std::ostream_iterator<PixelType>(std::cout, ""),
                   [](PixelType code) {
                       return convert(code);
                   });
    std::cout << '\n';

    printLine(std::next(lineBegin), lineEnd);
}

void printCompresedMap(CompressedImage::const_iterator pairBegin, CompressedImage::const_iterator pairEnd, size_t width = 0) {
    if (pairBegin == pairEnd) {
        return;
    }
    auto [code, times] = *pairBegin;
    std::fill_n(std::ostream_iterator<PixelType>(std::cout, ""), times, convert(code));

    width += times;
    if (width == maxWidth) {
        std::cout << '\n';
        width = 0;
    }
    printCompresedMap(std::next(pairBegin), pairEnd, width);
}

void printWithLambdas(const CompressedImage& compressed) {
    size_t width{};
    std::function<void(CompressedImage::const_iterator)> print =
        [pairEnd = cend(compressed), &width, &print](auto pairBegin) {
            if (pairBegin == pairEnd) {
                return;
            }
            auto [code, times] = *pairBegin;
            std::fill_n(std::ostream_iterator<PixelType>(std::cout, ""), times, convert(code));

            width += times;
            if (width == maxWidth) {
                std::cout << '\n';
                width = 0;
            }
            print(std::next(pairBegin));
        };
    print(begin(compressed));
}

}  // namespace

class Print_insert_iterator {
public:
    Print_insert_iterator() {}

    Print_insert_iterator& operator=(const ImageLine& line) {
        std::transform(cbegin(line), cend(line), std::ostream_iterator<PixelType>(std::cout, ""),
                       [](PixelType code) {
                           return convert(code);
                       });
        std::cout << '\n';
        return *this;
    }

    Print_insert_iterator& operator=(const CompressPair& pair) {
        auto [code, count] = pair;
        std::fill_n(container, count, convert(code));
        width += count;
        if (width == maxWidth) {
            std::cout << '\n';
            width = 0;
        }
        return *this;
    }

    Print_insert_iterator& operator*() { return *this; }
    Print_insert_iterator& operator++() { return *this; }

private:
    std::ostream_iterator<PixelType> container = std::ostream_iterator<PixelType>(std::cout, "");
    size_t width = 0;
};

void printMap(const Image& map) {
    if constexpr (print_version == PrintVersion::ITERATOR) {
        std::transform(cbegin(map), cend(map), Print_insert_iterator(),
                       [](const auto& line) {
                           return line;
                       });
    } else {
        printLine(cbegin(map), cend(map));
    }
}

void printCompresedMap(const CompressedImage& compressed) {
    if constexpr (print_version == PrintVersion::FUNCTION) {
        printCompresedMap(cbegin(compressed), cend(compressed));
    } else if constexpr (print_version == PrintVersion::LAMBDA) {
        printWithLambdas(compressed);
    } else if constexpr (print_version == PrintVersion::ITERATOR) {
        std::transform(cbegin(compressed), cend(compressed), Print_insert_iterator(),
                       [](const auto& pair) {
                           return pair;
                       });
    }
}
