#pragma once

#include <string>
#include <memory>
#include <stdexcept>


namespace HeuristicAlgorithm
{
    template<class InputType, class OutputType>
    struct Algorithm
    {
        virtual OutputType evolution(InputType&) = 0;
    };


    template <class ReturnType, class... Args>
    struct Wrapper
    {
        virtual ReturnType operator()(Args...) = 0;
    };
}