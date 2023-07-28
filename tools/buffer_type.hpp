#pragma once

#include <deque>

template<typename Type>
class BufferType
{
    const size_t max_size_;
    size_t processed_size_;
    std::deque<Type&> buff_;

public:

    BufferType();
    BufferType(size_t max_size);
    BufferType(const BufferType<Type>& buffer);
    BufferType(BufferType<Type>&& buffer);

    // template<typename NewType>
    // BufferType<NewType>& operator=(const BufferType<NewType>& buffer);
    // template<typenmae NewType>
    // BufferType<NewType>& operator=(BufferType<NewType>&& buffer);

    void add(const Type& new_elem);

    size_t buff_size() const;

    size_t processed_size() const;

    Type& back() const;

    Type& operator[](size_t index) const;

    void clear();
};
