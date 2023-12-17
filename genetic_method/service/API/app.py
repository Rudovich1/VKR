from fastapi import FastAPI, HTTPException
from starlette import status
from genetic_method.service.API import contract
from genetic_method.service.db import engine, models
from sqlalchemy.orm import Session

# from genetic_method.API import controller
# from genetic_method.API import contract
# from genetic_method.API.startup_db import startup_db


def get_session(engine=engine) -> Session:
    with Session(engine) as session:
        return session


app = FastAPI()


def not_found_except(item, name: str) -> str:
    if item is None:
        raise HTTPException(
            status.HTTP_404_NOT_FOUND,
            f"{name} not found"
        )
    
def already_exists_except(item, name: str) -> str:
    if item is None:
        raise HTTPException(
            status.HTTP_400_BAD_REQUEST,
            f"{name} already exists"
        )


@app.get("/test/hello_world")
def hello_world():
    return {"message": "Hello World"}


# FunctionParam


@app.get("/get/function/param/id={param_id}/")
def get_function_param(param_id: int) -> contract.FunctionParamLinks:
    session = get_session()
    param: models.FunctionParam = session.query(models.FunctionParam).get(param_id)
    not_found_except(param, "param")
    session.commit()
    res = contract.FunctionParamLinks(**param.to_dict())
    return res

@app.get("/get/function/id={function_id}/param/")
def get_function_params_by_function(function_id: int) -> list[contract.FunctionParamId]:
    session = get_session()
    fun: models.Function = session.query(models.Function).get(function_id)
    not_found_except(fun, "function")
    session.commit()
    res = list(map(lambda x: contract.FunctionId(**x.to_dict()), fun._params))
    return res

# @app.get("/get/function/param/")
# def get_function_params() -> list[contract.FunctionParam]:
#     session = get_session()
#     params = session.query(models.FunctionParam).all()
#     session.commit()
#     res = list(map(lambda x: contract.FunctionParam(**x.to_dict()), params))
#     return res

@app.post("/post/function/id={function_id}/param/")
def post_function_param(
        function_id: int, 
        param: contract.BaseFunctionParam) -> contract.Id:

    session = get_session()
    fun: models.Function = session.query(models.Function).get(function_id)
    not_found_except(fun, "function")
    already_exists_except(session.query(models.FunctionParam).filter(
        models.FunctionParam.function_id == function_id,
        models.FunctionParam.name == param.name).first(), "param in function")
    
    model: models.FunctionParam = models.FunctionParam.make_model(param.__dict__)
    fun._params.append(model)
    session.commit()
    res = contract.Id(**model.to_dict())
    return res

@app.delete("/delete/function/param/id={param_id}/")
def delete_function_param(param_id: int) -> None:
    session = get_session()
    param = session.query(models.FunctionParam).get(param_id)
    not_found_except(param, "param")
    session.delete(param)
    session.commit()


# GlobalVarArg
    

@app.get("/get/header/global_var/arg/id={arg_id}/")
def get_global_var_arg(arg_id: int) -> contract.GlobalVarArgLinks:
    session = get_session()
    arg: models.GlobalVarArg = session.query(models.GlobalVarArg).get(arg_id)
    not_found_except(arg, "arg")
    session.commit()
    res = contract.GlobalVarArgLinks(**arg.to_dict())
    return res

@app.get("/get/header/global_var/id={global_var_id}/arg/")
def get_global_vars_by_global_var(global_var_id: int) -> list[contract.GlobalVarArgId]:
    session = get_session()
    global_var: models.GlobalVar = session.query(models.GlobalVar).get(global_var_id)
    not_found_except(global_var, "global var")
    session.commit()
    res = list(map(lambda x: contract.GlobalVarId(**x.to_dict()), global_var._args))
    return res

# @app.get("/get/header/global_var/arg/")
# def get_global_var_args() -> list[contract.GlobalVarArg]:
#     session = get_session()
#     args = session.query(models.GlobalVarArg).all()
#     session.commit()
#     res = list(map(lambda x: contract.GlobalVarArg(**x.to_dict()), args))
#     return res


