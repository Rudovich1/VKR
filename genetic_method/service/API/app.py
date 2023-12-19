from fastapi import FastAPI
from genetic_method.service.API import contract
from genetic_method.service.API import controler


app = FastAPI()


@app.get("/test/hello_world")
def hello_world():
    return {"message": "Hello World"}


# Function

@app.get("/functions/{function_id}")
def get_function(function_id: int) -> contract.FunctionRes:
    session = controler.get_session()
    try:
        fun = controler.get_function(session, function_id, contract.Function)
        res = contract.FunctionRes(
            params=controler.get_function_params(session, function_id, contract.FunctionParamRes),
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
                  node_id: int | None = None) -> list[contract.FunctionRes]:
    session = controler.get_session()
    try:
        funs = controler.get_functions(session, contract.Function, node_id, type, gene_type)
        res = [contract.FunctionRes(
                params=controler.get_function_params(session, fun.id, contract.FunctionParamRes),
                **fun.model_dump()) for fun in funs]
    except Exception as ex:
        session.rollback()
        raise ex
        
    session.commit()
    return res

@app.post("/functions")
def post_function(fun: contract.BaseFunction, 
                  params: list[contract.BaseFunctionParam]) -> contract.FunctionPostRes:
    session = controler.get_session()
    try:
        fun_id = controler.post_function(session, fun)
        param_ids = controler.post_function_params(session, fun_id.id, params)
        res = contract.FunctionPostRes(id=fun_id.id, param_ids=param_ids)
    except Exception as ex:
        session.rollback()
        raise ex
    
    session.commit()
    return res

@app.delete("/functions/{function_id}")
def delete_function(function_id: int) -> None:
    session = controler.get_session()
    try:
        controler.delete_function(session, function_id)
    except Exception as ex:
        session.rollback()
        raise ex
    
    session.commit()


# TODO


# Node


# @app.get("/nodes/{node_id}")
# def get_node(node_id: int) -> contract.NodeRes:
#     session = controler.get_session()
#     try:
#         node = controler.get_node


# Graph


# Program