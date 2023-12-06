#pragma once

#include <unordered_set>
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <stdexcept>
#include <filesystem>
#include <memory>

namespace GeneticAlgorithm
{
    namespace Codegen
    {
        class Code
        {
            protected:
            static inline const std::string NSP = "GeneticAlgorithm::Interfaces::Types::";

            public:
            virtual std::string gen() const = 0;
        };

        namespace Functions
        {
            class FunctionCode: public Code
            {
            public:

                std::string function_name_;
                std::string gene_type_name_;
                std::vector<std::pair<std::string, std::string>> const_ref_types_;
                std::vector<std::pair<std::string, std::string>> const_types_;
                std::vector<std::pair<std::string, std::string>> ref_types_;
                std::vector<std::pair<std::string, std::string>> types_;
                std::string code_;

                std::string function_wrapper_name_;
                std::string z_function_return_type_name_;
                std::string z_function_arg_type_;
                std::string z_function_arg_name_;

                std::string template_args_;
                std::string function_args_;
                std::string z_function_args_;

                virtual std::string genTemplateArgs_() const
                {
                    std::stringstream template_args;
                    template_args << gene_type_name_;
                    for (const auto& type: const_ref_types_)
                    {
                        template_args << ", const " << type.first << "&";
                    }
                    for (const auto& type: const_types_)
                    {
                        template_args << ", const " << type.first;
                    }
                    for (const auto& type: ref_types_)
                    {
                        template_args << ", " << type.first << "&";
                    }
                    for (const auto& type: types_)
                    {
                        template_args << ", " << type.first;
                    }
                    return template_args.str();
                }

                virtual std::string genFunctionArgs_() const
                {
                    std::stringstream function_args;
                    bool first = true;
                    for (const auto& type: const_ref_types_)
                    {
                        if (!first) {function_args << ", ";}
                        function_args << "const " << type.first << "& " << type.second;
                        first = false;
                    }
                    for (const auto& type: const_types_)
                    {
                        if (!first) {function_args << ", ";}
                        function_args << "const " << type.first << " " << type.second;
                        first = false;
                    }
                    for (const auto& type: ref_types_)
                    {
                        if (!first) {function_args << ", ";}
                        function_args << type.first << "& " << type.second;
                        first = false;
                    }
                    for (const auto& type: types_)
                    {
                        if (!first) {function_args << ", ";}
                        function_args << type.first << " " << type.second;
                        first = false;
                    }
                    return function_args.str();
                }
            
                virtual std::string genZFunctionArgs_() const
                {
                    std::stringstream z_function_args;
                    bool first = true;
                    for (const auto& type: const_ref_types_)
                    {
                        if (!first) {z_function_args << ", ";}
                        z_function_args << "&" << type.second;
                        first = false;
                    }
                    for (const auto& type: const_types_)
                    {
                        if (!first) {z_function_args << ", ";}
                        z_function_args << type.second;
                        first = false;
                    }
                    for (const auto& type: ref_types_)
                    {
                        if (!first) {z_function_args << ", ";}
                        z_function_args << "&" << type.second;
                        first = false;
                    }
                    for (const auto& type: types_)
                    {
                        if (!first) {z_function_args << ", ";}
                        z_function_args << type.second;
                        first = false;
                    }
                    return z_function_args.str();
                }

                virtual std::string functionCode_() const
                {
                    std::stringstream function_code;
                    function_code << "struct "<< function_name_ << ": public "
                    << function_wrapper_name_
                    << "<" << template_args_ << ">{\n";
                    
                    function_code << "virtual std::function<"
                    << z_function_return_type_name_ 
                    << z_function_arg_type_
                    <<"> operator()(" << function_args_ << ") const override{\n";

                    function_code << "return [" << z_function_args_ << "]"
                    << "(" << z_function_arg_type_ << " " << z_function_arg_name_ << "){\n";

                    function_code << code_ << "\n};\n}\n};\n";
                    return function_code.str();
                }

