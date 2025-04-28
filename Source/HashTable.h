#pragma once
#include <cstdint>
#include <initializer_list>

static inline uint32_t MurmurFinalize32(uint32_t hash) {
    hash ^= hash >> 16;
    hash *= 0x85ebca6b;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= hash >> 16;
    return hash;
}

static inline uint32_t Murmur32(std::initializer_list<uint32_t> init_list) {
    uint32_t hash = 0;
    for (auto element: init_list) {
        element *= 0xcc9e2d51;
        element = (element << 15) | (element >> (32 - 15));
        element *= 0x1b873593;

        hash ^= element;
        hash = (hash << 13) | (hash >> (32 - 13));
        hash = hash * 5 + 0xe6546b64;
    }

    return MurmurFinalize32(hash);
}

// 基于开放寻址法的哈希表
class HashTable {
public:
    HashTable(uint32_t hash_size = 1024, uint32_t index_size = 0);
    HashTable(const HashTable& other);
    HashTable(HashTable&& other) noexcept;
    HashTable& operator=(const HashTable& other);
    HashTable& operator=(HashTable&& other) noexcept;

    void Clear();
    void Free();

    void     Resize(uint32_t new_index_size);

    uint32_t index_size() const { return m_index_size; }
    uint32_t hash_size() const { return m_hash_size; }

    uint32_t First(uint32_t key) const;
    uint32_t Next(uint32_t index) const;
    bool     IsValid(uint32_t index) const;

    void Add(uint32_t key, uint32_t index);
    void AddConcurrent(uint32_t key, uint32_t index);
    void Remove(uint32_t key, uint32_t index);

protected:
    uint32_t m_hash_size;
    uint32_t m_hash_mask;
    uint32_t m_index_size;

    uint32_t* m_hash; // 哈希数组中存储每个桶的链表头节点索引
    uint32_t* m_next_index; // 存储从index可以达到的下一个索引

    static uint32_t EmptyHash[1];
};
