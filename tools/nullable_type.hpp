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
    Type& getVal() const;

    ~NullableType();

    private:

    Type* val_ptr_;
};