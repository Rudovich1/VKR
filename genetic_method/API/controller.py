from genetic_method.API import db_scripts
from genetic_method.API.db_scripts import FunctionModel, VarModel
from genetic_method.API import contract


def to_main_var(var: contract.Var) -> contract.MainVar:
    res = contract.MainVar(
        name = var.name,
        type = var.type,
        is_const = var.is_const,
        is_ref = var.is_ref
    )
    return res

def to_main_function(fun: contract.Function) -> contract.MainFunction:
    res = contract.MainFunction(
        name = fun.name,
        type = fun.type,
        code = fun.code,
        gene_type = fun.gene_type,
        vars = list(map(lambda x: to_main_var(x), fun.vars))
    )
    return res

def to_var(var: contract.MainVar) -> contract.Var:
    res = contract.Var(
        name = var.name,
        type = var.type,
        is_const = var.is_const,
        is_ref = var.is_ref,
        id = None,
        function_id = None
    )
    return res

def to_function(fun: contract.MainFunction) -> contract.Function:
    res = contract.Function(
        name = fun.name,
        type = fun.type,
        code = fun.code,
        gene_type = fun.gene_type,
        id = None,
        vars = list(map(lambda x: to_var(x), fun.vars))
    )
    return res


def get_functions() -> list[contract.Function]:
    funs = db_scripts.get_functions()
    return funs


def get_functions_by_type(type: str) -> list[contract.Function]:
    funs = db_scripts.get_functions_by_type(type)
    return funs


def get_function(name: str) -> contract.Function | None:
    fun = db_scripts.get_function(name)
    return fun


def add_function(fun: contract.MainFunction) -> bool:
    res = to_function(fun)
    return db_scripts.add_function(res)


def remove_function(name: str) -> None:
    db_scripts.remove_function(name)


def get_vars() -> list[contract.Var]:
    vars = db_scripts.get_vars()
    return vars
