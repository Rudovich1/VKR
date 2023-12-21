from fastapi import HTTPException
from starlette import status
from genetic_method.service.API import contract
from genetic_method.service.db import models, engine
from sqlalchemy.orm import Session
from sqlalchemy import text
from typing import TypeVar, Type


MODEL = TypeVar("MODEL")
MODEL1 = TypeVar("MODEL1")
MODEL2 = TypeVar("MODEL2")
CONTRACT = TypeVar("CONTRACT")
T = TypeVar("T")
T1 = TypeVar("T1")
FIELD = TypeVar("FIELD")
FIELD1 = TypeVar("FIELD1")


def get_session(engine=engine) -> Session:
    with Session(engine) as session:
        session.execute(text("pragma foreign_keys=on"))
        session.commit()
        return session


def not_found_except(item, name: str) -> None:
    if item is None:
        raise HTTPException(
            status.HTTP_404_NOT_FOUND,
            f"{name} not found"
        )
    
def already_exists_except(item, name: str) -> None:
    if item is not None:
        raise HTTPException(
            status.HTTP_400_BAD_REQUEST,
            f"{name} already exists"
        )

def double_none_except(item1, item2) -> None:
    if item1 is None and item2 is None:
        raise HTTPException(
            status.HTTP_400_BAD_REQUEST,
            "one item must by not None"
        )


def to_contract(model: MODEL, contract_type: Type[CONTRACT]) -> CONTRACT:
    return contract_type(**model.to_dict())

def to_model(contract: CONTRACT, model_type: Type[MODEL]) -> MODEL:
    return model_type.make_model(contract.__dict__)

def to_list_contracts(models: list[MODEL], contract_type: Type[CONTRACT]) -> list[CONTRACT]:
    return list(map(lambda x: to_contract(x, contract_type), models))

def to_list_models(contracts: list[CONTRACT], model_type: Type[MODEL]) -> list[MODEL]:
    return list(map(lambda x: to_model(x, model_type), contracts))


def get_model(session: Session, 
              model: int | MODEL | str, 
              model_type: Type[MODEL]) -> MODEL:
    if type(model) == int:
        model = session.query(model_type).get(model)
    if type(model) == str:
        model = session.query(model_type).filter(model_type.name == model).first()
    not_found_except(model, model_type.__tablename__)
    return model

def get_model_id(session: Session,
                 model: int | MODEL | str,
                 model_type: Type[MODEL]) -> int:
    model_id = model
    if type(model) == model_type:
        model_id = model.id
    if type(model) == str:
        model_id = get_model(session, model, model_type).id
    return model_id


# FunctionParam


def get_function_param(session: Session, 
                       param: int | models.FunctionParam, 
                       contract_type: Type[CONTRACT]) -> CONTRACT:
    param = get_model(session, param, models.FunctionParam)
    res = to_contract(param, contract_type)
    return res

def get_function_params(session: Session, 
                        fun: int | models.Function | str, 
                        contract_type: Type[CONTRACT]) -> list[CONTRACT]:
    fun = get_model(session, fun, models.Function)
    res = to_list_contracts(fun._params, contract_type)
    return res

# def post_function_param(session: Session,
#                         fun: int | models.Function,
#                         param: contract.BaseFunctionParam) -> contract.Id:
#     fun = get_model(session, fun, models.Function)
#     nparam = session.query(models.FunctionParam).filter(
#         models.FunctionParam.name == param.name,
#         models.FunctionParam.function_id == fun.id).first()
#     already_exists_except(nparam, "param")
#     param_model = to_model(param, models.FunctionParam)
#     fun._params.append(param_model)
#     session.flush()
#     res = to_contract(param_model, contract.Id)
#     return res

def post_function_params(session: Session,
                         fun: int | models.Function | str, 
                         params: list[contract.BaseFunctionParam]) -> list[contract.Id]:
    fun = get_model(session, fun, models.Function)
    nparam = session.query(models.FunctionParam).filter(
        models.FunctionParam.function_id == fun.id,
        models.FunctionParam.name.in_([param.name for param in params])).all()
    if len(nparam) == 0:
        nparam = None
    already_exists_except(nparam, f"{nparam} in function")
    param_models = to_list_models(params, models.FunctionParam)
    fun._params.extend(param_models)
    session.flush()
    res = to_list_contracts(param_models, contract.Id)
    return res

