
GA = "GeneticAlgorithm::"
GAI = GA + "Interfaces::"
GAIT = GAI + "Types::"


class Function(object):

    def __init__(
            self,
            name: str,
            gene_type: str,
            const_ref_vars: list[tuple[str, str]],
            const_vars: list[tuple[str, str]],
            ref_vars: list[tuple[str, str]],
            vars: list[tuple[str, str]],
            code: str):
        
        self.name = name
        self.gene_type = gene_type
        self.const_ref_vars = const_ref_vars
        self.const_vars = const_vars
        self.ref_vars = ref_vars
        self.vars = vars
        self.code = code
        self.function_wrapper_type: str = ""
        self.z_function_return_type: str = ""
        self.z_function_arg_types: list[str] = []
        self.z_function_arg_names: list[str] = []

    def gen_template_args_(self) -> str:
        lst = []
        lst.append(self.gene_type)
        lst.extend(map(lambda x: f"const {x[0]}&"), self.const_ref_vars)
        lst.extend(map(lambda x: f"const {x[0]}"), self.const_vars)
        lst.extend(map(lambda x: f"{x[0]}&"), self.ref_vars)
        lst.extend(map(lambda x: f"{x[0]}"), self.vars)
        return ", ".join(lst)
    
    def gen_function_args_(self) -> str:
        lst = []
        lst.extend(map(lambda x: f"const {x[0]}& {x[1]}"), self.const_ref_vars)
        lst.extend(map(lambda x: f"const {x[0]} {x[1]}"), self.const_vars)
        lst.extend(map(lambda x: f"{x[0]}& {x[1]}"), self.ref_vars)
        lst.extend(map(lambda x: f"{x[0]} {x[1]}"), self.vars)
        return ", ".join(lst)
    
    def gen_z_function_сapture_(self) -> str:
        lst = []
        lst.extend(map(lambda x: f"{x[0]}& {x[1]}"), self.const_ref_vars)
        lst.extend(map(lambda x: f"{x[0]} {x[1]}"), self.const_vars)
        lst.extend(map(lambda x: f"{x[0]}& {x[1]}"), self.ref_vars)
        lst.extend(map(lambda x: f"{x[0]} {x[1]}"), self.vars)
        return ", ".join(lst)
    
    def gen_z_function_args_(self) -> str:
        return ", ".join(map(lambda x, y: f"{x} {y}"), self.z_function_arg_types, self.z_function_arg_names)
    
    def gen_function_(self) -> str:
        return f"""\
            struct {self.name}: public {self.function_wrapper_type}<{self.gen_template_args_()}> {{ 
                virtual std::function<{self.z_function_return_type}({self.z_function_arg_types})> \
                operator()({self.gen_function_args_()}) const override {{
                    return [{self.gen_z_function_сapture_()}]({self.gen_z_function_args_()}) {{ 
                        {self.code}
                    }};
                }}
            }};
            """

    def gen(self) -> str:
        return self.gen_function_()


class Fitness(Function):
    
    def __init__(self, **kwargs):
        super().__init__(kwargs)
        self.function_wrapper_type = "FitnessFunctionWrapper"
        self.z_function_return_type = "double"
        self.z_function_arg_types = [f"const {GAIT}Chromosome<{self.gene_type}>&"]
        self.z_function_arg_names = ["chromosome"]


class StartPopulation(Function):
    
    def __init__(self, **kwargs):
        super().__init__(kwargs)
        self.function_wrapper_type = "StartPopulationFunctionWrapper"
        self.z_function_return_type = f"{GAIT}Population<{self.gene_type}>"
        self.z_function_arg_types = []
        self.z_function_arg_names = []


class Selection(Function):
    
    def __init__(self, **kwargs):
        super().__init__(kwargs)
        self.function_wrapper_type = "SelectionFunctionWrapper"
        self.z_function_return_type = f"{GAIT}Generation<{self.gene_type}>"
        self.z_function_arg_types = [f"{GAIT}Generation<{self.gene_type}>&"]
        self.z_function_arg_names = ["generation"]


class ConditionsForStopping(Function):
    
    def __init__(self, **kwargs):
        super().__init__(kwargs)
        self.function_wrapper_type = "ConditionsForStoppingWrapper"
        self.z_function_return_type = "bool"
        self.z_function_arg_types = [f"const {GAIT}Population<{self.gene_type}>&"]
        self.z_function_arg_names = ["population"]


