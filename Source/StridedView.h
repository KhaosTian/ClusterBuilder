#pragma once

#include <cassert>
#include <cstdint>
#include <type_traits>

template <typename InElementType, typename InSizeType = int32_t>
class StridedView;

template <typename T, typename SizeType = int32_t>
using ConstStridedView = StridedView<const T, SizeType>;

template <typename ElementType, typename SizeType>
class StridedView
{
public:
    static_assert(std::is_signed<SizeType>::value, "StridedView only supports signed index types");

    StridedView() = default;

    template <typename OtherElementType, typename = std::enable_if_t<std::is_convertible<OtherElementType*, ElementType*>::value>>
    StridedView(SizeType bytesBetweenElements, OtherElementType* firstElementPtr, SizeType numElements) :
        m_FirstElementPtr(firstElementPtr), m_BytesBetweenElements(bytesBetweenElements), m_NumElements(numElements)
    {
        assert(m_NumElements >= 0 && "Number of elements must be non-negative");
        assert(m_BytesBetweenElements >= 0 && "Stride must be non-negative");
        assert(m_BytesBetweenElements % alignof(ElementType) == 0 && "Stride must respect element alignment");
    }

    template <typename OtherElementType, typename = std::enable_if_t<std::is_convertible<OtherElementType*, ElementType*>::value>>
    StridedView(const StridedView<OtherElementType, SizeType>& other) :
        m_FirstElementPtr(nullptr), m_BytesBetweenElements(other.GetStride()), m_NumElements(other.Num())
    {
        if (other.Num() > 0)
        {
            m_FirstElementPtr = &other[0];
        }
    }

    bool IsValidIndex(SizeType index) const { return (index >= 0) && (index < m_NumElements); }
    bool IsEmpty() const { return m_NumElements == 0; }

    SizeType Num() const { return m_NumElements; }
    SizeType GetStride() const { return m_BytesBetweenElements; }
    ElementType& GetUnsafe(SizeType index) const { return *GetElementPtrUnsafe(index); }

    ElementType& operator[](SizeType index) const
    {
        RangeCheck(index);
        return *GetElementPtrUnsafe(index);
    }

    class Iterator
    {
    public:

        Iterator(const StridedView* owner, SizeType index) : m_Owner(owner), m_Index(index) {}

        Iterator& operator++()
        {
            ++m_Index;
            return *this;
        }

        ElementType& operator*() const { return m_Owner->GetUnsafe(m_Index); }

        bool operator==(const Iterator& other) const { return m_Owner == other.m_Owner && m_Index == other.m_Index; }
        bool operator!=(const Iterator& other) const { return !(*this == other); }

    private:
        const StridedView* m_Owner;
        SizeType           m_Index;
    };

    Iterator begin() const { return Iterator(this, 0); }
    Iterator end() const { return Iterator(this, m_NumElements); }

private:

    void RangeCheck(SizeType index) const { assert((index >= 0) && (index < m_NumElements) && "Array index out of bounds"); }

    ElementType* GetElementPtrUnsafe(SizeType index) const
    {
        using ByteType = typename std::conditional<std::is_const<ElementType>::value, const uint8_t, uint8_t>::type;
        ByteType* asBytes = reinterpret_cast<ByteType*>(m_FirstElementPtr);
        ElementType* asElement =
            reinterpret_cast<ElementType*>(asBytes + static_cast<std::size_t>(index) * static_cast<std::size_t>(m_BytesBetweenElements));
        return asElement;
    }

private:
    ElementType* m_FirstElementPtr = nullptr;
    SizeType     m_BytesBetweenElements = 0;
    SizeType     m_NumElements = 0;
};

// helper functions for make strided view
template <typename ElementType>
StridedView<ElementType> MakeStridedView(int bytesBetweenElements, ElementType* firstElement, int count)
{
    return StridedView<ElementType>(bytesBetweenElements, firstElement, count);
}

template <typename ContainerType, typename ElementType, typename StructType>
StridedView<ElementType> MakeStridedView(ContainerType& container, ElementType StructType::* member)
{
    if (container.size() == 0)
    {
        return StridedView<ElementType>();
    }
    auto* data = &container[0];
    return StridedView<ElementType>(sizeof(StructType), &(data->*member), static_cast<int32_t>(container.size()));
}

template <typename ContainerType>
auto MakeStridedView(ContainerType& container) -> StridedView<typename std::remove_reference<decltype(container[0])>::type>
{
    using ElementType = typename std::remove_reference<decltype(container[0])>::type;
    if (container.size() == 0)
    {
        return StridedView<ElementType>();
    }
    return StridedView<ElementType>(sizeof(ElementType), &container[0], static_cast<int32_t>(container.size()));
}

// for const strided view
template <typename ElementType>
StridedView<const ElementType> MakeConstStridedView(int bytesBetweenElements, const ElementType* firstElement, int count)
{
    return StridedView<const ElementType>(bytesBetweenElements, firstElement, count);
}

template <typename ContainerType, typename ElementType, typename StructType>
ConstStridedView<ElementType> MakeConstStridedView(ContainerType& container, const ElementType StructType::* member)
{
    if (container.size() == 0)
    {
        return ConstStridedView<ElementType>();
    }
    auto* data = &container[0];
    return ConstStridedView<ElementType>(sizeof(StructType), &(data->*member), static_cast<int32_t>(container.size()));
}

template <typename ContainerType>
auto MakeConstStridedView(const ContainerType& container) -> ConstStridedView<typename std::remove_reference<decltype(container[0])>::type>
{
    using ElementType = typename std::remove_reference<decltype(container[0])>::type;
    if (container.size() == 0)
    {
        return ConstStridedView<ElementType>();
    }
    return ConstStridedView<ElementType>(sizeof(ElementType), &container[0], static_cast<int32_t>(container.size()));
}
