#include "HashTable.h"
#include <bit>
#include "Common.h"

HashTable::HashTable(uint32_t hashSize, uint32_t indexSize) :
    m_HashSize(hashSize), m_HashMask(0), m_IndexSize(indexSize), m_Hash(EmptyHash), m_NextIndex(nullptr)
{
    // ȷ����ϣ��Ĵ�С����0����2���ݴη�
    CHECK(m_HashSize > 0);
    CHECK(std::has_single_bit(m_HashSize));

    // ���ڴ�СΪ2���ݴη��Ĺ�ϣ��x % m_HashSize �ȼ��� x & (m_HashSize - 1)
    if (m_IndexSize)
    {
        m_HashMask = m_HashSize - 1;
        // �����ϣͰ��ͷ����������
        m_Hash = new uint32_t[m_IndexSize];
        m_NextIndex = new uint32_t[m_IndexSize];
        // ��ʼ������Ԫ��Ϊ0xff
        std::memset(m_Hash, 0xff, m_HashSize * sizeof(uint32_t));
    }
}

HashTable::HashTable(const HashTable& other) :
    m_HashSize(other.m_HashSize), m_HashMask(other.m_HashMask), m_IndexSize(other.m_IndexSize),
    m_Hash(EmptyHash) // ��δ��ʼ�������ͷŵĹ�ϣ��Ҳ�ܰ�ȫ��Ӧ

{
    if (m_IndexSize)
    {
        m_Hash = new uint32_t[m_HashSize];
        m_NextIndex = new uint32_t[m_IndexSize];

        // �����ڴ�
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

        // �����ڴ�
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
    // �жϴ�Ͱ������ķ������
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

// ����key��Ӧ������ĵ�һ������
uint32_t HashTable::First(uint32_t key) const
{
    key &= m_HashMask;
    return m_Hash[key];
}

// ��������ǰ���������һ������
uint32_t HashTable::Next(uint32_t index) const
{
    CHECK(index < m_IndexSize);
    // ��������ڵ������õ��µ�����ѭ��
    CHECK(m_NextIndex[index] != index);
    return m_NextIndex[index];
}

bool HashTable::IsValid(uint32_t index) const { return index != ~0u; }

// key����Ԫ�ط����ĸ�Ͱ��index�����������ⲿ�����е�����λ�ã�HashTable�����洢���ݣ�ֻ�洢����
void HashTable::Add(uint32_t key, uint32_t index)
{
    // ����ṩ������������ǰ��ϣ����������̬����
    if (index >= m_IndexSize)
    {
        // ������ȡ32��(index+1)����ȡ����2�����еĽϴ�ֵ
        Resize(std::max<uint32_t>(32u, index + 1 <= 1 ? 1u : std::bit_ceil(static_cast<uint32_t>(index + 1))));
    }

    // �ȼ��� key % m_HashSize
    key &= m_HashMask;

    // ʹ��ͷ�巨�����ϣ��ͻ����������
    // m_Hash[key]�洢����ͷ�ڵ������
    m_NextIndex[index] = m_Hash[key];
    // ��������ͷΪ����ӵ�Ԫ�أ���m_Hash[key]���Է��ʵ����������ϵ�����Ԫ��:
    m_Hash[key] = index;
}

void HashTable::AddConcurrent(uint32_t key, uint32_t index)
{
    CHECK(index < m_IndexSize);

    key &= m_HashMask;
    // ʹ��ԭ�ӽ�������ʵ���̰߳�ȫ��ͷ�巨
    m_NextIndex[index] = std::atomic_exchange( // ��һ��ԭ�Ӷ����ֵ�滻Ϊ��ֵ�������滻ǰ�ľ�ֵ
        reinterpret_cast<std::atomic<uint32_t>*>(&m_Hash[key]), //��m_Hash[key]��Ϊԭ�ӱ���
        index // ԭ�ӵض�ȡm_Hash[key]�ĵ�ǰֵ���������滻Ϊ�µ�index
    ); // ��ԭʼֵ����Ϊ��Ԫ�ص�nextָ��
}

void HashTable::Remove(uint32_t key, uint32_t index)
{
    if (index >= m_IndexSize)
    {
        return;
    }

    key &= m_HashMask;

    // ���index�����Ǹ�keyͰ��ͷ�ڵ�
    if (m_Hash[key] == index)
    {
        // ����һ���ڵ���Ϊͷ�ڵ㼴��
        m_Hash[key] = m_NextIndex[index]; 
        return;
    }

    // ��ͷ�ڵ㿪ʼ����
    for (uint32_t i = m_Hash[key]; IsValid(i); i = m_NextIndex[i])
    {
        if (m_NextIndex[i] == index) // �ҵ��ýڵ�
        {
            m_NextIndex[i] = m_NextIndex[index]; // ָ������һ���ڵ㼴��
            break;
        }
    }
}