# def delete_function_param(session: Session, 
#                           param: int | models.FunctionParam) -> None:
#     param = get_model(session, param, models.FunctionParam)
#     session.delete(param)

def delete_function_params(session: Session, 
                           fun: int | models.Function | str) -> None:
    function_id = get_model_id(session, fun, models.Function)
    session.query(models.FunctionParam).filter(
        models.FunctionParam.function_id == function_id).delete()


# GlobalVarArg
    

def get_global_var_arg(session: Session, 
                       arg: int | models.GlobalVarArg,
                       contract_type: Type[CONTRACT]) -> CONTRACT:
    arg = get_model(session, arg, models.GlobalVarArg)
    res = to_contract(arg, contract_type)
    return res

def get_global_var_args(session: Session, 
                        global_var: int | models.GlobalVar, 
                        contract_type: Type[CONTRACT]) -> list[CONTRACT]:
    global_var = get_model(session, global_var, models.GlobalVar)
    res = to_list_contracts(global_var._args, contract_type)
    return res

# def post_global_var_arg(session: Session, 
#                         global_var: int | models.GlobalVar, 
#                         arg: contract.BaseGlobalVarArg) -> contract.Id:
#     global_var = get_model(session, global_var, models.GlobalVar)
#     model = to_model(arg, models.GlobalVarArg)
#     global_var._args.append(model)
#     session.flush()
#     res = to_contract(model, contract.Id)
#     return res

def post_global_var_args(session: Session,
                         global_var: int | models.GlobalVar,
                         args: list[contract.BaseGlobalVarArg]) -> list[contract.Id]:
    global_var = get_model(session, global_var, models.GlobalVar)
    arg_models = to_list_models(args, models.GlobalVarArg)
    global_var._args.extend(arg_models)
    session.flush()
    res = to_list_contracts(arg_models, contract.Id)
    return res

# def delete_global_var_arg(session: Session, 
#                           arg: int | models.GlobalVarArg) -> None:
#     arg = get_model(session, arg, models.GlobalVarArg)
#     session.delete(arg)
#     session.flush()

def delete_global_var_args(session: Session,
                       global_var: int | models.GlobalVar) -> None:
    global_var_id = get_model_id(session, global_var, models.GlobalVar)
    session.query(models.GlobalVarArg).filter(
        models.GlobalVarArg.global_var_id == global_var_id).delete()
    session.flush()


# Library
    

def get_library(session: Session, 
                lib: int | models.Library,
                contract_type: Type[CONTRACT]) -> CONTRACT:
    lib = get_model(session, lib, models.Library)
    res = to_contract(lib, contract_type)
    return res

def get_libraries(session: Session, 
                  program: int | models.Program | str,
                  contract_type: Type[CONTRACT]) -> list[CONTRACT]:
    program = get_model(session, program, models.Program)
    res = to_list_contracts(program._libraries, contract_type)
    return res

def post_library(session: Session, 
                 program: int | models.Program | str, 
                 lib: contract.BaseLibrary) -> contract.Id:
    program = get_model(session, program, models.Program)
    nlib = session.query(models.Library).filter(
        models.Library.program_id == program.id,
        models.Library.name == lib.name).first()
    already_exists_except(nlib, "library in program")
    model = to_model(lib, models.Library)
    program._libraries.append(model)
    session.flush()
    res = to_contract(model, contract.Id)
    return res

def post_libraries(session: Session,
                   program: int | models.Program | str,
                   libs: list[contract.BaseLibrary]) -> list[contract.Id]:
    program = get_model(session, program, models.Program)
    nlibs = session.query(models.Library.name).filter(
        models.Library.program_id == program.id,
        models.Library.name.in_([lib.name for lib in libs])).all()
    if len(nlibs) == 0:
        nlibs = None
    already_exists_except(nlibs, f"{nlibs}")
    lib_models = to_list_models(libs, models.Library)
    program._libraries.extend(lib_models)
    session.flush()
    res = to_list_contracts(lib_models, contract.Id)
    return res

def delete_library(session: Session, 
                   library: int | models.Library) -> None:
    lib = get_model(session, library, models.Library)
    session.delete(lib)
    session.flush()

def delete_libraries(session: Session,
                     program: int | models.Program | str) -> None:
    program_id = get_model_id(session, program, models.Program)
    session.query(models.Library).filter(
        models.Library.program_id == program_id).delete()
    session.flush()


