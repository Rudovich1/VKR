from fastapi import HTTPException
from starlette import status
from genetic_method.service.API import contract
from genetic_method.service.db import models, engine
from sqlalchemy.orm import Session, Query
from sqlalchemy import select
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
        return session

def not_found_except(item, name: str) -> str:
    if item is None:
        raise HTTPException(
            status.HTTP_404_NOT_FOUND,
            f"{name} not found"
        )
    
def already_exists_except(item, name: str) -> str:
    if item is not None:
        raise HTTPException(
            status.HTTP_400_BAD_REQUEST,
            f"{name} already exists"
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
              model: int | MODEL, 
              model_type: Type[MODEL]) -> MODEL:
    if type(model) == int:
        model = session.query(model_type).get(model)
    not_found_except(model, model_type.__tablename__)
    return model


# FunctionParam


def get_function_param(session: Session, 
                       param: int | models.FunctionParam, 
                       contract_type: Type[CONTRACT]) -> CONTRACT:
    param = get_model(session, param, models.FunctionParam)
    res = to_contract(param, contract_type)
    return res

def get_function_params(session: Session, 
                        fun: int | models.Function, 
                        contract_type: Type[CONTRACT]) -> list[CONTRACT]:
    fun = get_model(session, fun, models.Function)
    res = to_list_contracts(fun._params, contract_type)
    return res

def post_function_param(session: Session,
                        fun: int | models.Function,
                        param: contract.BaseFunctionParam) -> contract.Id:
    fun = get_model(session, fun, models.Function)
    nparam = session.query(models.FunctionParam).filter(
        models.FunctionParam.name == param.name,
        models.FunctionParam.function_id == fun.id).first()
    already_exists_except(nparam, "param")
    param_model = to_model(param, models.FunctionParam)
    fun._params.append(param_model)
    session.flush()
    res = to_contract(param_model, contract.Id)
    return res

def post_function_params(session: Session,
                         fun: int | models.Function, 
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

def delete_function_param(session: Session, 
                          param: int | models.FunctionParam) -> None:
    param = get_model(session, param, models.FunctionParam)
    session.delete(param)

def delete_function_params(session: Session, 
                           function: int | models.Function) -> None:
    function_id = function
    if type(function) == models.Function:
        function_id = function.id
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

def post_global_var_arg(session: Session, 
                        global_var: int | models.GlobalVar, 
                        arg: contract.BaseGlobalVarArg) -> contract.Id:
    global_var = get_model(session, global_var, models.GlobalVar)
    model = to_model(arg, models.GlobalVarArg)
    global_var._args.append(model)
    session.flush()
    res = to_contract(model, contract.Id)
    return res

def post_global_var_args(session: Session,
                         global_var: int | models.GlobalVar,
                         args: list[contract.BaseGlobalVarArg]) -> list[contract.Id]:
    global_var = get_model(session, global_var, models.GlobalVar)
    arg_models = to_list_models(args, models.GlobalVarArg)
    global_var._args.extend(arg_models)
    session.flush()
    res = to_list_contracts(arg_models, contract.Id)
    return res

def delete_global_var_arg(session: Session, 
                          arg: int | models.GlobalVarArg) -> None:
    arg = get_model(session, arg, models.GlobalVarArg)
    session.delete(arg)
    session.flush()

def delete_global_var_args(session: Session,
                       global_var: int | models.GlobalVar) -> None:
    global_var_id = global_var
    if type(global_var) == models.GlobalVar:
        global_var_id = global_var.id
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
                  program: int | models.Program,
                  contract_type: Type[CONTRACT]) -> list[CONTRACT]:
    program = get_model(session, program, models.Program)
    res = to_list_contracts(program._libraries, contract_type)
    return res

