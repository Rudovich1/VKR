#pragma once

#include "codegen.hpp"

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <sstream>

namespace GeneticAlgorithm
{
    namespace Codegen
    {
        class Node: public Code
        {
            public:
            std::string name_;
            std::string gene_type_;
            std::optional<std::pair<std::string, std::vector<std::string>>> fitness_function_;
            std::optional<std::pair<std::string, std::vector<std::string>>> selection_function_;
            std::optional<std::pair<std::string, std::vector<std::string>>> mutation_function_;
            std::optional<std::pair<std::string, std::vector<std::string>>> crossingover_function_;
            std::optional<std::pair<std::string, std::vector<std::string>>> conditionsForStopping_function_;
            std::optional<std::pair<std::string, std::vector<std::string>>> any_function_;
            std::optional<std::pair<std::string, std::vector<std::string>>> end_node_;
            std::optional<std::pair<std::string, std::vector<std::string>>> start_node_;
            std::optional<std::pair<std::string, std::vector<std::string>>> startNode_log_;
            std::optional<std::pair<std::string, std::vector<std::string>>> newGeneration_log_;
            std::optional<std::pair<std::string, std::vector<std::string>>> endNode_log_;

            virtual std::string functionsGen_() const
            {
                std::stringstream ss;
                if (fitness_function_.has_value())
                {
                    ss << name_ << ".setFitnessFunction(" << fitness_function_->first << "()(";
                    if (fitness_function_->second.size() > 0)
                    {
                        for (size_t i = 0; i < fitness_function_->second.size() - 1; ++i)
                        {
                            ss << fitness_function_->second[i] << ", ";
                        }
                        ss << fitness_function_->second.back();
                    }
                    ss << "));\n";
                }

                if (selection_function_.has_value())
                {
                    ss << name_ << ".setSelectionFunction(" << selection_function_->first << "()(";
                    if (selection_function_->second.size() > 0)
                    {
                        for (size_t i = 0; i < selection_function_->second.size() - 1; ++i)
                        {
                            ss << selection_function_->second[i] << ", ";
                        }
                        ss << selection_function_->second.back();
                    }
                    ss << "));\n";
                }
                
                if (mutation_function_.has_value())
                {
                    ss << name_ << ".setMutationFunction(" << mutation_function_->first << "()(";
                    if (mutation_function_->second.size() > 0)
                    {
                        for (size_t i = 0; i < mutation_function_->second.size() - 1; ++i)
                        {
                            ss << mutation_function_->second[i] << ", ";
                        }
                        ss << mutation_function_->second.back();
                    }
                    ss << "));\n";
                }
                
                if (crossingover_function_.has_value())
                {
                    ss << name_ << ".setCrossingoverFunction(" << crossingover_function_->first << "()(";
                    if (crossingover_function_->second.size() > 0)
                    {
                        for (size_t i = 0; i < crossingover_function_->second.size() - 1; ++i)
                        {
                            ss << crossingover_function_->second[i] << ", ";
                        }
                        ss << crossingover_function_->second.back();
                    }
                    ss << "));\n";
                }
                
                if (conditionsForStopping_function_.has_value())
                {
                    ss << name_ << ".setConditionsForStoppingFunction(" << conditionsForStopping_function_->first << "()(";
                    if (conditionsForStopping_function_->second.size() > 0)
                    {
                        for (size_t i = 0; i < conditionsForStopping_function_->second.size() - 1; ++i)
                        {
                            ss << conditionsForStopping_function_->second[i] << ", ";
                        }
                        ss << conditionsForStopping_function_->second.back();
                    }
                    ss << "));\n";
                }
                
                if (any_function_.has_value())
                {
                    ss << name_ << ".setAnyFunction(" << any_function_->first << "()(";
                    if (any_function_->second.size() > 0)
                    {
                        for (size_t i = 0; i < any_function_->second.size() - 1; ++i)
                        {
                            ss << any_function_->second[i] << ", ";
                        }
                        ss << any_function_->second.back();
                    }
                    ss << "));\n";
                }
                
                if (end_node_.has_value())
                {
                    ss << name_ << ".setEndNode(" << end_node_->first << "()(";
                    if (end_node_->second.size() > 0)
                    {
                        for (size_t i = 0; i < end_node_->second.size() - 1; ++i)
                        {
                            ss << end_node_->second[i] << ", ";
                        }
                        ss << end_node_->second.back();
                    }
                    ss << "));\n";
                }
                
                if (start_node_.has_value())
                {
                    ss << name_ << ".setStartNode(" << start_node_->first << "()(";
                    if (start_node_->second.size() > 0)
                    {
                        for (size_t i = 0; i < start_node_->second.size() - 1; ++i)
                        {
                            ss << start_node_->second[i] << ", ";
                        }
                        ss << start_node_->second.back();
                    }
                    ss << "));\n";
                }
                
                if (startNode_log_.has_value())
                {
                    ss << name_ << ".setStartNodeLog(" << startNode_log_->first << "()(";
                    if (startNode_log_->second.size() > 0)
                    {
                        for (size_t i = 0; i < startNode_log_->second.size() - 1; ++i)
                        {
                            ss << startNode_log_->second[i] << ", ";
                        }
                        ss << startNode_log_->second.back();
                    }
                    ss << "));\n";
                }
                
                if (newGeneration_log_.has_value())
                {
                    ss << name_ << ".setNewGenerationLog(" << newGeneration_log_->first << "()(";
                    if (newGeneration_log_->second.size() > 0)
                    {
                        for (size_t i = 0; i < newGeneration_log_->second.size() - 1; ++i)
                        {
                            ss << newGeneration_log_->second[i] << ", ";
                        }
                        ss << newGeneration_log_->second.back();
                    }
                    ss << "));\n";
                }
                
                if (endNode_log_.has_value())
                {
                    ss << name_ << ".setEndNodeLog(" << endNode_log_->first << "()(";
                    if (endNode_log_->second.size() > 0)
                    {
                        for (size_t i = 0; i < endNode_log_->second.size() - 1; ++i)
                        {
                            ss << endNode_log_->second[i] << ", ";
                        }
                        ss << endNode_log_->second.back();
                    }
                    ss << "));\n";
                }
                
                return ss.str(); 
            }