                FunctionCode(
                    std::string function_name,
                    std::string gene_type_name,
                    std::vector<std::pair<std::string, std::string>> const_ref_types,
                    std::vector<std::pair<std::string, std::string>> const_types,
                    std::vector<std::pair<std::string, std::string>> ref_types,
                    std::vector<std::pair<std::string, std::string>> types,
                    std::string code
                    ):
                    function_name_(std::move(function_name)),
                    gene_type_name_(std::move(gene_type_name)),
                    const_ref_types_(std::move(const_ref_types)),
                    const_types_(std::move(const_types)),
                    ref_types_(std::move(ref_types)),
                    types_(std::move(types)),
                    code_(std::move(code))
                    {
                        template_args_ = genTemplateArgs_();
                        function_args_ = genFunctionArgs_();
                        z_function_args_ = genZFunctionArgs_(); 
                    }

                virtual std::string gen() const override
                {
                    return functionCode_();
                }
            };

            class FitnessFunctionCode: public FunctionCode
            {
            public:
                FitnessFunctionCode(
                    std::string function_name,
                    std::string gene_type_name,
                    std::vector<std::pair<std::string, std::string>> const_ref_types,
                    std::vector<std::pair<std::string, std::string>> const_types,
                    std::vector<std::pair<std::string, std::string>> ref_types,
                    std::vector<std::pair<std::string, std::string>> types,
                    std::string code
                    ):
                    FunctionCode(function_name, gene_type_name, const_ref_types, const_types, ref_types, types, code)
                {
                    function_wrapper_name_ = "FitnessFunctionWrapper";
                    z_function_return_type_name_ = "double";
                    z_function_arg_type_ = "const " + NSP + "Chromosome<" + gene_type_name_ + ">&";
                    z_function_arg_name_ = "chromosome";
                }
            };

            class StartPopulationFunctionCode: public FunctionCode
            {
            public:
                StartPopulationFunctionCode(
                    std::string function_name,
                    std::string gene_type_name,
                    std::vector<std::pair<std::string, std::string>> const_ref_types,
                    std::vector<std::pair<std::string, std::string>> const_types,
                    std::vector<std::pair<std::string, std::string>> ref_types,
                    std::vector<std::pair<std::string, std::string>> types,
                    std::string code
                    ):
                    FunctionCode(function_name, gene_type_name, const_ref_types, const_types, ref_types, types, code)
                {
                    function_wrapper_name_ = "StartPopulationFunctionWrapper";
                    z_function_return_type_name_ = NSP + "Population<" + gene_type_name_ + ">";
                    z_function_arg_type_ = "";
                    z_function_arg_name_ = "";
                }
            };

            class SelectionFunctionCode: public FunctionCode
            {
            public:
                SelectionFunctionCode(
                    std::string function_name,
                    std::string gene_type_name,
                    std::vector<std::pair<std::string, std::string>> const_ref_types,
                    std::vector<std::pair<std::string, std::string>> const_types,
                    std::vector<std::pair<std::string, std::string>> ref_types,
                    std::vector<std::pair<std::string, std::string>> types,
                    std::string code
                    ):
                    FunctionCode(function_name, gene_type_name, const_ref_types, const_types, ref_types, types, code)
                {
                    function_wrapper_name_ = "SelectionFunctionWrapper";
                    z_function_return_type_name_ = NSP + "Generation<" + gene_type_name_ + ">";
                    z_function_arg_type_ = NSP + "Generation<" + gene_type_name_ + ">&";
                    z_function_arg_name_ = "generation";
                }
            };

            class ConditionsForStoppingCode: public FunctionCode
            {
            public:
                ConditionsForStoppingCode(
                    std::string function_name,
                    std::string gene_type_name,
                    std::vector<std::pair<std::string, std::string>> const_ref_types,
                    std::vector<std::pair<std::string, std::string>> const_types,
                    std::vector<std::pair<std::string, std::string>> ref_types,
                    std::vector<std::pair<std::string, std::string>> types,
                    std::string code
                    ):
                    FunctionCode(function_name, gene_type_name, const_ref_types, const_types, ref_types, types, code)
                {
                    function_wrapper_name_ = "ConditionsForStoppingWrapper";
                    z_function_return_type_name_ = "bool";
                    z_function_arg_type_ = "const " + NSP + "Population<" + gene_type_name_ + ">&";
                    z_function_arg_name_ = "population";
                }
            };

            class PoolingPopulationsCode: public FunctionCode
            {
            protected:
                std::string num_populations_;