def post_library(session: Session, 
                 program: int | models.Program, 
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
                   program: int | models.Program,
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
    res = to_contract(lib_models, contract.Id)
    return res

def delete_library(session: Session, 
                   library: int | models.Library) -> None:
    lib = get_model(session, library, models.Library)
    session.delete(lib)
    session.flush()

def delete_libraries(session: Session,
                     program: int | models.Program) -> None:
    program_id = program
    if type(program) == models.Program:
        program_id = program.id
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

def post_function_node_arg(session: Session, 
                           fun_node: int | models.Function_Node,
                           arg: contract.BaseFunction_NodeArg) -> contract.Id:
    fun_node = get_model(session, fun_node, models.Function_Node)
    model = to_model(arg, models.Function_NodeArg)
    fun_node._args.append(model)
    session.flush()
    res = to_contract(model, contract.Id)
    return res

def post_function_node_args(session: Session,
                            fun_node: int | models.Function_Node,
                            args: list[contract.BaseFunction_NodeArg]) -> list[contract.Id]:
    fun_node = get_model(session, fun_node, models.Function_Node)
    arg_models = to_list_models(args, models.Function_NodeArg)
    fun_node._args.extend(arg_models)
    session.flush()
    res = to_list_contracts(arg_models, contract.Id)
    return res

def delete_function_node_arg(session: Session, 
                             arg: int | models.Function_NodeArg) -> None:
    arg = get_model(session, arg, models.Function_NodeArg)
    session.delete(arg)
    session.flush()

def delete_function_node_args(session: Session,
                              fun_node: int | models.Function_Node) -> None:
    fun_node_id = fun_node
    if type(fun_node) == models.Function_Node:
        fun_node_id = fun_node.id
    session.query(models.Function_NodeArg).filter(
        models.Function_NodeArg.function_node_id == fun_node_id).delete()
    session.flush()


# GlobalVar
    

def get_global_var(session: Session, 
                   global_var: int | models.GlobalVar,
                   contract_type: Type[CONTRACT]) -> CONTRACT:
    global_var = get_model(session, global_var, models.GlobalVar)
    res = to_contract(global_var, CONTRACT)
    return res

def get_global_vars(session: Session, 
                    program: int | models.Program,
                    contract_type: Type[CONTRACT]) -> list[CONTRACT]:
    program = get_model(session, program, models.Program)
    res = to_list_contracts(program._global_vars, contract_type)
    return res

def post_global_var(session: Session, 
                    program: int | models.Program, 
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
                     program: int | models.Program,
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
    delete_global_var_args(session, global_var)
    session.delete(global_var)
    session.flush()

def delete_global_vars(session: Session,
                       program: int | models.Program) -> None:
    program = get_model(session, program, models.Program)
    for global_var in program._global_vars:
        delete_global_var_args(session, global_var)
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

def get_program_graphs(session: Session,
                       contract_type: Type[CONTRACT],
                       program: int | models.Program | None = None,
                       graph: int | models.Graph | None = None) -> list[CONTRACT]:
    q = session.query(models.Program_Graph)
    if program is not None:
        program_id = program
        if type(program) == models.Program:
            program_id = program.id
        q = q.filter(models.Program_Graph.program_id == program_id)
    if graph is not None:
        graph_id = graph
        if type(graph) == models.Graph:
            graph_id = graph.id
        q = q.filter(models.Program_Graph.graph_id == graph_id)
    pg_models = q.all()
    res = to_list_contracts(pg_models, contract_type)
    return res

def post_program_graph(session: Session,
                       program: int | models.Program,
                       graph: int | models.Graph) -> contract.Id:
    program = get_model(session, program, models.Program)
    graph = get_model(session, graph, models.Graph)
    nprogram_graph = session.query(models.Program_Graph).filter(
        models.Program_Graph.program_id == program.id,
        models.Program_Graph.graph_id == graph.id).first()
    already_exists_except(nprogram_graph, "program-graph")
    gp_model = models.Program_Graph()
    program._graph_associations.append(gp_model)
    graph._program_associations.append(gp_model)
    session.flush()
    res = to_contract(gp_model, contract.Id)
    return res

def delete_program_graph(session: Session,
                         program: int | models.Program,
                         graph: int | models.Graph) -> None:
    program_id = program
    if type(program) == models.Program:
        program_id = program.id
    graph_id = graph
    if type(graph) == models.Graph:
        graph_id = graph.id
    session.query(models.Program_Graph).filter(
        models.Program_Graph.program_id == program_id,
        models.Program_Graph.graph_id == graph_id).delete()
    session.flush()


# Function_Node
    

def get_function_node(session: Session,
                      function_node: int | models.Function_Node,
                      contract_type: Type[CONTRACT]) -> CONTRACT:
    function_node = get_model(session, function_node, models.Function_Node)
    res = to_contract(function_node, contract_type)
    return res

def get_function_nodes(session: Session,
                       contract_type: Type[CONTRACT],
                       fun: int | models.Function | None = None,
                       node: int | models.Node | None = None) -> list[CONTRACT]:
    q = session.query(models.Function_Node)
    if fun is not None:
        fun_id = fun
        if type(fun) == models.Function:
            fun_id = fun.id
        q = q.filter(models.Function_Node.function_id == fun_id)
    if node is not None:
        node_id = node
        if type(node) == models.Node:
            node_id = node.id
        q = q.filter(models.Function_Node.node_id == node_id)
    fn_models = q.all()
    res = to_list_contracts(fn_models, contract_type)
    return res

def post_function_node(session: Session,
                       fun: int | models.Function,
                       node: int | models.Node) -> contract.Id:
    fun = get_model(session, fun, models.Function)
    node = get_model(session, node, models.Node)
    fun_node = models.Function_Node()
    fun._node_associations.append(fun_node)
    node._function_associations.append(fun_node)
    session.flush()
    res = to_contract(fun_node, contract.Id)
    return res

def delete_function_node(session: Session,
                         fun_node: int | models.Function_Node) -> None:
    fun_node = get_model(session, fun_node, models.Function_Node)
    delete_function_node_args(session, fun_node)
    session.delete(fun_node)
    session.flush()


# Node_Graph
    

def get_node_graph(session: Session,
                   node_graph: int | models.Node_Graph,
                   contract_type: Type[CONTRACT]) -> CONTRACT:
    node_graph = get_model(session, node_graph, models.Node_Graph)
    res = to_contract(node_graph, contract_type)
    return res

def get_node_graphs(session: Session,
                    contract_type: Type[CONTRACT],
                    node: int | models.Node | None = None,
                    graph: int | models.Graph | None = None) -> list[CONTRACT]:
    q = session.query(models.Node_Graph)
    if node is not None:
        node_id = node
        if type(node) == models.Node:
            node_id = node.id
        q = q.filter(models.Node_Graph.node_id == node_id)
    if graph is not None:
        graph_id = graph
        if type(graph) == models.Graph:
            graph_id = graph.id
        q = q.filter(models.Node_Graph.graph_id == graph_id)
    mg_models = q.all()
    res = to_list_contracts(mg_models, contract_type)
    return res

def post_node_graph(session: Session,
                    node: int | models.Node,
                    graph: int | models.Graph,
                    parent: int | models.Node_Graph | None = None) -> contract.Id:
    node = get_model(session, node, models.Node)
    graph = get_model(session, graph, models.Graph)
    node_graphs_q = session.query(models.Node_Graph).filter(
        models.Node_Graph.node_id == node.id,
        models.Node_Graph.graph_id == graph.id)
    if parent is None:
        root = node_graphs_q.filter(models.Node_Graph.parent_node_graph_id != None).first()
        already_exists_except(root, "root in graph")
        ng_model = models.Node_Graph()
        node._node_graphs.append(ng_model)
        graph._node_graphs.append(ng_model)
    else:
        parent = get_model(session, parent, models.Node_Graph)
        if parent.graph_id != graph.id:
            raise HTTPException(
                status.HTTP_400_BAD_REQUEST,
                "parent node not in graph"
            )
        ng_model = models.Node_Graph()
        node._node_graphs.append(ng_model)
        graph._node_graphs.append(ng_model)
        parent._child_node_graphs.append(ng_model)
    session.flush()
    res = to_contract(ng_model, contract.Id)
    return res

def delete_node_graph(session: Session,
                      node_graph: int | models.Node_Graph) -> None:
    node_graph = get_model(session, node_graph, models.Node_Graph)
    for child in node_graph._child_node_graphs:
        delete_node_graph(session, child)
    session.delete(node_graph)
    session.flush()


# Function
    

def get_function(session: Session, 
                 fun: int | models.Function,
                 contract_type: Type[CONTRACT]) -> CONTRACT:
    fun = get_model(session, fun, models.Function)
    res = to_contract(fun, contract_type)
    return res

def get_functions(session: Session,
                  contract_type: Type[CONTRACT],
                  node: int | models.Node | None = None, 
                  type: contract.FunctionTypes | None = None,
                  gene_type: str | None = None):
    q = session.query(models.Function)
    if node is not None:
        node_id = node
        if type(node) == models.Node:
            node_id = node.id
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
        models.Function.name == fun.name,
        models.Function.gene_type == fun.gene_type,
        models.Function.type == fun.type,
        models.Function.code == fun.code).first()
    already_exists_except(nfun, "function")
    model = to_model(fun, models.Function)
    session.add(model)
    session.flush()
    res = to_contract(model, contract.Id)
    return res

def delete_function(session: Session, 
                    fun: int | models.Function) -> None:
    fun = get_model(session, fun, models.Function)
    delete_function_params(session, fun)
    for fun_node in fun._node_associations:
        delete_function_node(session, fun_node)
    session.delete(fun)
    session.flush()


# TODO
