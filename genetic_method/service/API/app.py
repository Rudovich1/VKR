from fastapi import FastAPI
from genetic_method.service.API import contract
from genetic_method.service.API import controler
import json
from pathlib import Path


app = FastAPI()


@app.get("/test/hello_world")
def hello_world():
    return {"message": "Hello World"}


# Function


@app.get("/functions/name={function_name}")
def get_function(function_name: str) -> contract.FunctionRes:
    session = controler.get_session()
    try:
        fun = controler.get_function(session, function_name, contract.Function)
        res = contract.FunctionRes(
            params=controler.get_function_params(session, fun.id, contract.BaseFunctionParam),
            **fun.model_dump()
        )
    except Exception as ex:
        session.rollback()
        raise ex
    
    session.commit()
    return res

@app.get("/functions")
def get_functions(type: contract.FunctionTypes | None = None,
                  gene_type: str | None = None,
                  node_name: str | None = None) -> list[contract.FunctionRes]:
    session = controler.get_session()
    try:
        funs = controler.get_functions(session, contract.Function, node_name, type, gene_type)
        res = [contract.FunctionRes(
                params=controler.get_function_params(session, fun.id, contract.BaseFunctionParam),
                **fun.model_dump()) for fun in funs]
    except Exception as ex:
        session.rollback()
        raise ex
        
    session.commit()
    return res

@app.post("/functions")
def post_function(function: contract.BaseFunction, 
                  params: list[contract.BaseFunctionParam]) -> None:
    session = controler.get_session()
    try:
        fun_id = controler.post_function(session, function)
        controler.post_function_params(session, fun_id.id, params)
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()

@app.delete("/functions/name={function_name}")
def delete_function(function_name: str) -> None:
    session = controler.get_session()
    try:
        controler.delete_function(session, function_name)
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()
    

# Node


@app.get("/nodes/name={node_name}")
def get_node(node_name: str) -> contract.NodeRes:
    session = controler.get_session()
    try:
        node = controler.get_node(session, node_name, contract.Node)
        res = contract.NodeRes(**node.model_dump())
        functions = controler.get_functions(session, contract.Function, node=node.id)
        for fun in functions:
            fun_node = controler.get_function_node(session, fun.id, node.id, contract.Function_Node)
            fun_params = controler.get_function_params(session, fun.id, contract.BaseFunctionParam)
            fun_args = controler.get_function_node_args(session, fun_node.id, contract.BaseFunction_NodeArg)
            res.functions.append(contract.FunctionWithArgs(**fun.model_dump(),
                                                           params=fun_params,
                                                           args=fun_args))
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()
    return res

@app.get("/nodes")
def get_nodes(type: contract.NodeTypes | None = None,
              gene_type: str | None = None,
              function_name: str | None = None,
              graph_name: str | None = None) -> list[contract.NodeRes]:
    session = controler.get_session()
    try:
        nodes = controler.get_nodes(session, contract.Node, type, gene_type, function_name, graph_name)
        res = [get_node(node.id) for node in nodes]
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()
    return res

@app.post("/nodes")
def post_node(node: contract.BaseNode) -> None:
    session = controler.get_session()
    try:
        controler.post_node(session, node)
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()

@app.delete("/nodes/name={node_name}")
def delete_node(node_name: str) -> None:
    session = controler.get_session()
    try:
        controler.delete_node(session, node_name)
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()


# Graph


@app.get("/graphs/name={graph_name}")
def get_graph(graph_name: str) -> contract.GraphRes:
    session = controler.get_session()
    try:
        graph = controler.get_graph(session, graph_name, contract.Graph)
        node_graphs = controler.get_node_graphs(session, contract.Node_Graph, graph=graph.id)
        ng_res = []
        for node_graph in node_graphs:
            model = contract.Node_GraphRes(
                name=node_graph.name,
                parent_node_graph_name=(controler.get_node_graph(session, 
                                                                node_graph.parent_node_graph_id, 
                                                                contract.Node_Graph).name
                if node_graph.parent_node_graph_id is not None else None),
                node_name=controler.get_node(session, node_graph.node_id, contract.Node).name
            )
            ng_res.append(model)
        res = contract.GraphRes(name=graph.name, node_graphs=ng_res)
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()
    return res

@app.get("/graphs")
def get_graphs(program_name: str | None = None,
               node_name: str | None = None) -> list[contract.GraphRes]:
    session = controler.get_session()
    try:
        graphs = controler.get_graphs(session, contract.Graph, program_name, node_name)
        res = [get_graph(graph.name) for graph in graphs]
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()
    return res

@app.post("/graphs")
def post_graph(graph: contract.BaseGraph) -> None:
    session = controler.get_session()
    try:
        controler.post_graph(session, graph).id
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()

@app.delete("/graphs/name={graph_name}")
def delete_graph(graph_name: str) -> None:
    session = controler.get_session()
    try:
        controler.delete_graph(session, graph_name)
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()


# Program


@app.get("/programs/name={program_name}")
def get_program(program_name: str) -> contract.ProgramRes:
    session = controler.get_session()
    try:
        program = controler.get_program(session, program_name, contract.Program)
        graphs = get_graphs(program_name)
        libraries = controler.get_libraries(session, program_name, contract.BaseLibrary)
        global_vars = controler.get_global_vars(session, program_name, contract.GlobalVar)
        global_vars_res = [contract.GlobalVarRes(
            **global_var.model_dump(),
            args=controler.get_global_var_args(session, global_var.id, contract.BaseGlobalVarArg))
            for global_var in global_vars]
        res = contract.ProgramRes(
            name=program.name,
            libraries=libraries,
            global_vars=global_vars_res,
            graphs=graphs)
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()
    return res