@app.post("/post/header/global_var/id={global_var_id}/arg/")
def post_global_var_arg(global_var_id: int, arg: contract.BaseGlobalVarArg) -> contract.Id:
    session = get_session()
    global_var: models.GlobalVar = session.query(models.GlobalVar).get(global_var_id)
    not_found_except(global_var, "global var")
    model: models.GlobalVarArg = models.GlobalVarArg.make_model(arg.__dict__)
    global_var._args.append(model)
    session.commit()
    res = contract.Id(**model.to_dict())
    return res

@app.delete("/delete/header/global_var/arg/id={arg_id}/")
def delete_global_var_arg(global_var_arg_id: int) -> None:
    session = get_session()
    global_var_arg = session.query(models.GlobalVarArg).get(global_var_arg_id)
    not_found_except(global_var_arg, "arg")
    session.delete(global_var_arg)
    session.commit()


# Library
    

@app.get("/get/header/library/id={library_id}/")
def get_library(library_id: int) -> contract.LibraryLinks:
    session = get_session()
    lib: models.Library = session.query(models.Library).get(library_id)
    not_found_except(lib, "library")
    session.commit()
    res = contract.LibraryLinks(**lib.to_dict())
    return res

@app.get("/get/header/id={header_id}/library/")
def get_libraries_by_header(header_id: int) -> list[contract.LibraryId]:
    session = get_session()
    header: models.Header = session.query(models.Header).get(header_id)
    not_found_except(header, "header")
    session.commit()
    res = list(map(lambda x: contract.LibraryId(**x.to_dict()), header._libraries))
    return res

# @app.get("/get/header/library/")
# def get_libraries() -> list[contract.Library]:
#     session = get_session()
#     libs = session.query(models.Library).all()
#     session.commit()
#     res = list(map(lambda x: contract.Library(**x.to_dict()), libs))
#     return res

@app.post("/post/header/id={header_id}/library/")
def post_library(header_id: int, lib: contract.BaseLibrary) -> contract.Id:
    session = get_session()
    header: models.Header = session.query(models.Header).get(header_id)
    not_found_except(header, "header")
    already_exists_except(session.query(models.Library).filter(
        models.Library.header_id == header_id,
        models.Library.name == lib.name).first(), "library in header")
    model: models.Library = models.Library.make_model(lib.__dict__)
    header._libraries.append(model)
    session.commit()
    res = contract.Id(**model.to_dict())
    return res

@app.delete("/delete/header/library/id={library_id}")
def delete_library(library_id: int) -> None:
    session = get_session()
    lib: models.Library = session.query(models.Library).get(library_id)
    not_found_except(lib, "library")
    session.delete(lib)
    session.commit()


# Function_NodeArg
    

@app.get("/get/function/node/arg/id={arg_id}/")
def get_function_node_arg(arg_id: int) -> contract.Function_NodeArgLinks:
    session = get_session()
    arg: models.Function_NodeArg = session.query(models.Function_NodeArg).get(arg_id)
    not_found_except(arg, "arg")
    session.commit()
    res = contract.Function_NodeArgLinks(**arg.to_dict())
    return res

@app.get("/get/function/id={function_id}/node/id={node_id}/arg/")
def get_function_node_args(function_id: int, node_id: int) -> list[contract.Function_NodeArgId]:
    session = get_session()
    not_found_except(session.query(models.Function).get(function_id), "function")
    not_found_except(session.query(models.Node).get(node_id), "node")
    fun_node = session.query(models.Function_Node).filter(
        models.Function_Node.function_id == function_id,
        models.Function_Node.node_id == node_id).first()
    not_found_except(fun_node, "not with function")
    session.commit()
    res = list(map(lambda x: contract.Function_NodeArgId(**x.to_dict()), fun_node._args))
    return res

@app.post("/post/function/id={function_id}/node/id={node_id}/arg/")
def post_function_node_arg(function_id: int, 
                           node_id: int, 
                           arg: contract.BaseFunction_NodeArg) -> contract.Id:
    
    session = get_session()
    not_found_except(session.query(models.Function).get(function_id), "function")
    not_found_except(session.query(models.Node).get(node_id), "node")
    fun_node = session.query(models.Function_Node).filter(
        models.Function_Node.function_id == function_id,
        models.Function_Node.node_id == node_id).first()
    not_found_except(fun_node, "not with function")
    model: models.Function_NodeArg = models.Function_NodeArg.make_model(arg.__dict__)
    fun_node._args.append(model)
    session.commit()
    res = contract.Id(**model.to_dict())
    return res