                virtual std::string genTemplateArgs_() const override
                {
                    std::stringstream template_args;
                    template_args << gene_type_name_ << ", " << num_populations_;
                    for (const auto& type: const_ref_types_)
                    {
                        template_args << ", const " << type.first << "&";
                    }
                    for (const auto& type: const_types_)
                    {
                        template_args << ", const " << type.first;
                    }
                    for (const auto& type: ref_types_)
                    {
                        template_args << ", " << type.first << "&";
                    }
                    for (const auto& type: types_)
                    {
                        template_args << ", " << type.first;
                    }
                    return template_args.str();
                }


            public:
                PoolingPopulationsCode(
                    std::string function_name,
                    std::string gene_type_name,
                    size_t num_populations,
                    std::vector<std::pair<std::string, std::string>> const_ref_types,
                    std::vector<std::pair<std::string, std::string>> const_types,
                    std::vector<std::pair<std::string, std::string>> ref_types,
                    std::vector<std::pair<std::string, std::string>> types,
                    std::string code
                    ):
                    num_populations_(std::to_string(num_populations)),
                    FunctionCode(function_name, gene_type_name, const_ref_types, const_types, ref_types, types, code)
                {
                    function_wrapper_name_ = "PoolingPopulationsWrapper";
                    z_function_return_type_name_ = NSP + "Population<" + gene_type_name_ + ">";
                    z_function_arg_type_ = "std::array<" + NSP + "Population<" + gene_type_name_ + ">, " + num_populations_ + ">&)>";
                    z_function_arg_name_ = "populations";
                }
            };

            class MutationFunctionCode: public FunctionCode
            {
            public:
                MutationFunctionCode(
                    std::string function_name,
                    std::string gene_type_name,
                    std::vector<std::pair<std::string, std::string>> const_ref_types,
                    std::vector<std::pair<std::string, std::string>> const_types,
                    std::vector<std::pair<std::string, std::string>> ref_types,
                    std::vector<std::pair<std::string, std::string>> types,
                    std::string code
                    ):
                    FunctionCode(function_name, gene_type_name, const_ref_types, const_types, ref_types, types, code)
                {
                    function_wrapper_name_ = "MutationFunctionWrapper";
                    z_function_return_type_name_ = "void";
                    z_function_arg_type_ = NSP + "Generation<" + gene_type_name_ + ">&";
                    z_function_arg_name_ = "generation";
                }
            };

            class CrossingoverFunctionCode: public FunctionCode
            {
            public:
                CrossingoverFunctionCode(
                    std::string function_name,
                    std::string gene_type_name,
                    std::vector<std::pair<std::string, std::string>> const_ref_types,
                    std::vector<std::pair<std::string, std::string>> const_types,
                    std::vector<std::pair<std::string, std::string>> ref_types,
                    std::vector<std::pair<std::string, std::string>> types,
                    std::string code
                    ):
                    FunctionCode(function_name, gene_type_name, const_ref_types, const_types, ref_types, types, code)
                {
                    function_wrapper_name_ = "CrossingoverFunctionWrapper";
                    z_function_return_type_name_ = NSP + "Generation<" + gene_type_name_ + ">";
                    z_function_arg_type_ = "const " + NSP + "Population<" + gene_type_name_ + ">&";
                    z_function_arg_name_ = "population";
                }
            };

            class AnyFunctionCode: public FunctionCode
            {
            public:
                AnyFunctionCode(
                    std::string function_name,
                    std::string gene_type_name,
                    std::vector<std::pair<std::string, std::string>> const_ref_types,
                    std::vector<std::pair<std::string, std::string>> const_types,
                    std::vector<std::pair<std::string, std::string>> ref_types,
                    std::vector<std::pair<std::string, std::string>> types,
                    std::string code
                    ):
                    FunctionCode(function_name, gene_type_name, const_ref_types, const_types, ref_types, types, code)
                {
                    function_wrapper_name_ = "AnyFunctionWrapper";
                    z_function_return_type_name_ = "void";
                    z_function_arg_type_ = NSP + "Population<" + gene_type_name_ + ">&";
                    z_function_arg_name_ = "population";
                }
            };

            class EndNodeFunctionCode: public FunctionCode
            {
            public:
                EndNodeFunctionCode(
                    std::string function_name,
                    std::string gene_type_name,
                    std::vector<std::pair<std::string, std::string>> const_ref_types,
                    std::vector<std::pair<std::string, std::string>> const_types,
                    std::vector<std::pair<std::string, std::string>> ref_types,
                    std::vector<std::pair<std::string, std::string>> types,
                    std::string code
                    ):
                    FunctionCode(function_name, gene_type_name, const_ref_types, const_types, ref_types, types, code)
                {
                    function_wrapper_name_ = "EndNodeFunctionWrapper";
                    z_function_return_type_name_ = "void";
                    z_function_arg_type_ = "";
                    z_function_arg_name_ = "";
                }
            };

