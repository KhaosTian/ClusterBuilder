#pragma once

#include <string>

template<typename FuncType>
static inline void ParallelFor(const std::string& message, int32_t count, int32_t batch_size, FuncType Function) {
    for (int32_t index = 0; index < count; ++index) {
        Function(index);
    }
}
