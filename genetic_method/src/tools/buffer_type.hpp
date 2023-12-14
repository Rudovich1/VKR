#pragma once

#include <deque>
#include <cstddef>

template<typename Type>
class BufferType
{
    size_t max_size_;
    size_t processed_size_;
    std::deque<Type> buff_;

public:

    BufferType();
    BufferType(size_t max_size);
    BufferType(const BufferType<Type>& buffer);
    BufferType(BufferType<Type>&& buffer);

    BufferType& operator=(const BufferType& buffer);
    BufferType& operator=(BufferType&& buffer);

    void add(const Type& new_elem);
    size_t buff_size() const;
    size_t processed_size() const;
    Type& back();
    const Type& back() const;
    Type& operator[](size_t index) const;
    void clear();
};

template<typename Type>
BufferType<Type>::BufferType(): max_size_(1), processed_size_(0) {}

template<typename Type>
BufferType<Type>::BufferType(size_t size) : max_size_(size), processed_size_(0) {}

template<typename Type>
BufferType<Type>::BufferType(const BufferType<Type>& buffer): 
    max_size_(buffer.max_size_), processed_size_(buffer.processed_size), buff_(buffer.buff_) {}

template<typename Type>
BufferType<Type>::BufferType(BufferType<Type>&& buffer): 
    max_size_(std::move(buffer.max_size_)), processed_size_(std::move(buffer.processed_size_)), buff_(std::move(buffer.buff_)) {}

template<typename Type>
BufferType<Type>& BufferType<Type>::operator=(const BufferType<Type>& buff)
{
    if (this != &buff)
    {
        max_size_ = buff.max_size_;
        processed_size_ = buff.processed_size_;
        buff_ = buff.buff_;
    }
    return *this;
}

template<typename Type>
BufferType<Type>& BufferType<Type>::operator=(BufferType<Type>&& buff)
{
    if (this != &buff)
    {
        max_size_ = buff.max_size_;
        processed_size_ = buff.processed_size_;
        buff_ = std::move(buff.buff_);
    }
    return *this;
}

template<typename Type>
void BufferType<Type>::add(const Type &new_elem)
{
    if (buff_.size() == max_size_)
    {
        buff_.pop_front();
    }
    ++processed_size_;
    buff_.push_back(new_elem);
}

template<typename Type>
size_t BufferType<Type>::buff_size() const
{
    return buff_.size();
}

template<typename Type>
size_t BufferType<Type>::processed_size() const
{
    return processed_size_;
}

template<typename Type>
Type& BufferType<Type>::back() 
{
    return buff_.back();
}

template<typename Type>
const Type& BufferType<Type>::back() const 
{
    return buff_.back();
}

template<typename Type>
Type& BufferType<Type>::operator[](size_t index) const
{
    return buff_[index];
}

template<typename Type>
void BufferType<Type>::clear()
{
    buff_.clear();
}
