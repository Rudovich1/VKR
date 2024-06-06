#pragma once

#include "../algorithm.hpp"

#include <string>
#include <vector>

namespace HeuristicAlgorithm
{
    namespace Graph
    {
        namespace Interfaces
        {
            template<class InputType>
            struct StartNodeLogWrapper : public Wrapper<void, InputType&, const std::string &>
            {
            };

            template<class OutputType>
            struct EndNodeLogWrapper : public Wrapper<void, OutputType&, const std::string &>
            {
            };

            template <class InputType>
            struct StartNodeFunctionWrapper : public Wrapper<void, InputType&>
            {
            };

            template<class OutputType>
            struct EndNodeFunctionWrapper : public Wrapper<void, OutputType&>
            {
            };

            template<class SerializationType, class InputType>
            struct DeserializationWrapper: public Wrapper<InputType, std::vector<SerializationType>&>
            {
            };

            template<class SerializationType, class OutputType>
            struct SerializationWrapper: public Wrapper<SerializationType, OutputType&>
            {
            };
        }
    }
}