class PoolingPopulations(Function):
    
    def __init__(self, num_populations: int, **kwargs):
        self.num_populations = num_populations
        super().__init__(kwargs)
        self.function_wrapper_type = "PoolingPopulationsWrapper"
        self.z_function_return_type = f"{GAIT}Population<{self.gene_type}>"
        self.z_function_arg_types = [f"std::array<{GAIT}Population<{self.gene_type}>, {self.num_populations}>&"]
        self.z_function_arg_names = ["populations"]

    def gen_template_args_(self) -> str:
        lst = []
        lst.append(self.gene_type)
        lst.append(self.num_populations)
        lst.extend(map(lambda x: f"const {x[0]}&"), self.const_ref_vars)
        lst.extend(map(lambda x: f"const {x[0]}"), self.const_vars)
        lst.extend(map(lambda x: f"{x[0]}&"), self.ref_vars)
        lst.extend(map(lambda x: f"{x[0]}"), self.vars)
        return ", ".join(lst)


class Mutation(Function):
    
    def __init__(self, **kwargs):
        super().__init__(kwargs)
        self.function_wrapper_type = "MutationFunctionWrapper"
        self.z_function_return_type = "void"
        self.z_function_arg_types = [f"{GAIT}Generation<{self.gene_type}>&"]
        self.z_function_arg_names = ["generation"]


class Crossingover(Function):
    
    def __init__(self, **kwargs):
        super().__init__(kwargs)
        self.function_wrapper_type = "CrossingoverFunctionWrapper"
        self.z_function_return_type = f"{GAIT}Generation<{self.gene_type}>"
        self.z_function_arg_types = [f"const {GAIT}Population<{self.gene_type}>&"]
        self.z_function_arg_names = ["population"]


class Any(Function):
    
    def __init__(self, **kwargs):
        super().__init__(kwargs)
        self.function_wrapper_type = "AnyFunctionWrapper"
        self.z_function_return_type = "void"
        self.z_function_arg_types = [f"{GAIT}Population<{self.gene_type}>&"]
        self.z_function_arg_names = ["population"]


class EndNode(Function):
    
    def __init__(self, **kwargs):
        super().__init__(kwargs)
        self.function_wrapper_type = "EndNodeFunctionWrapper"
        self.z_function_return_type = "void"
        self.z_function_arg_types = []
        self.z_function_arg_names = []
        

class StartNode(Function):
    
    def __init__(self, **kwargs):
        super().__init__(kwargs)
        self.function_wrapper_type = "StartNodeFunctionWrapper"
        self.z_function_return_type = "void"
        self.z_function_arg_types = []
        self.z_function_arg_names = []


class NewGenerationLog(Function):
    
    def __init__(self, **kwargs):
        super().__init__(kwargs)
        self.function_wrapper_type = "NewGenerationLogWrapper"
        self.z_function_return_type = "void"
        self.z_function_arg_types = [f"const {GAIT}Generation<{self.gene_type}>&", "const std::string&"]
        self.z_function_arg_names = ["generation", "node_id"]


class StartNodeLog(Function):
    
    def __init__(self, **kwargs):
        super().__init__(kwargs)
        self.function_wrapper_type = "StartNodeLogWrapper"
        self.z_function_return_type = "void"
        self.z_function_arg_types = [f"const {GAIT}Population<{self.gene_type}>&", "const std::string&"]
        self.z_function_arg_names = ["population", "node_id"]


class EndNodeLog(Function):
    
    def __init__(self, **kwargs):
        super().__init__(kwargs)
        self.function_wrapper_type = "EndNodeLogWrapper"
        self.z_function_return_type = "void"
        self.z_function_arg_types = [f"const {GAIT}Population<{self.gene_type}>&", "const std::string&"]
        self.z_function_arg_names = ["population", "node_id"]



class Library(object):

    def __init__(self, name: str, is_stl: bool):
        self.name = name
        self.is_stl = is_stl

    def gen(self) -> str:
        return f"#include {'<' if self.is_stl else '\"'}{self.name}{'<' if self.is_stl else '\"'}\n"
    


class Variable(object):

    def __init__(
            self, 
            type: str,
            name: str,
            args: list[str],
            is_const: bool):
        
        self.type = type
        self.name = name
        self.args = args
        self.is_const = is_const

    def gen(self) -> str:
        return f"{'const' if self.is_const else ''} {self.type} {self.name}({', '.join(self.args)})\n"


class Header(object):

    def __init__(
            self,
            libraries: list[Library], 
            variables: list[Variable],
            functions: list[Function]):
        
        self.libraries = libraries
        self.variables = variables
        self.functions = functions

    def gen(self) -> str:
        res = []
        res.extend(map(lambda x: x.gen(), self.libraries))
        res.extend(map(lambda x: x.gen(), self.variables))
        res.extend(map(lambda x: x.gen(), self.functions))
        return ''.join(res)
