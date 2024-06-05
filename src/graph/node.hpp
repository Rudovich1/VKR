#pragma once

#include "../algorithm.hpp"
#include "interfaces.hpp"

#include <memory>
#include <string>
#include <stdexcept>
#include <vector>


namespace HeuristicAlgorithm
{
    namespace Graph
    {
        using namespace Interfaces;

        template<class SerializationType>
        struct BaseNode
        {
            BaseNode(std::string id): id_(std::move(id)) {}
            virtual SerializationType evolution(std::vector<SerializationType>&) = 0;

            template <class Interface>
            void __set_interface_error(const std::shared_ptr<Interface> &interface, std::string interface_name) const
            {
                if (!interface)
                {
                    std::__throw_logic_error(("interface \"" + interface_name + "\" not set in \"" + id_ + "\"").c_str());
                }
            }

            const std::string id_;
        };


        template<class SerializationType, class InputType, class OutputType>
        struct Node: public BaseNode<SerializationType>
        {
            using BaseNode_ = BaseNode<SerializationType>;

            using Deserialization_ = TypeCastWrapper<std::vector<SerializationType>&, InputType>;
            using StartNodeLog_ = StartNodeLogWrapper<InputType>;
            using StartNodeFunction_ = StartNodeFunctionWrapper<InputType>;

            using Algorithm_ = Algorithm<InputType, OutputType>;

            using Serialization_ = TypeCastWrapper<OutputType&, SerializationType>;
            using EndNodeLog_ = EndNodeLogWrapper<OutputType>;
            using EndNodeFunction_ = EndNodeFunctionWrapper<OutputType>;

            Node(std::string id): BaseNode_(std::move(id)) {}

            void setAlgorithm(std::shared_ptr<Algorithm_> alg) { algorithm_ = alg; }
            void setDeserialization(std::shared_ptr<Deserialization_> deserialization) { deserialization_ = deserialization; }
            void setStartNodeLog(std::shared_ptr<StartNodeLog_> fun) { startNodeLog_ = fun; }
            void setStartNodeFunction(std::shared_ptr<StartNodeFunction_> fun) { startNodeFunction_ = fun; }
            void setSerialization(std::shared_ptr<Serialization_> serialization) { serialization_ = serialization; }
            void setEndNodeLog(std::shared_ptr<EndNodeLog_> fun) { endNodeLog_ = fun; }
            void setEndNodeFunction(std::shared_ptr<EndNodeFunction_> fun) { endNodeFunction_ = fun; }

            InputType deserialization(std::vector<SerializationType>& ser_data)
            {
                BaseNode_::__set_interface_error(deserialization_, "Deserialization");
                return (*deserialization_)(ser_data);
            }

            void startNode(InputType& input)
            {
                if (startNodeLog_) { (*startNodeLog_)(input, BaseNode_::id_); }
                if (startNodeFunction_) { (*startNodeFunction_)(input); }
            }

            SerializationType serialization(OutputType& output)
            {
                BaseNode_::__set_interface_error(serialization_, "Serialization");
                return (*serialization_)(output);
            }

            void endNode(OutputType& output)
            {
                if (endNodeFunction_) { (*endNodeFunction_)(output); }
                if (endNodeLog_) { (*endNodeLog_)(output, BaseNode_::id_); }
            }

            virtual SerializationType evolution(std::vector<SerializationType>& ser_data) override
            {
                BaseNode_::__set_interface_error(algorithm_, "Algorithm");
                
                InputType input = deserialization(ser_data);
                startNode(input);
                OutputType output = algorithm_->evolution(input);
                endNode(output);
                return serialization(output);
            }

            std::shared_ptr<Algorithm_> algorithm_;

            std::shared_ptr<Deserialization_> deserialization_;
            std::shared_ptr<StartNodeLog_> startNodeLog_;
            std::shared_ptr<StartNodeFunction_> startNodeFunction_;
            std::shared_ptr<Serialization_> serialization_;
            std::shared_ptr<EndNodeLog_> endNodeLog_;
            std::shared_ptr<EndNodeFunction_> endNodeFunction_;
        };
    }
}