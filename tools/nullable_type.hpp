#pragma once

#include <exception>

template<typename Type>
class NullableType
{
    public:
    
    NullableType();
    NullableType(const Type& val);
    NullableType(Type&& val);
    NullableType(const NullableType<Type>& nullable_type);
    NullableType(NullableType<Type>&& nullable_type);

    NullableType& operator=(const Type& val);
    NullableType& operator=(Type&& val);
    NullableType& operator=(const NullableType<Type>& nullable_type);
    NullableType& operator=(NullableType<Type>&& nullable_type);

    bool isNull() const;
    void makeNull();
    void setVal(const Type& val);
    void setVal(Type&& val);
    const Type& getVal() const;

    ~NullableType();

    private:

    Type* val_ptr_;
};

template<typename Type>
NullableType<Type>::NullableType(): val_ptr_(nullptr) {}

template<typename Type>
NullableType<Type>::NullableType(const Type& val): val_ptr_(new Type(val)) {}

template<typename Type>
NullableType<Type>::NullableType(Type&& val): val_ptr_(new Type(val)) {}

template<typename Type>
NullableType<Type>::NullableType(const NullableType<Type>& nullable_type): val_ptr_(nullptr)
{
    if (!nullable_type.isNull())
    {
        val_ptr_ = new Type(*nullable_type.val_ptr_);
    }
}

template<typename Type>
NullableType<Type>::NullableType(NullableType<Type>&& nullable_type): val_ptr_(nullptr) 
{
    if (!nullable_type.isNull())
    {
        val_ptr_ = new Type(*nullable_type.val_ptr_);
    }
}

template<typename Type>
NullableType<Type>& NullableType<Type>::operator=(const Type& val)
{
    makeNull();
    val_ptr_ = new Type(val);
    return *this;
}

template<typename Type>
NullableType<Type>& NullableType<Type>::operator=(Type&& val)
{
    makeNull();
    val_ptr_ = new Type(val);
    return *this;
}

template<typename Type>
NullableType<Type>& NullableType<Type>::operator=(const NullableType<Type>& nullable_type)
{   
    if (this == &nullable_type)
    {
        return *this;
    }
    makeNull();
    if (!nullable_type.isNull())
    {
        val_ptr_ = new Type(*nullable_type.val_ptr_);
    }
    return *this;
}

template<typename Type>
NullableType<Type>& NullableType<Type>::operator=(NullableType<Type>&& nullable_type)
{
    if (this == &nullable_type)
    {
        return *this;
    }
    makeNull();
    if (!nullable_type.isNull())
    {
        val_ptr_ = new Type(*nullable_type.val_ptr_);
    }
    return *this;
}

template<typename Type>
bool NullableType<Type>::isNull() const
{
    if (val_ptr_)
    {
        return false;
    }
    return true;
}

template<typename Type>
void NullableType<Type>::makeNull()
{
    if (!isNull())
    {
        delete val_ptr_;
        val_ptr_ = nullptr;
    }
}

template<typename Type>
void NullableType<Type>::setVal(const Type& val)
{
    makeNull();
    val_ptr_ = new Type(val);
}

template<typename Type>
void NullableType<Type>::setVal(Type&& val)
{
    makeNull();
    val_ptr_ = new Type(val);
}

template<typename Type>
const Type& NullableType<Type>::getVal() const
{
    if (isNull())
    {
        throw std::exception();
        // TODO throw ... 
    }
    return *val_ptr_;
}

template<typename Type>
NullableType<Type>::~NullableType()
{
    delete val_ptr_;
}