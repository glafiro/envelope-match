#pragma once

#include <vector>
using std::vector;
#include <iostream>
#include <algorithm>

class RingQueue {
private:
    vector<float> data{};
    size_t idx{ 0 };
    size_t length{ 0 };

public:
    void prepare(size_t N) {
        length = N;
        data.resize(length, 0.0f);
    }

    void insert(const float* buffer, size_t n) {
        if (idx > 0) {
            size_t shift_amount = std::min(n, idx);
            for (size_t i = 0; i < length - shift_amount; ++i) {
                data[i] = data[i + shift_amount];
            }
        }

        std::copy(buffer, buffer + n, data.end() - n);
        idx = std::min(idx + n, length);
    }

    float* getData() {
        return data.data();
    }

    size_t getSize() {
        return length;
    }
};

