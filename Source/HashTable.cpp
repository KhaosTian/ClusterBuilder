#include "HashTable.h"
#include <bit>
#include "Common.h"

HashTable::HashTable(uint32_t hashSize, uint32_t indexSize) :
    m_HashSize(hashSize), m_HashMask(0), m_IndexSize(indexSize), m_Hash(EmptyHash), m_NextIndex(nullptr)
{
    // 确保哈希表的大小大于0且是2的幂次方
    CHECK(m_HashSize > 0);
    CHECK(std::has_single_bit(m_HashSize));

    // 对于大小为2的幂次方的哈希表，x % m_HashSize 等价于 x & (m_HashSize - 1)
    if (m_IndexSize)
    {
        m_HashMask = m_HashSize - 1;
        // 分配哈希桶的头索引和链表
        m_Hash = new uint32_t[m_IndexSize];
        m_NextIndex = new uint32_t[m_IndexSize];
        // 初始化数组元素为0xff
        std::memset(m_Hash, 0xff, m_HashSize * sizeof(uint32_t));
    }
}

HashTable::HashTable(const HashTable& other) :
    m_HashSize(other.m_HashSize), m_HashMask(other.m_HashMask), m_IndexSize(other.m_IndexSize),
    m_Hash(EmptyHash) // 让未初始化或已释放的哈希表也能安全响应

{
    if (m_IndexSize)
    {
        m_Hash = new uint32_t[m_HashSize];
        m_NextIndex = new uint32_t[m_IndexSize];

        // 拷贝内存
        std::memcmp(m_Hash, other.m_Hash, m_HashSize * sizeof(uint32_t));
        std::memcmp(m_NextIndex, other.m_NextIndex, m_IndexSize * sizeof(uint32_t));
    }
}

HashTable::HashTable(HashTable&& other) noexcept :
    m_HashSize(other.m_HashSize), m_HashMask(other.m_HashMask), m_IndexSize(other.m_IndexSize), m_Hash(other.m_Hash),
    m_NextIndex(other.m_NextIndex)
{
    other.m_HashSize = 0;
    other.m_HashMask = 0;
    other.m_IndexSize = 0;
    other.m_Hash = EmptyHash;
    other.m_NextIndex = m_NextIndex;
}

HashTable& HashTable::operator=(const HashTable& other)
{ Free();
    m_HashSize = other.m_HashSize;
    m_HashMask = other.m_HashMask;
    m_IndexSize = other.m_IndexSize;

    if (m_IndexSize)
    {
        m_Hash = new uint32_t[m_HashSize];
        m_NextIndex = new uint32_t[m_IndexSize];

        // 拷贝内存
        std::memcmp(m_Hash, other.m_Hash, m_HashSize * sizeof(uint32_t));
        std::memcmp(m_NextIndex, other.m_NextIndex, m_IndexSize * sizeof(uint32_t));
    }
    return *this;
}

HashTable& HashTable::operator=(HashTable&& other)
{
    m_HashSize = other.m_HashSize;
    m_HashMask = other.m_HashMask;
    m_IndexSize = other.m_IndexSize;
    m_Hash = other.m_Hash;
    m_NextIndex = other.m_NextIndex;

    other.m_HashSize = 0;
    other.m_HashMask = 0;
    other.m_IndexSize = 0;
    other.m_Hash = EmptyHash;
    other.m_NextIndex = m_NextIndex;
    return *this;
}

void HashTable::Clear()
{
    // 切断从桶到链表的访问入口
    if (m_IndexSize)
    {
        std::memset(m_Hash, 0xff, m_HashSize * sizeof(uint32_t));
    }
}

void HashTable::Free()
{
    if (m_IndexSize)
    {
        m_HashMask = 0;
        m_IndexSize = 0;

        delete[] m_Hash;
        m_Hash = EmptyHash;

        delete[] m_NextIndex;
        m_NextIndex = nullptr;
    }
}

void HashTable::Resize(uint32_t newIndexSize) {}

// 返回key对应的链表的第一个索引
uint32_t HashTable::First(uint32_t key) const
{
    key &= m_HashMask;
    return m_Hash[key];
}

// 返回链表当前索引后的下一个索引
uint32_t HashTable::Next(uint32_t index) const
{
    CHECK(index < m_IndexSize);
    // 避免链表节点自引用导致的无限循环
    CHECK(m_NextIndex[index] != index);
    return m_NextIndex[index];
}

bool HashTable::IsValid(uint32_t index) const { return index != ~0u; }

// key决定元素放入哪个桶，index决定数据在外部数组中的索引位置，HashTable本身不存储数据，只存储索引
void HashTable::Add(uint32_t key, uint32_t index)
{
    // 如果提供的索引超出当前哈希表容量，动态扩容
    if (index >= m_IndexSize)
    {
        // 新容量取32和(index+1)向上取整到2的幂中的较大值
        Resize(std::max<uint32_t>(32u, index + 1 <= 1 ? 1u : std::bit_ceil(static_cast<uint32_t>(index + 1))));
    }

    // 等价于 key % m_HashSize
    key &= m_HashMask;

    // 使用头插法处理哈希冲突，构建链表
    // m_Hash[key]存储的是头节点的索引
    m_NextIndex[index] = m_Hash[key];
    // 更新链表头为新添加的元素，从m_Hash[key]可以访问到整个链表上的所有元素:
    m_Hash[key] = index;
}

void HashTable::AddConcurrent(uint32_t key, uint32_t index)
{
    CHECK(index < m_IndexSize);

    key &= m_HashMask;
    // 使用原子交换操作实现线程安全的头插法
    m_NextIndex[index] = std::atomic_exchange( // 将一个原子对象的值替换为新值并返回替换前的旧值
        reinterpret_cast<std::atomic<uint32_t>*>(&m_Hash[key]), //将m_Hash[key]视为原子变量
        index // 原子地读取m_Hash[key]的当前值，并将其替换为新的index
    ); // 将原始值设置为新元素的next指针
}

void HashTable::Remove(uint32_t key, uint32_t index)
{
    if (index >= m_IndexSize)
    {
        return;
    }

    key &= m_HashMask;

    // 如果index正好是该key桶的头节点
    if (m_Hash[key] == index)
    {
        // 将后一个节点设为头节点即可
        m_Hash[key] = m_NextIndex[index]; 
        return;
    }

    // 从头节点开始遍历
    for (uint32_t i = m_Hash[key]; IsValid(i); i = m_NextIndex[i])
    {
        if (m_NextIndex[i] == index) // 找到该节点
        {
            m_NextIndex[i] = m_NextIndex[index]; // 指向下下一个节点即可
            break;
        }
    }
}