@app.delete("/delete/function/node/arg/id={arg_id}/")
def delete_function_node_arg(arg_id: int) -> None:
    session = get_session()
    arg = session.query(models.Function_NodeArg).get(arg_id)
    not_found_except(arg, "arg")
    session.delete(arg)
    session.commit()


# GlobalVar
    

@app.get("/get/header/global_var/id={global_var_id}/")
def get_global_var(global_var_id: int) -> contract.GlobalVarLinks:
    session = get_session()
    global_var: models.GlobalVar = session.query(models.GlobalVar).get(global_var_id)
    not_found_except(global_var, "global var")
    session.commit()
    res = contract.GlobalVarLinks(**global_var.to_dict())
    return res

@app.get("/get/header/id={header_id}/global_var/")
def get_global_vars_by_header(header_id: int) -> list[contract.GlobalVarId]:
    session = get_session()
    header: models.Header = session.query(models.Header).get(header_id)
    not_found_except(header, "header")
    session.commit()
    res = list(map(lambda x: contract.GlobalVarId(**x.to_dict()), header._globalVars))
    return res

# @app.get("/get/header/global_var/")
# def get_global_vars() -> list[contract.Res_GetGlobalVars_Item]:
#     session = get_session()
#     global_vars = session.query(models.GlobalVarModel).all()
#     session.commit()
#     res = list(map(lambda x: contract.Res_GetGlobalVars_Item(**x.to_dict()), global_vars))
#     return res

@app.post("/post/header/id={header_id}/global_var/")
def post_global_var(header_id: int, global_var: contract.BaseGlobalVar) -> contract.Id:
    session = get_session()
    header: models.Header = session.query(models.Header).get(header_id)
    not_found_except(header, "header")
    already_exists_except(session.query(models.GlobalVar).filter(
        models.Library.header_id == header_id,
        models.GlobalVar.name == global_var.name).first(), "global var in header")
    global_var_model: models.GlobalVar = models.GlobalVar.make_model(global_var.__dict__)
    header._globalVars.append(global_var_model)
    session.commit()
    res = contract.Id(**global_var_model.to_dict())
    return res

@app.delete("/delete/header/global_var/id={global_var_id}")
def delete_global_var(global_var_id: int) -> None:
    session = get_session()
    global_var: models.GlobalVar = session.query(models.GlobalVar).get(global_var_id)
    not_found_except(global_var, "global var")
    session.query(models.GlobalVarArg).filter(
        models.GlobalVarArg.globalVar_id == global_var_id).delete()
    session.delete(global_var)
    session.commit()


# Program ---------------------------------------------------------------- continue


# @app.get("/get/function/id={function_id}/")
# def get_function(function_id: int) -> contract.BaseFunction:
#     session = get_session()
#     fun: models.Function = session.query(models.Function).get(function_id)
#     if fun is None:
#         raise HTTPException(
#             status.HTTP_404_NOT_FOUND,
#             "function not found"
#         )
#     session.commit()
#     res = contract.BaseFunctionId(**fun.to_dict())
#     return res

# @app.get("/get/function/")
# def get_functions() -> list[contract.]:
#     session = get_session()
#     fun = session.query(models.FunctionModel).all()
#     session.commit()
#     res = list(map(lambda x: contract.Res_GetAllFunction_Item(**x.to_dict()), fun))
#     return res

# @app.get("/get/header/id={header_id}/function/")
# def get_header_functions(header_id: int) -> list[contract.Res_GetHeaderFunctions_Item]:
#     session = get_session()
#     header: models.HeaderModel = session.query(models.HeaderModel).get(header_id)
#     if header is None:
#         raise HTTPException(
#             status.HTTP_404_NOT_FOUND,
#             "header not found"
#         )
#     funs = session.query(models.FunctionModel).filter(
#         models.HeaderModel.id == header_id,
#         models.HeaderModel.id == models.Function_HeaderModel.header_id,
#         models.
#         )
#     session.commit()


