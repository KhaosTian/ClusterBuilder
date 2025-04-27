#pragma once

#include <string>

template <typename FunctionType>
static inline void ParallelFor(const std::string& message, int32_t count, int32_t batchSize, FunctionType Function)
{
    for (int32_t index = 0; index < Count; ++index)
    {
        Function(index);
    }
}