# Function_NodeArg
    

def get_function_node_arg(session: Session, 
                          arg: int | models.Function_NodeArg,
                          contract_type: Type[CONTRACT]) -> CONTRACT:
    arg = get_model(session, arg, models.Function_NodeArg)
    res = to_contract(arg, contract_type)
    return res

def get_function_node_args(session: Session, 
                           fun_node: int | models.Function_Node,
                           contract_type: Type[CONTRACT]) -> list[CONTRACT]:
    fun_node = get_model(session, fun_node, models.Function_Node)
    res = to_list_contracts(fun_node._args, contract_type)
    return res

# def post_function_node_arg(session: Session, 
#                            fun_node: int | models.Function_Node,
#                            arg: contract.BaseFunction_NodeArg) -> contract.Id:
#     fun_node = get_model(session, fun_node, models.Function_Node)
#     model = to_model(arg, models.Function_NodeArg)
#     fun_node._args.append(model)
#     session.flush()
#     res = to_contract(model, contract.Id)
#     return res

def post_function_node_args(session: Session,
                            fun_node: int | models.Function_Node,
                            args: list[contract.BaseFunction_NodeArg]) -> list[contract.Id]:
    fun_node = get_model(session, fun_node, models.Function_Node)
    fun = get_model(session, fun_node.function_id, models.Function)
    if len(fun._params) != len(args):
        raise HTTPException(
            status.HTTP_400_BAD_REQUEST,
            "num params not equal num args"
        )
    arg_models = to_list_models(args, models.Function_NodeArg)
    fun_node._args.extend(arg_models)
    session.flush()
    res = to_list_contracts(arg_models, contract.Id)
    return res

# def delete_function_node_arg(session: Session, 
#                              arg: int | models.Function_NodeArg) -> None:
#     arg = get_model(session, arg, models.Function_NodeArg)
#     session.delete(arg)
#     session.flush()

def delete_function_node_args(session: Session,
                              fun_node: int | models.Function_Node) -> None:
    fun_node_id = get_model_id(session, fun_node, models.Function_Node)
    session.query(models.Function_NodeArg).filter(
        models.Function_NodeArg.function_node_id == fun_node_id).delete()
    session.flush()


# GlobalVar
    

def get_global_var(session: Session, 
                   global_var: int | models.GlobalVar,
                   contract_type: Type[CONTRACT]) -> CONTRACT:
    global_var = get_model(session, global_var, models.GlobalVar)
    res = to_contract(global_var, contract_type)
    return res

def get_global_vars(session: Session, 
                    program: int | models.Program | str,
                    contract_type: Type[CONTRACT]) -> list[CONTRACT]:
    program = get_model(session, program, models.Program)
    res = to_list_contracts(program._global_vars, contract_type)
    return res

def post_global_var(session: Session, 
                    program: int | models.Program | str, 
                    global_var: contract.BaseGlobalVar) -> contract.Id:
    program = get_model(session, program, models.Program)
    nglobal_var = session.query(models.GlobalVar).filter(
        models.GlobalVar.program_id == program.id,
        models.GlobalVar.name == global_var.name).first()
    already_exists_except(nglobal_var, "global var in program")
    global_var_model = to_model(global_var, models.GlobalVar)
    program._global_vars.append(global_var_model)
    session.flush()
    res = to_contract(global_var_model, contract.Id)
    return res

def post_global_vars(session: Session,
                     program: int | models.Program | str,
                     global_vars: list[contract.BaseGlobalVar]) -> list[contract.Id]:
    program = get_model(session, program, models.Program)
    nglobal_vars = session.query(models.GlobalVar).filter(
        models.GlobalVar.program_id == program.id,
        models.GlobalVar.name.in_([gv.name for gv in global_vars])).all()
    if len(nglobal_vars) == 0:
        nglobal_vars = None
    already_exists_except(nglobal_vars, f"{nglobal_vars} in program")
    gv_models = to_list_models(global_vars, models.GlobalVar)
    program._global_vars.extend(gv_models)
    session.flush()
    res = to_list_contracts(gv_models, contract.Id)
    return res

def delete_global_var(session: Session, 
                      global_var: int | models.GlobalVar) -> None:
    global_var = get_model(session, global_var, models.GlobalVar)
    session.delete(global_var)
    session.flush()