# @app.post("/post/function/")
# def post_function(fun: contract.Req_PostFunction) -> contract.Res_PostFunction:
#     session = get_session()
#     model = session.query(models.FunctionModel).filter(
#         models.FunctionModel.name == fun.name).first()
#     if model is not None:
#         raise HTTPException(
#             status.HTTP_400_BAD_REQUEST,
#             f"function \"{model.name}\" already exists (id: {model.id})"
#         )

#     model: models.FunctionModel = models.FunctionModel.make_model(fun.__dict__)
#     session.add(model)
#     session.commit()
#     res = contract.Res_PostFunction(**model.to_dict())
#     return res

# @app.delete("/delete/function/id={function_id}/")
# def delete_function(function_id: int) -> None:
#     session = get_session()
#     fun = session.query(models.FunctionModel).get(function_id)
#     if fun is None:
#         raise HTTPException(
#             status.HTTP_404_NOT_FOUND,
#             "function not found"
#         )
#     session.query(models.FunctionParamModel).filter(
#         models.FunctionParamModel.function_id == function_id).delete()
#     session.delete(fun)
#     session.commit()


# # global_var_args
    

# @app.get("/get/header/global_var/arg/id={global_var_arg_id}/")
# def get_global_var_arg(global_var_arg_id: int) -> contract.Res_GetGlobalVarArg:
#     session = get_session()
#     global_var_arg: models.GlobalVarArgModel = session.query(models.GlobalVarArgModel).get(global_var_arg_id)
#     if global_var_arg is None:
#         raise HTTPException(
#             status.HTTP_404_NOT_FOUND,
#             "global var arg not found"
#         )
#     session.commit()
#     res = contract.Res_GetGlobalVarArg(**global_var_arg.to_dict())
#     return res

# @app.get("/get/header/global_var/id={global_var_id}/arg/")
# def get_global_var_global_var_args(global_var_id: int) -> list[contract.Res_GetGlobalVarArgs_Item]:
#     session = get_session()
#     global_var: models.GlobalVarModel = session.query(models.GlobalVarModel).get(global_var_id)
#     if global_var is None:
#         raise HTTPException(
#             status.HTTP_404_NOT_FOUND,
#             "global var not found"
#         )
#     session.commit()
#     res = list(map(lambda x: contract.Res_GetGlobalVarArgs_Item(**x.to_dict()), global_var._args))
#     return res

# @app.get("/get/header/global_var/arg")
# def get_global_var_args() -> list[contract.Res_GetGlobalVarArgs_Item]:
#     session = get_session()
#     global_var_args = session.query(models.GlobalVarArgModel).all()
#     session.commit()
#     res = list(map(lambda x: contract.Res_GetGlobalVarArgs_Item(**x.to_dict()), global_var_args))
#     return res

# @app.post("/post/header/global_var/id={global_var}/arg/")
# def post_global_var_arg(global_var_id: int, global_var_arg: contract.Req_PostGlobalVarArg) -> contract.Res_PostGlobalVarArg:
#     session = get_session()
#     global_var: models.GlobalVarModel = session.query(models.GlobalVarModel).get(global_var_id)
#     if global_var is None:
#         raise HTTPException(
#             status.HTTP_404_NOT_FOUND,
#             f"global var not found"
#         )
#     arg_model: models.GlobalVarArgModel = models.GlobalVarArgModel.make_model(global_var_arg.__dict__)
#     global_var._args.append(arg_model)
#     session.commit()
#     res = contract.Res_PostGlobalVarArg(**arg_model.to_dict())
#     return res

# @app.delete("/delete/header/global_var/arg/id={global_var_arg_id}")
# def delete_global_var(global_var_arg_id: int) -> None:
#     session = get_session()
#     global_var_arg: models.GlobalVarModel = session.query(models.GlobalVarModel).get(global_var_arg_id)
#     if global_var_arg is None:
#         raise HTTPException(
#             status.HTTP_404_NOT_FOUND,
#             "global var arg not found"
#         )
#     session.delete(global_var_arg)
#     session.commit()

