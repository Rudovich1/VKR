from genetic_method.API import db_scripts
from genetic_method.API.db_scripts import FunctionModel, VarModel
from genetic_method.API import contract


def get_functions() -> list[contract.Function]:
    funs = db_scripts.get_functions()
    return list(map(lambda x: contract.Function(**x), funs))


def get_functions_by_type(type: str) -> list[contract.Function]:
    funs = db_scripts.get_functions_by_type(type)
    return list(map(lambda x: contract. Function(**x), funs))


def get_function(name: str) -> contract.Function | None:
    fun = db_scripts.get_function(name)
    if (fun is None):
        return None
    return contract.Function(**fun.__dict__)


def add_function(fun: contract.Function) -> bool:
    model = FunctionModel(
        name=fun.name,
        type=fun.type,
        gene_type=fun.gene_type,
        code=fun.code)
    
    for var in fun.vars:
        model.vars.append(VarModel(**var.__dict__))

    return db_scripts.add_function(model)


def remove_function(name: str) -> None:
    db_scripts.remove_function(name)
