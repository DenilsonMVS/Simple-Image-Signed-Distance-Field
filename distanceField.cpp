
#include "distanceField.hpp"

#include <array>
#include <cmath>
#include <functional>


struct Vec2 {
    size_t x, y;
};

static float sqf(const size_t num) {
    return num * num;
}

static float pixelDistance(const Vec2 &pos1, const Vec2 &pos2) {
    return std::sqrt(sqf(pos1.x - pos2.x) + sqf(pos1.y - pos2.y));
}

std::vector<float> generateDistanceField(const std::vector<bool> &bitmap, const size_t width) {
    
    constexpr float UNCALCULATED_DISTANCE = 1e10f;
    constexpr size_t UNCALCULATED_CLOSEST = 0xffffffffffffffff;
    constexpr size_t NUM_BUFFERS = 2;


    const auto height = bitmap.size() / width;

    auto distanceField = std::vector<float>(bitmap.size(), UNCALCULATED_DISTANCE);
    auto closestPixel = std::vector<size_t>(bitmap.size(), UNCALCULATED_CLOSEST);

    auto buffers = std::array<std::vector<size_t>, 2>();
    size_t nextBuffer = 0;

    
    const auto getNextBuffer = [&buffers, &nextBuffer]() {
        const auto bufferIndex = nextBuffer;
        nextBuffer = (nextBuffer + 1) % NUM_BUFFERS;
        auto *buffer = &buffers[bufferIndex];
        buffer->clear();
        return buffer;
    };

    const auto isPixelInside = [width, height](const Vec2 &pos) {
        return pos.x < width && pos.y < height;
    };

    const auto vecToIndex = [width](const Vec2 &pos) {
        return pos.y * width + pos.x;
    };

    const auto indexToVec = [width](const size_t index) {
        return Vec2 {
            index % width,
            index / width
        };
    };


    const auto mainAlgorithm = [&](const bool bit) {
        
        const float sig = bit ? 1 : -1;

        auto *lastPixels = getNextBuffer();
        for(size_t i = 0; i < bitmap.size(); i++)
            if(bitmap[i] != bit)
                lastPixels->push_back(i);

        const auto runAllPixels = [&](const std::function<void (const Vec2 &from, const Vec2 &to, std::vector<size_t> &borderPixels)> &test) {
            auto *borderPixels = getNextBuffer();

            for(auto &pixelIndex : *lastPixels) {
                const auto pixelPos = indexToVec(pixelIndex);

                test(pixelPos, {pixelPos.x + 1, pixelPos.y}, *borderPixels);
                test(pixelPos, {pixelPos.x - 1, pixelPos.y}, *borderPixels);
                test(pixelPos, {pixelPos.x, pixelPos.y + 1}, *borderPixels);
                test(pixelPos, {pixelPos.x, pixelPos.y - 1}, *borderPixels);
            }

            lastPixels = borderPixels;
        };

        const auto firstIterationTest = [&](const Vec2 &from, const Vec2 &to, std::vector<size_t> &borderPixels) {
            if(!isPixelInside(to))
                return;
            
            const auto fromIndex = vecToIndex(from);
            const auto toIndex = vecToIndex(to);
            if(bitmap[toIndex] == bit && closestPixel[toIndex] == UNCALCULATED_CLOSEST) {
                borderPixels.push_back(toIndex);
                closestPixel[toIndex] = fromIndex;
                distanceField[toIndex] = sig;
            }
        };

        const auto test = [&](const Vec2 &from, const Vec2 &to, std::vector<size_t> &borderPixels) {
            if(!isPixelInside(to))
                return;
            
            const auto fromIndex = vecToIndex(from);
            const auto toIndex = vecToIndex(to);
            if(bitmap[toIndex] == bit) {
                if(closestPixel[toIndex] == UNCALCULATED_CLOSEST)
                    borderPixels.push_back(toIndex);
                
                const auto distance = pixelDistance(indexToVec(closestPixel[fromIndex]), to);
                if(std::abs(distance) < std::abs(distanceField[toIndex])) {
                    closestPixel[toIndex] = closestPixel[fromIndex];
                    distanceField[toIndex] = sig * distance;
                }
            }
        };

        runAllPixels(firstIterationTest);
        while(!lastPixels->empty())
            runAllPixels(test);
    };

    mainAlgorithm(false);
    mainAlgorithm(true);

    return distanceField;
}