def delete_global_vars(session: Session,
                       program: int | models.Program | str) -> None:
    program = get_model(session, program, models.Program)
    session.query(models.GlobalVar).filter(
        models.GlobalVar.program_id == program.id).delete()
    session.flush()


# Program_Graph
    

def get_program_graph(session: Session,
                      program_graph: int | models.Program_Graph,
                      contract_type: Type[CONTRACT]) -> CONTRACT:
    program_graph = get_model(session, program_graph, models.Program_Graph)
    res = to_contract(program_graph, contract_type)
    return res

def get_program_graph(session: Session,
                      program: int | models.Program | str,
                      graph: int | models.Graph | str,
                      contract_type: Type[CONTRACT]) -> CONTRACT:
    program_id = get_model_id(session, program, models.Program)
    graph_id = get_model_id(session, graph, models.Graph)
    program_graph = session.query(models.Program_Graph).filter(
        models.Program_Graph.program_id == program_id,
        models.Program_Graph.graph_id == graph_id).first()
    not_found_except(program_graph, "program-graph")
    res = to_contract(program_graph, contract_type)
    return res

def get_program_graphs(session: Session,
                       contract_type: Type[CONTRACT],
                       program: int | models.Program | str | None = None,
                       graph: int | models.Graph | str | None = None) -> list[CONTRACT]:
    q = session.query(models.Program_Graph)
    if program is not None:
        program_id = get_model_id(session, program, models.Program)
        q = q.filter(models.Program_Graph.program_id == program_id)
    if graph is not None:
        graph_id = get_model_id(session, graph, models.Graph)
        q = q.filter(models.Program_Graph.graph_id == graph_id)
    pg_models = q.all()
    res = to_list_contracts(pg_models, contract_type)
    return res

def post_program_graph(session: Session,
                       program: int | models.Program | str,
                       graph: int | models.Graph | str) -> contract.Id:
    program = get_model(session, program, models.Program)
    graph = get_model(session, graph, models.Graph)
    nprogram_graph = session.query(models.Program_Graph).filter(
        models.Program_Graph.program_id == program.id,
        models.Program_Graph.graph_id == graph.id).first()
    already_exists_except(nprogram_graph, "program-graph")
    gp_model = models.Program_Graph()
    gp_model._program = program
    gp_model._graph = graph
    session.add(gp_model)
    session.flush()
    res = to_contract(gp_model, contract.Id)
    return res

def delete_program_graph(session: Session,
                         program_graph: int | models.Program_Graph) -> None:
    program_graph = get_model(session, program_graph, models.Program_Graph)
    session.delete(program_graph)
    session.flush()

def delete_program_graphs(session: Session,
                          program: int | models.Program | str | None = None,
                          graph: int | models.Graph | str | None = None) -> None:
    double_none_except(program, graph)
    q = session.query(models.Program_Graph)
    if program is None:
        program_id = get_model_id(session, program, models.Program)
        q = q.filter(models.Program_Graph.program_id == program_id)
    if graph is None:
        graph_id = get_model_id(session, graph, models.Graph)
        q = q.filter(models.Program_Graph.graph_id == graph_id)
    q.delete()
    session.flush()


# Function_Node
    

def get_function_node(session: Session,
                      function_node: int | models.Function_Node,
                      contract_type: Type[CONTRACT]) -> CONTRACT:
    function_node = get_model(session, function_node, models.Function_Node)
    res = to_contract(function_node, contract_type)
    return res

def get_function_node(session: Session,
                      function: int | models.Function | str,
                      node: int | models.Node | str,
                      contract_type: Type[CONTRACT]) -> CONTRACT:
    function_id = get_model_id(session, function, models.Function)
    node_id = get_model_id(session, node, models.Node)
    function_node = session.query(models.Function_Node).filter(
        models.Function_Node.function_id == function_id,
        models.Function_Node.node_id == node_id).first()
    not_found_except(function_node, "function-node")
    res = to_contract(function_node, contract_type)
    return res

