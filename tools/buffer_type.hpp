#pragma once

#include <deque>

template<typename Type>
class BufferType
{
    const size_t max_size_;
    size_t processed_size_;
    std::deque<Type&> buff_;

public:

    BufferType(): max_size_(1), processed_size_(0) {}
    BufferType(size_t max_size): max_size_(max_size), processed_size_(0) {}

    void add(const Type& new_elem)
    {
        if (buff_.size() == max_size_)
        {
            buff_.pop_front();
        }
        ++processed_size_;
        buff_.push_back(new_elem);
    }

    size_t buff_size() const
    {
        return buff_.size();
    }

    size_t processed_size() const
    {
        return processed_size_;
    }

    Type& back() const 
    {
        return buff_.back();
    }

    Type& operator[](size_t index) const
    {
        return buff_[index + processed_size_ - max_size_];
    }

    void clear()
    {
        buff_.clear();
    }
};