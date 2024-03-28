#include "suffix.hpp"
#include "unit_test.hpp"
#include <stdexcept>


template<class Type>
std::string to_string(Type elem)
{
    return std::to_string(elem);
}

template<>
std::string to_string<const char*>(const char* elem)
{
    return std::string(elem);
}

template<>
std::string to_string<std::string>(std::string elem)
{
    return std::move(elem);
}

template<class Type, class ... Types>
std::string to_string(Type elem, Types ... elems)
{
    return to_string(elem) + to_string(std::move(elems)...);
}

template<class ... Types>
const char* c_str(Types ... elems)
{
    return to_string(std::move(elems)...).c_str();
}


struct TestClass
{
    TestClass() {++num_constructors;}
    
    TestClass(const TestClass& test_class): TestClass() {}
    TestClass& operator=(const TestClass& test_class) {}

    TestClass(TestClass&& test_class): TestClass() {}
    TestClass& operator=(TestClass&& test_class) {}

    ~TestClass() {++num_deconstructors;}

    static inline size_t num_constructors = 0;
    static inline size_t num_deconstructors = 0;

    void clear() {num_constructors = 0; num_deconstructors = 0;}
};


int main()
{
    {
        GeneticAlgorithm::Suffix<size_t> default_constructor_size_t;
        UnitTest::add("default constructor size_t from 0 to 10", [&default_constructor_size_t]()
        {
            for (size_t i = 0; i < 10; ++i)
            {
                default_constructor_size_t.push_back(i);
                if (default_constructor_size_t[0] != i) {std::__throw_length_error(c_str("error in index ", i));}
                // TODO ....
            }
        });
    }
}