def get_function_nodes(session: Session,
                       contract_type: Type[CONTRACT],
                       fun: int | models.Function | str | None = None,
                       node: int | models.Node | str | None = None) -> list[CONTRACT]:
    q = session.query(models.Function_Node)
    if fun is not None:
        fun_id = get_model_id(session, fun, models.Function)
        q = q.filter(models.Function_Node.function_id == fun_id)
    if node is not None:
        node_id = get_model_id(session, node, models.Node)
        q = q.filter(models.Function_Node.node_id == node_id)
    fn_models = q.all()
    res = to_list_contracts(fn_models, contract_type)
    return res

def post_function_node(session: Session,
                       fun: int | models.Function | str,
                       node: int | models.Node | str) -> contract.Id:
    fun = get_model(session, fun, models.Function)
    node = get_model(session, node, models.Node)
    
    if fun.gene_type != node.gene_type:
        raise HTTPException(
            status.HTTP_400_BAD_REQUEST,
            "fun gene_type not equal node gene_type"
        )
    nfun = session.query(models.Function).filter(
        models.Function.type == fun.type,
        models.Function.id == models.Function_Node.function_id,
        models.Function_Node.node_id == node.id).first()
    already_exists_except(nfun, f"function with type \"{fun.type}\" in node")
    if (fun.type == contract.FunctionTypes.pooling_populations and
        node.type != contract.NodeTypes.K_node):
        raise HTTPException(
            status.HTTP_400_BAD_REQUEST,
            f"{fun.type} used only in {contract.NodeTypes.K_node}"
        )
    if (fun.type == contract.FunctionTypes.start_population and
        node.type != contract.NodeTypes.population_node):
        raise HTTPException(
            status.HTTP_400_BAD_REQUEST,
            f"{fun.type} used only in {contract.NodeTypes.population_node}"
        )

    fun_node = models.Function_Node()
    fun_node._function = fun
    fun_node._node = node
    session.add(fun_node)
    session.flush()
    res = to_contract(fun_node, contract.Id)
    return res

def delete_function_node(session: Session,
                         fun_node: int | models.Function_Node) -> None:
    fun_node = get_model(session, fun_node, models.Function_Node)
    session.delete(fun_node)
    session.flush()

def delete_function_nodes(session: Session,
                          fun: int | models.Function | str | None = None,
                          node: int | models.Node | str | None = None) -> None:
    double_none_except(fun, node)
    q = session.query(models.Function_Node)
    if fun is not None:
        fun_id = get_model_id(session, fun, models.Function)
        q = q.filter(models.Function_Node.function_id == fun_id)
    if node is not None:
        node_id = get_model_id(session, node, models.Node)
        q = q.filter(models.Function_Node.node_id == node_id)
    q.delete()
    session.flush()


# Node_Graph
    

def get_node_graph(session: Session,
                   node_graph: int | models.Node_Graph,
                   contract_type: Type[CONTRACT]) -> CONTRACT:
    node_graph = get_model(session, node_graph, models.Node_Graph)
    res = to_contract(node_graph, contract_type)
    return res

def get_node_graph(session: Session,
                   node: int | models.Node | str,
                   graph: int | models.Graph | str,
                   contract_type: Type[CONTRACT]) -> CONTRACT:
    node_id = get_model_id(session, node, models.Node)
    graph_id = get_model_id(session, graph, models.Graph)
    node_graph = session.query(models.Node_Graph).filter(
        models.Node_Graph.node_id == node_id,
        models.Node_Graph.graph_id == graph_id).first()
    not_found_except(node_graph, "node-graph")
    res = to_contract(node_graph, contract_type)
    return res

def get_node_graphs(session: Session,
                    contract_type: Type[CONTRACT],
                    node: int | models.Node | str | None = None,
                    graph: int | models.Graph | str | None = None) -> list[CONTRACT]:
    q = session.query(models.Node_Graph)
    if node is not None:
        node_id = get_model_id(session, node, models.Node)
        q = q.filter(models.Node_Graph.node_id == node_id)
    if graph is not None:
        graph_id = get_model_id(session, graph, models.Graph)
        q = q.filter(models.Node_Graph.graph_id == graph_id)
    mg_models = q.all()
    res = to_list_contracts(mg_models, contract_type)
    return res