            Node(std::string name, std::string gene_type): name_(std::move(name)), gene_type_(std::move(gene_type)) {}

            void setFitnessFunction(std::string name, std::vector<std::string> args)
            {
                fitness_function_ = {std::move(name), std::move(args)};
            }

            void setSelectionFunction(std::string name, std::vector<std::string> args)
            {
                selection_function_ = {std::move(name), std::move(args)};
            }

            void setMutationFunction(std::string name, std::vector<std::string> args)
            {
                mutation_function_ = {std::move(name), std::move(args)};
            }

            void setCrossingoverFunction(std::string name, std::vector<std::string> args)
            {
                crossingover_function_ = {std::move(name), std::move(args)};
            }

            void setConditionsForStoppingFunction(std::string name, std::vector<std::string> args)
            {
                conditionsForStopping_function_ = {std::move(name), std::move(args)};
            }

            void setAnyFunction(std::string name, std::vector<std::string> args)
            {
                any_function_ = {std::move(name), std::move(args)};
            }

            void setEndNode(std::string name, std::vector<std::string> args)
            {
                end_node_ = {std::move(name), std::move(args)};
            }

            void setStartNode(std::string name, std::vector<std::string> args)
            {
                start_node_ = {std::move(name), std::move(args)};
            }

            void setStartNodeLog(std::string name, std::vector<std::string> args)
            {
                startNode_log_ = {std::move(name), std::move(args)};
            }

            void setNewGenerationLog(std::string name, std::vector<std::string> args)
            {
                newGeneration_log_ = {std::move(name), std::move(args)};
            }

            void setEndNodeLog(std::string name, std::vector<std::string> args)
            {
                endNode_log_ = {std::move(name), std::move(args)};
            }
        };

        class UnaryNode: public Node
        {
            public:
            std::unique_ptr<Node> next_node_ptr_;

            UnaryNode(std::string name, std::string gene_type, std::unique_ptr<Node> next_node_ptr): 
                Node(std::move(name), std::move(gene_type)), next_node_ptr_(std::move(next_node_ptr)) {}