            class EndNodeFunctionCode: public FunctionCode
            {
            public:
                EndNodeFunctionCode(
                    std::string function_name,
                    std::string gene_type_name,
                    std::vector<std::pair<std::string, std::string>> const_ref_types,
                    std::vector<std::pair<std::string, std::string>> const_types,
                    std::vector<std::pair<std::string, std::string>> ref_types,
                    std::vector<std::pair<std::string, std::string>> types,
                    std::string code
                    ):
                    FunctionCode(function_name, gene_type_name, const_ref_types, const_types, ref_types, types, code)
                {
                    function_wrapper_name_ = "EndNodeFunctionWrapper";
                    z_function_return_type_name_ = "void";
                    z_function_arg_type_ = "";
                    z_function_arg_name_ = "";
                }
            };
        
            class FunctionsCode: public Code
            {
                protected:
                std::vector<std::unique_ptr<FunctionCode>> functions_;
                static inline std::unordered_set<std::string> names_;

                public:
                FunctionsCode& addFunction(std::unique_ptr<FunctionCode> function_ptr)
                {
                    if (names_.find(function_ptr->function_name_) != names_.end())
                        {throw std::logic_error("function \"" + function_ptr->function_name_  + "\" already exists");}
                    names_.insert(function_ptr->function_name_);
                    functions_.push_back(std::move(function_ptr));
                }

                virtual std::string gen() const
                {
                    std::stringstream ss;
                    for (const auto& fun: functions_)
                    {
                        ss << fun->gen() << '\n';
                    }
                    return ss.str();
                }
            };
        }
        // namespace FuntionsCodegen

        namespace Libraries
        {   
            class LibrariesCode: public Code 
            {
                protected:
                std::unordered_set<std::string> stl_libs_;
                std::unordered_set<std::string> user_libs_;

                public:
                LibrariesCode() 
                {
                    user_libs_.insert("src/genetic_algorithm.hpp");
                    stl_libs_.insert("memory");
                }

                LibrariesCode& addStlLib(std::string lib)
                {
                    if (stl_libs_.find(lib) != stl_libs_.end()) 
                        {throw std::logic_error("Library <" + lib + "> already exists");}
                    stl_libs_.insert(std::move(lib));
                    return *this;
                }

                LibrariesCode& addUserLib(std::string lib)
                {
                    if (user_libs_.find(lib) != user_libs_.end()) 
                        {throw std::logic_error("Library \"" + lib + "\" already exists");}
                    user_libs_.insert(std::move(lib));
                    return *this;
                }

                virtual std::string gen() const override
                {
                    std::stringstream ss;
                    for (const auto& lib: stl_libs_)
                    {
                        ss << "<" << lib << ">\n"; 
                    }
                    for (const auto& lib: user_libs_)
                    {
                        ss << "\"" << lib << "\"\n"; 
                    }
                    return ss.str();
                }
            };
        }
        // namespace HeaderCodegen

        namespace Variables
        {
            class VariablesCode: public Code
            {
                protected:
                std::unordered_map<std::string, std::pair<std::string, std::string>> const_vars_;
                std::unordered_map<std::string, std::pair<std::string, std::string>> vars_;
                
                public:
                VariablesCode& addConstVar(std::string name, std::string type, std::string args, bool is_const)
                {
                    if (const_vars_.find(name) != const_vars_.end() || vars_.find(name) != vars_.end())
                        {throw std::logic_error("variable \"" + name + "\" already exists");}
                    if (is_const) {const_vars_[name] = {type, args};}
                    else {vars_[name] = {type, args};}
                    return *this;
                }

                virtual std::string gen() const override
                {
                    std::stringstream ss;
                    for (const auto& var: const_vars_)
                    {
                        ss << "const " << var.first << " " << var.second.first << "(" << var.second.second << ")\n";
                    }
                    for (const auto& var: vars_)
                    {
                        ss << var.first << " " << var.second.first << "(" << var.second.second << ")\n";
                    }
                    return ss.str();
                }
            };
        }
    }
    // namespace Code
}
// namespace GeneticAlgorithm