def post_node_graph(session: Session,
                    node_graph: contract.BaseNode_Graph,
                    node: int | models.Node | str,
                    graph: int | models.Graph | str,
                    parent: int | models.Node_Graph | str | None = None) -> contract.Id:
    node = get_model(session, node, models.Node)
    graph = get_model(session, graph, models.Graph)
    node_graphs_q = session.query(models.Node_Graph).filter(
        models.Node_Graph.graph_id == graph.id)
    if parent is None:
        root = node_graphs_q.filter(models.Node_Graph.parent_node_graph_id is not None).first()
        already_exists_except(root, "root in graph")
        ng_model = to_model(node_graph, models.Node_Graph)
        ng_model._node = node
        ng_model._graph = graph
    else:
        if type(parent) == str:
            parent = session.query(models.Node_Graph).filter(
                models.Node_Graph.graph_id == graph.id,
                models.Node_Graph.name == parent).first()
            not_found_except(parent, "parent")
        else:
            parent = get_model(session, parent, models.Node_Graph)
            if parent.graph_id != graph.id:
                raise HTTPException(
                    status.HTTP_400_BAD_REQUEST,
                    "parent node not in graph"
                )
        nnode_graph = node_graphs_q.filter(models.Node_Graph.name == node_graph.name).first()
        already_exists_except(nnode_graph, f"node_graph with name \"{node_graph.name}\"")
        ng_model = to_model(node_graph, models.Node_Graph)
        ng_model._node = node
        ng_model._graph = graph
        ng_model._parent_node_graph = parent
    session.add(ng_model)
    session.flush()
    res = to_contract(ng_model, contract.Id)
    return res

def delete_node_graph(session: Session,
                      node_graph: int | models.Node_Graph) -> None:
    node_graph = get_model(session, node_graph, models.Node_Graph)
    session.delete(node_graph)
    session.flush()

def delete_node_graphs(session: Session,
                       node: int | models.Node | str | None = None,
                       graph: int | models.Graph | str | None = None) -> None:
    double_none_except(node, graph)
    q = session.query(models.Node_Graph)
    if node is not None:
        node_id = get_model_id(session, node, models.Node)
        q = q.filter(models.Node_Graph.node_id == node_id)
    if graph is not None:
        graph_id = get_model_id(session, graph, models.Graph)
        q = q.filter(models.Node_Graph.graph_id == graph_id)
    q.delete()
    session.flush()


# Function
    

def get_function(session: Session, 
                 fun: int | models.Function | str,
                 contract_type: Type[CONTRACT]) -> CONTRACT:
    fun = get_model(session, fun, models.Function)
    res = to_contract(fun, contract_type)
    return res

def get_functions(session: Session,
                  contract_type: Type[CONTRACT],
                  node: int | models.Node | str | None = None, 
                  type: contract.FunctionTypes | None = None,
                  gene_type: str | None = None):
    q = session.query(models.Function)
    if node is not None:
        node_id = get_model_id(session, node, models.Node)
        q = q.filter(
            models.Function.id == models.Function_Node.function_id,
            models.Function_Node.node_id == node_id)
    if type is not None:
        if not isinstance(type, contract.FunctionTypes):
            raise HTTPException(
                status.HTTP_400_BAD_REQUEST,
                f"\"{type}\" not function type"
            )
        q = q.filter(models.Function.type == type)
    if gene_type is not None:
        q = q.filter(models.Function.gene_type == gene_type)
    funs = q.all()
    res = to_list_contracts(funs, contract_type)
    return res

def post_function(session: Session, 
                  fun: contract.BaseFunction) -> contract.Id:
    nfun = session.query(models.Function).filter(
        models.Function.name == fun.name).first()
    already_exists_except(nfun, "function")
    model = to_model(fun, models.Function)
    session.add(model)
    session.flush()
    res = to_contract(model, contract.Id)
    return res

def delete_function(session: Session, 
                    fun: int | models.Function| str) -> None:
    fun = get_model(session, fun, models.Function)
    session.delete(fun)
    session.flush()


# Node
    

def get_node(session: Session,
             node: int | models.Node | str,
             contract_type: Type[CONTRACT]) -> CONTRACT:
    node = get_model(session, node, models.Node)
    res = to_contract(node, contract_type)
    return res