            virtual std::string gen() const override
            {
                std::stringstream ss;
                ss << next_node_ptr_->gen() << '\n';
                ss << "std::unique_ptr<GeneticAlgorithm::EvolutionTree::UnaryNode<" << gene_type_ << ">> " << name_ 
                << " = std::make_unique<GeneticAlgorithm::EvolutionTree::UnaryNode<" << gene_type_  << ">>(\"" << name_ 
                << "\", std::move(" << next_node_ptr_->name_ << "));\n";
                ss << functionsGen_();
                return ss.str();
            }
        };

        class K_Node: public Node
        {
            public:
            std::vector<std::unique_ptr<Node>> next_node_ptrs_;
            std::optional<std::pair<std::string, std::vector<std::string>>> pooling_populations_;

            K_Node(std::string name, std::string gene_type, std::vector<std::unique_ptr<Node>> next_node_ptrs):
                Node(std::move(name), std::move(gene_type)), next_node_ptrs_(std::move(next_node_ptrs)) {}

            void setPoolingPopulations(std::string name, std::vector<std::string> args)
            {
                pooling_populations_ = {std::move(name), std::move(args)};
            }

            virtual std::string functionsGen_() const override
            {
                std::stringstream ss;
                ss << Node::functionsGen_();
                if (pooling_populations_.has_value())
                {
                    ss << name_ << ".setPoolingPopulations(" << pooling_populations_->first << "()(";
                    if (pooling_populations_->second.size() > 0)
                    {
                        for (size_t i = 0; i < pooling_populations_->second.size() - 1; ++i)
                        {
                            ss << pooling_populations_->second[i] << ", ";
                        }
                        ss << pooling_populations_->second.back();
                    }
                    ss << "));\n";
                }
                return ss.str();
            }

            virtual std::string gen() const override
            {
                std::stringstream ss;
                for (const auto& node: next_node_ptrs_)
                {
                    ss << node->gen();
                }

                ss << "std::array<std::unique_ptr<GeneticAlgorithm::EvolutionTree::Node<" << gene_type_ << ">>, " 
                    << std::to_string(next_node_ptrs_.size()) << "> NODES_FOR_" << name_ << " = {";
                for (size_t i = 0; i < next_node_ptrs_.size() - 1; ++i)
                {
                    ss << "std::move(" << next_node_ptrs_[i]->name_ << "), ";
                }
                ss << "std::move(" << next_node_ptrs_.back()->name_ << ")};\n";
                ss << "std::unique_ptr<GeneticAlgorithm::EvolutionTree::K_Node<" << gene_type_ << ", " + std::to_string(next_node_ptrs_.size()) << ">> "
                    << name_ << " = std::make_unique<GeneticAlgorithm::EvolutionTree::K_Node<"
                    << gene_type_ << ", " + std::to_string(next_node_ptrs_.size()) << ">>(" << name_ << ", std::move(" << "NODES_FOR_" << name_ << "));\n";
                ss << functionsGen_();
                return ss.str();
            }
        };

        class PopulationNode: public Node
        {
            public:
            std::optional<std::pair<std::string, std::vector<std::string>>> startPopulation_function_;

            PopulationNode(std::string name, std::string gene_type): Node(std::move(name), std::move(gene_type)) {}

            void setStartPopulationFunction(std::string name, std::vector<std::string> args)
            {
                startPopulation_function_ = {std::move(name), std::move(args)};
            }

            virtual std::string functionsGen_() const override
            {
                std::stringstream ss;
                ss << Node::functionsGen_();
                if (startPopulation_function_.has_value())
                {
                    ss << name_ << ".setStartPopulationFunction(" << startPopulation_function_->first << "()(";
                    if (startPopulation_function_->second.size() > 0)
                    {
                        for (size_t i = 0; i < startPopulation_function_->second.size() - 1; ++i)
                        {
                            ss << startPopulation_function_->second[i] << ", ";
                        }
                        ss << startPopulation_function_->second.back();
                    }
                    ss << "));\n";
                }
                return ss.str();
            }
            
            virtual std::string gen() const override
            {
                std::stringstream ss;

                ss << "std::unique_ptr<GeneticAlgorithm::EvolutionTree::PopulationNode<" << gene_type_ << ">> " << name_ 
                    << " = std::make_unique<GeneticAlgorithm::EvolutionTree::PopulationNode<" << name_ << ">>(" << name_ << ");\n";
                ss << functionsGen_();
                return ss.str();
            }
        };
    }
}