@app.get("/programs")
def get_programs(graph_name: str | None = None) -> list[contract.ProgramRes]:
    session = controler.get_session()
    try:
        programs = controler.get_programs(session, contract.Program, graph_name)
        res = [get_program(program.name) for program in programs]
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()
    return res

@app.post("/programs")
def post_program(program: contract.BaseProgram,
                 libs: list[contract.BaseLibrary],
                 global_vars: list[contract.GlobalVarWithArgs]) -> None:
    session = controler.get_session()
    try:
        program_id = controler.post_program(session, program).id
        controler.post_libraries(session, program_id, libs)
        gv_base = [contract.BaseGlobalVar(**gv.model_dump()) for gv in global_vars]
        gv_args_base = [gv.args for gv in global_vars]
        gv_ids = controler.post_global_vars(session, program_id, gv_base)
        for index in range(len(gv_ids)):
            controler.post_global_var_args(session, gv_ids[index].id, gv_args_base[index])
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()

@app.delete("/programs/name={program_name}")
def delete_program(program_name: str) -> None:
    session = controler.get_session()
    try:
        controler.delete_program(session, program_name)
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()

    
# link program-graph
    

@app.post("/connections/program-graph")
def post_connect_program_graph(program_name: str,
                               graph_name: str) -> None:
    session = controler.get_session()
    try:
        controler.post_program_graph(session, program_name, graph_name)
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()

@app.delete("/connection/program-graph")
def delete_connect_program_graph(program_name: str,
                                 graph_name: str) -> None:
    session = controler.get_session()
    try:
        pg_model = controler.get_program_graph(session, program_name, graph_name, contract.Program_Graph)
        controler.delete_program_graph(session, pg_model.id)
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()


# link function-node
    

@app.post("/connections/function-node")
def post_connect_function_node(function_name: str,
                               node_name: str,
                               args: list[contract.BaseFunction_NodeArg]) -> None:
    session = controler.get_session()
    try:
        fun_node = controler.post_function_node(session, function_name, node_name)
        controler.post_function_node_args(session, fun_node.id, args)
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()

@app.delete("/connections/function-node")
def delete_connect_function_node(function_name: str,
                                 node_name: str) -> None:
    session = controler.get_session()
    try:
        fun_node = controler.get_function_node(session, function_name, node_name, contract.Function_Node)
        controler.delete_function_node(session, fun_node.id)
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()


# link node-graph
    

@app.post("/connections/node-graph")
def post_connect_node_graph(node_graph: contract.BaseNode_Graph,
                            node_name: str,
                            graph_name: str,
                            parent_node_name: str | None) -> None:
    session = controler.get_session()
    try:
        controler.post_node_graph(session, node_graph, node_name, graph_name, parent_node_name)
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()

@app.delete("/connection/node-graph")
def delete_connect_node_graph(node_name: str,
                              graph_name: str) -> None:
    session = controler.get_session()
    try:
        node_graph = controler.get_node_graph(session, node_name, graph_name, contract.Node_Graph)
        controler.delete_node_graph(session, node_graph.id)
    except Exception as ex:
        session.rollback()
        raise ex
    session.commit()


STARTUP_PATH = Path("genetic_method\\service\\db\\startup_data\\startup.json")

@app.on_event("startup")
def startup_db(path: Path = STARTUP_PATH):
    with open(path.absolute()) as file:
        json_data: dict[str,] = json.load(file)
    try:
        for fun in json_data["functions"]:
            params = [contract.BaseFunctionParam(**param) for param in fun["params"]]
            fun = contract.BaseFunction(**fun)
            try:
                post_function(fun, params)
            except Exception as ex:
                print(ex)

        for node in json_data["nodes"]:
            node = contract.BaseNode(**node)
            try:
                post_node(node)
            except Exception as ex:
                print(ex)

        for graph in json_data["graphs"]:
            graph = contract.BaseGraph(**graph)
            try:
                post_graph(graph)
            except Exception as ex:
                print(ex)
        
        for program in json_data["programs"]:
            libs = [contract.BaseLibrary(**lib) for lib in program["libs"]]
            global_vars = [contract.GlobalVarWithArgs(
                name=gv["name"],
                is_const=gv["is_const"],
                type=gv["type"],
                args=[contract.BaseGlobalVarArg(arg=arg) for arg in gv["args"]]
            ) for gv in program["global_vars"]]
            cont = contract.BaseProgram(name=program["name"])
            try:
                post_program(cont, libs, global_vars)
            except Exception as ex:
                print(ex)

        for fun_node in json_data["function_nodes"]:
            args = [contract.BaseFunction_NodeArg(arg=arg) for arg in fun_node["args"]]
            fun_name = fun_node["function"]
            node_name = fun_node["node"]
            try:
                post_connect_function_node(fun_name, node_name, args)
            except Exception as ex:
                print(ex)
        
        for program_graph in json_data["program_graphs"]:
            try:
                post_connect_program_graph(**program_graph)
            except Exception as ex:
                print(ex)
        
        for node_graph in json_data["node_graphs"]:
            cont = contract.BaseNode_Graph(name=node_graph["name"])
            try:
                post_connect_node_graph(node_graph=cont, 
                                        node_name=node_graph["node_name"],
                                        graph_name=node_graph["graph_name"],
                                        parent_node_name=node_graph["parent_node_name"])
            except Exception as ex:
                print(ex)
            
    except Exception as ex:
        exit()
        