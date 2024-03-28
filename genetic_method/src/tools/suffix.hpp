#pragma once

#include <array>
#include <vector>
#include <stdexcept>

namespace GeneticAlgorithm
{
    template<class Type>
    class Suffix
    {
        void __zero_capacity_error() const 
        {if (capacity_ == 0) {std::__throw_logic_error("capacity must be greater than 0");}}

        void __smaller_container_error() const
        {if (suffix_data_.size() > capacity_) {std::__throw_logic_error("capacity must be greater then vector size");}}

        void __invalid_start_ind_error() const
        {if (start_ >= suffix_data_.size()) {std::__throw_logic_error("start index must be smoller then vector size");}}

        void __constructor_error() const
        {__zero_capacity_error(); __smaller_container_error(); __invalid_start_ind_error();}

    public:
        using SuffixData_ = std::vector<Type>;
        using Suffix_ = Suffix<Type>;

        Suffix(): size_(0), start_(0), capacity_(1), suffix_data_(1) {}
        Suffix(size_t capacity): size_(0), start_(0), capacity_(capacity), suffix_data_(capacity) 
        {__constructor_error();}

        Suffix(const SuffixData_& suff_data, size_t capacity, size_t start = 0): size_(suff_data.size()), start_(start), capacity_(capacity), suffix_data_(suff_data) 
        {__constructor_error();}
        Suffix(SuffixData_&& suff_data, size_t capacity, size_t start = 0): size_(suff_data.size()), start_(start), capacity_(capacity), suffix_data_(std::move(suff_data)) 
        {__constructor_error();}

        Suffix(const Suffix_& suff): size_(suff.size_), start_(suff.start_), capacity_(suff.capacity_), suffix_data_(suff.suffix_data_) {}
        Suffix_& operator=(const Suffix_& suff)
        {
            if (this != &suff) 
            {
                size_ = suff.size_;
                start_ = suff.start_;
                capacity_ = suff.capacity_;
                suffix_data_ = suff.suffix_data_;
            }
            return *this;
        }
        Suffix(Suffix_&& suff): size_(suff.size_), start_(suff.start_), capacity_(suff.capacity_), suffix_data_(std::move(suff.suffix_data_)) {}
        Suffix_& operator=(Suffix_&& suff)
        {
            if (this != &suff)
            {
                size_ = suff.size_;
                start_ = suff.start_;
                capacity_ = suff.capacity_;
                suffix_data_ = std::move(suff.suffix_data_);
            }
            return *this;
        }

        Type& operator[](size_t ind)
        {
            if (ind >= capacity_ || ind >= size_) {std::__throw_out_of_range("Suffix");}
            size_t shift = std::min(capacity_, size_);
            return suffix_data_[(capacity_ + start_ + shift - 1 - ind) % capacity_];
        }

        const Type& operator[](size_t ind) const
        {
            if (ind >= capacity_ || ind >= size_) {std::__throw_out_of_range("Suffix");}
            size_t shift = std::min(capacity_, size_);
            return suffix_data_[(capacity_ + start_ + shift - 1 - ind) % capacity_];
        }

        void push(const Type& item)
        {
            size_t shift = std::min(capacity_, size_);
            suffix_data_[(start_ + shift) % capacity_] = item;
            if (shift == capacity_) {(++start_) %= capacity_;}
            ++size_;
        }

        size_t size() const {return size_;}
        size_t capacity() const {return capacity_;}
        size_t start() const {return start_;}

    protected:
        size_t capacity_;
        size_t size_;
        size_t start_;
        SuffixData_ suffix_data_;
    };


    template<class Type, size_t capacity_>
    class StatSuffix
    {
        constexpr void __zero_capacity_error() const 
        {if (capacity_ == 0) {std::__throw_logic_error("capacity must be greater than 0");}}

        void __invalid_start_ind_error() const
        {if (start_ >= capacity_) {std::__throw_logic_error("start index must be smoller then capacity");}}

        void __constructor_error() const
        {__zero_capacity_error(); __invalid_start_ind_error();}

    public:
        using StatSuffixData_ = std::array<Type, capacity_>;
        using StatSuffix_ = StatSuffix<Type, capacity_>;

        StatSuffix(): size_(0), start_(0) {}

        StatSuffix(const StatSuffixData_& stat_suff_data, size_t start = 0): size_(capacity_), start_(start), stat_suffix_data_(stat_suff_data) 
        {__constructor_error();}
        StatSuffix(StatSuffixData_&& stat_suff_data, size_t start = 0): size_(capacity_), start_(start), stat_suffix_data_(std::move(stat_suff_data)) 
        {__constructor_error();}

        StatSuffix(const StatSuffix_& stat_suff): size_(stat_suff.size_), start_(stat_suff.start_), stat_suffix_data_(stat_suff.stat_suffix_data_) {}
        StatSuffix_& operator=(const StatSuffix_& stat_suff)
        {
            if (this != &stat_suff)
            {
                size_ = stat_suff.size_;
                start_ = stat_suff.start_;
                stat_suffix_data_ = stat_suff.stat_suffix_data_;
            }
            return *this;
        }
        StatSuffix(StatSuffix_&& stat_suff): size_(stat_suff.size_), start_(stat_suff.start_), stat_suffix_data_(std::move(stat_suff.stat_suffix_data_)) {}
        StatSuffix_& operator=(StatSuffix_&& stat_suff)
        {
            if (this != &stat_suff)
            {
                size_ = stat_suff.size_;
                start_ = stat_suff.start_;
                stat_suffix_data_ = std::move(stat_suff.stat_suffix_data_);
            }
            return *this;
        }

        Type& operator[](size_t ind)
        {
            if (ind >= capacity_ || ind >= size_) {std::__throw_out_of_range("StatSuffix");}
            size_t shift = std::min(capacity_, size_);
            return stat_suffix_data_[(capacity_ + start_ + shift - 1 - ind) % capacity_];
        }

        const Type& operator[](size_t ind) const
        {
            if (ind >= capacity_ || ind >= size_) {std::__throw_out_of_range("Suffix");}
            size_t shift = std::min(capacity_, size_);
            return stat_suffix_data_[(capacity_ + start_ + shift - 1 - ind) % capacity_];
        }

        void push_back(const Type& item)
        {
            size_t shift = std::min(capacity_, size_);
            stat_suffix_data_[(start_ + shift) % capacity_] = item;
            if (shift == capacity_) {(++start_) %= capacity_;}
            ++size_;
        }

        size_t size() const {return size_;}
        size_t capacity() const {return capacity_;}
        size_t start() const {return start_;}

    protected:
        size_t size_;
        size_t start_;
        StatSuffixData_ stat_suffix_data_;
    };
}