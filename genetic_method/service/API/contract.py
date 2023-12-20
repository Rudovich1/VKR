from pydantic import BaseModel
from enum import Enum


class FunctionTypes(str, Enum):
    fitness = 'fitness',
    start_population = 'start_population',
    selection = 'selection',
    conditions_for_stopping = 'conditions_for_stopping',
    pooling_populations = 'pooling_populations',
    mutation = 'mutation',
    crossingover = 'crossingover',
    any = 'any',
    end_node = 'end_node',
    start_node = 'start_node',
    new_generation_log = 'new_generation_log',
    start_node_log = 'start_node_log',
    end_node_log = 'end_node_log'


class NodeTypes(str, Enum):
    unary_node = 'unary_node'
    K_node = 'K_node'
    population_node = 'population_node'


class Name(BaseModel):
    name: str

class Id(BaseModel):
    id: int


# FunctionParams


class BaseFunctionParam(Name):
    is_const: bool
    is_ref: bool
    type: str

class FunctionParamLinks(BaseFunctionParam):
    function_id: int

class FunctionParamId(BaseFunctionParam, Id):
    pass

class FunctionParam(FunctionParamLinks, FunctionParamId):
    pass


# GlobalVarArg


class BaseGlobalVarArg(BaseModel):
    arg: str

class GlobalVarArgLinks(BaseGlobalVarArg):
    global_var_id: int

class GlobalVarArgId(BaseGlobalVarArg, Id):
    pass

class GlobalVarArg(GlobalVarArgId):
    pass


# Library


class BaseLibrary(Name):
    is_stl: bool

class LibraryLinks(BaseLibrary):
    program_id: int

class LibraryId(BaseLibrary, Id):
    pass

class Library(LibraryLinks, LibraryId):
    pass


# Function_NodeArg


class BaseFunction_NodeArg(BaseModel):
    arg: str

class Function_NodeArgLinks(BaseFunction_NodeArg):
    function_id: int
    node_id: int

class Function_NodeArgId(BaseFunction_NodeArg, Id):
    pass

class Function_NodeArg(Function_NodeArgId, Function_NodeArgLinks):
    pass


# GlobalVar


class BaseGlobalVar(Name):
    is_const: bool
    type: str

class GlobalVarLinks(BaseGlobalVar):
    program_id: int
    arg_ids: list[int] = []

class GlobalVarId(BaseGlobalVar, Id):
    pass

class GlobalVar(GlobalVarLinks, GlobalVarId):
    pass

class GlobalVarRes(BaseGlobalVar):
    args: list[BaseGlobalVarArg]

class GlobalVarWithArgs(BaseGlobalVar):
    args: list[BaseGlobalVarArg]

# Program_Graph


class BaseProgram_Graph(BaseModel):
    pass

class Program_GraphLinks(BaseProgram_Graph):
    program_id: int
    graph_id: int

class Program_GraphId(BaseProgram_Graph, Id):
    pass

class Program_Graph(Program_GraphLinks, Program_GraphId):
    pass


# Function_Node


class BaseFunction_Node(BaseModel):
    pass

class Function_NodeLinks(BaseFunction_Node):
    function_id: int
    node_id: int
    arg_ids: list[int] = []

class Function_NodeId(BaseFunction_Node, Id):
    pass

class Function_Node(Function_NodeLinks, Function_NodeId):
    pass

class Function_NodePostRes(Id):
    arg_ids: list[Id] = []

# Node_Graph


class BaseNode_Graph(Name):
    pass

class Node_GraphLinks(BaseNode_Graph):
    parent_node_graph_id: int | None
    node_id: int
    graph_id: int

class Node_GraphId(BaseNode_Graph, Id):
    pass

class Node_Graph(Node_GraphLinks, Node_GraphId):
    pass

class Node_GraphRes(BaseNode_Graph):
    parent_node_graph_name: str | None
    node_name: str



# Function


class BaseFunction(Name):
    type: FunctionTypes = FunctionTypes.any
    gene_type: str
    code: str

class FunctionLinks(BaseFunction):
    node_ids: list[int] = []
    param_ids: list[int] = []

class FunctionId(BaseFunction, Id):
    pass

class Function(FunctionLinks, FunctionId):
    pass

class FunctionRes(BaseFunction):
    params: list[BaseFunctionParam] = []

class FunctionPostRes(Id):
    param_ids: list[Id] = []

class FunctionWithArgs(FunctionRes):
    args: list[BaseFunction_NodeArg] = []


# Node


class BaseNode(Name):
    type: NodeTypes = NodeTypes.population_node
    gene_type: str

class NodeLinks(BaseNode):
    function_ids: list[int] = []
    graph_ids: list[int] = []

class NodeId(BaseNode, Id):
    pass

class Node(NodeLinks, NodeId):
    pass

class NodeRes(BaseNode):
    functions: list[FunctionWithArgs] = []

# Graph


class BaseGraph(Name):
    pass

class GraphLinks(BaseGraph):
    program_ids: list[int] = []
    node_ids: list[int] = []
    node_graph_ids: list[int] = []

class GraphId(BaseGraph, Id):
    pass

class Graph(GraphLinks, GraphId):
    pass

class GraphRes(BaseGraph):
    node_graphs: list[Node_GraphRes] = []


# Program


class BaseProgram(Name):
    pass

class ProgramLinks(BaseProgram):
    library_ids: list[int] = []
    global_var_ids: list[int] = []
    graph_ids: list[int] = []

class ProgramId(BaseProgram, Id):
    pass

class Program(ProgramLinks, ProgramId):
    pass

class ProgramRes(BaseProgram):
    libraries: list[BaseLibrary] = []
    global_vars: list[GlobalVarRes] = []
    graphs: list[GraphRes] = []