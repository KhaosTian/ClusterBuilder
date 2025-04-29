#pragma once

// 基于开放寻址法的哈希表
class HashTable {
public:
    HashTable(uint32_t hash_size = 1024, uint32_t index_size = 0);
    HashTable(const HashTable& other);
    HashTable(HashTable&& other) noexcept;
    HashTable& operator=(const HashTable& other);
    HashTable& operator=(HashTable&& other) noexcept;

    void Clear() const;
    void Free();

    void Resize(uint32_t new_index_size);

    uint32_t IndexSize() const { return m_index_size; }
    uint32_t HashSize() const { return m_hash_size; }

    uint32_t First(uint32_t key) const;
    uint32_t Next(uint32_t index) const;
    bool     IsValid(uint32_t index) const;

    void Add(uint32_t key, uint32_t index);
    void AddConcurrent(uint32_t key, uint32_t index) const;
    void Remove(uint32_t key, uint32_t index) const;

protected:
    uint32_t m_hash_size;
    uint32_t m_hash_mask;
    uint32_t m_index_size;

    uint32_t* m_hash; // 哈希数组中存储每个桶的链表头节点索引
    uint32_t* m_next_index; // 存储从index可以达到的下一个索引

    static uint32_t EmptyHash[1];
};