def get_nodes(session: Session,
              contract_type: Type[CONTRACT],
              type: contract.NodeTypes | None = None,
              gene_type: str | None = None,
              function: int | models.Function | str | None = None,
              graph: int | models.Graph | str | None = None) -> list[CONTRACT]:
    q = session.query(models.Node)
    if type is not None:
        if not isinstance(type, contract.NodeTypes):
            raise HTTPException(
                status.HTTP_400_BAD_REQUEST,
                f"\"{type} is not node type\""
            )
        q = q.filter(models.Node.type == type)
    if gene_type is not None:
        q = q.filter(models.Node.gene_type == gene_type)
    if function is not None:
        function_id = get_model_id(session, function, models.Function)
        q = q.filter(models.Node.id == models.Function_Node.node_id,
                     models.Function_Node.function_id == function_id)
    if graph is not None:
        graph_id = get_model_id(session, graph, models.Graph)
        q = q.filter(models.Node.id == models.Node_Graph.node_id,
                     models.Node_Graph.graph_id == graph_id)
    nodes = q.all()
    res = to_list_contracts(nodes, contract_type)
    return res

def post_node(session: Session,
              node: contract.BaseNode) -> contract.Id:
    nnode = session.query(models.Node).filter(
        models.Node.name == node.name).first()
    already_exists_except(nnode, "node")
    model_node = to_model(node, models.Node)
    session.add(model_node)
    session.flush()
    res = to_contract(model_node, contract.Id)
    return res

def delete_node(session: Session,
                node: int | models.Node | str) -> None:
    node = get_model(session, node, models.Node)
    session.delete(node)
    session.flush()


# Graph
    

def get_graph(session: Session,
              graph: int | models.Graph | str,
              contract_type: Type[CONTRACT]) -> CONTRACT:
    graph = get_model(session, graph, models.Graph)
    res = to_contract(graph, contract_type)
    return res

def get_graphs(session: Session,
               contract_type: Type[CONTRACT],
               program: int | models.Program | str | None = None,
               node: int | models.Node | str | None = None,
               node_graph: int | models.Node_Graph | None = None) -> list[CONTRACT]:
    q = session.query(models.Graph)
    if program is not None:
        program_id = get_model_id(session, program, models.Program)
        q = q.filter(models.Graph.id == models.Program_Graph.graph_id,
                     models.Program_Graph.program_id == program_id)
    if node is not None:
        node_id = get_model_id(session, node, models.Node)
        q = q.filter(models.Graph.id == models.Node_Graph.graph_id,
                     models.Node_Graph.node_id == node_id)
    if node_graph is not None:
        node_graph = get_model_id(session, node_graph, models.Node_Graph)
        q = q.filter(models.Graph.id == models.Node_Graph.graph_id,
                     models.Node_Graph.id == node_graph)
    graphs = q.all()
    res = to_list_contracts(graphs, contract_type)
    return res

def post_graph(session: Session,
               graph: contract.BaseGraph) -> contract.Id:
    ngraph = session.query(models.Graph).filter(
        models.Graph.name == graph.name).first()
    already_exists_except(ngraph, "graph")
    m_graph = to_model(graph, models.Graph)
    session.add(m_graph)
    session.flush()
    res = to_contract(m_graph, contract.Id)
    return res

def delete_graph(session: Session,
                 graph: int | models.Graph | str) -> None:
    graph = get_model(session, graph, models.Graph)
    session.delete(graph)
    session.flush()


# Program
    

def get_program(session: Session,
                program: int | models.Program | str,
                contract_type: Type[CONTRACT]) -> CONTRACT:
    program = get_model(session, program, models.Program)
    res = to_contract(program, contract_type)
    return res

def get_programs(session: Session,
                 contract_type: Type[CONTRACT],
                 graph: int | models.Graph | str | None = None) -> list[CONTRACT]:
    q = session.query(models.Program)
    if graph is not None:
        graph_id = get_model_id(session, graph, models.Graph)
        q = q.filter(models.Program.id == models.Program_Graph.program_id,
                     models.Program_Graph.graph_id == graph_id)
    program_models = q.all()
    res = to_list_contracts(program_models, contract_type)
    return res

def post_program(session: Session,
                 program: contract.BaseProgram) -> contract.Id:
    nprog = session.query(models.Program).filter(models.Program.name == program.name).first()
    already_exists_except(nprog, "program")
    program_model = to_model(program, models.Program)
    session.add(program_model)
    session.flush()
    res = to_contract(program_model, contract.Id)
    return res

def delete_program(session: Session,
                   program: int | models.Program | str) -> None:
    program = get_model(session, program, models.Program)
    session.delete(program)
    session.flush()

