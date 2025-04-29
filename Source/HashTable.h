#pragma once

// 基于开放寻址法的哈希表
class HashTable {
public:
    HashTable(uint32 hash_size = 1024, uint32 index_size = 0);
    HashTable(const HashTable& other);
    HashTable(HashTable&& other) noexcept;
    HashTable& operator=(const HashTable& other);
    HashTable& operator=(HashTable&& other) noexcept;

    void Clear() const;
    void Free();

    void Resize(uint32 new_index_size);

    uint32 IndexSize() const { return m_index_size; }
    uint32 HashSize() const { return m_hash_size; }

    uint32 First(uint32 key) const;
    uint32 Next(uint32 index) const;
    bool     IsValid(uint32 index) const;

    void Add(uint32 key, uint32 index);
    void AddConcurrent(uint32 key, uint32 index) const;
    void Remove(uint32 key, uint32 index) const;

protected:
    uint32 m_hash_size;
    uint32 m_hash_mask;
    uint32 m_index_size;

    uint32* m_hash; // 哈希数组中存储每个桶的链表头节点索引
    uint32* m_next_index; // 存储从index可以达到的下一个索引

    static uint32 EmptyHash[1];
};