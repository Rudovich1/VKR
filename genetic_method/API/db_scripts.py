from sqlalchemy.orm import Session
from genetic_method.function_store.models import FunctionModel, VarModel
from genetic_method.function_store.db import engine
from genetic_method.API import contract


def to_contract_var(model: VarModel) -> contract.Var:
    res = contract.Var(
        id = model.id,
        name = model.name,
        type = model.type,
        is_const = model.is_const,
        is_ref = model.is_ref,
        function_id = model.function_id
    )
    return res

def to_contract_function(model: FunctionModel) -> contract.Function:
    res = contract.Function(
        id = model.id,
        name = model.name,
        type = model.type,
        gene_type = model.gene_type,
        code = model.code,
        vars = list(map(lambda x: to_contract_var(x), model.vars))
    )
    return res

def to_model_var(cont: contract.Var) -> VarModel:
    res = VarModel(
        id = cont.id,
        name = cont.name,
        type = cont.type,
        is_const = cont.is_const,
        is_ref = cont.is_ref,
        function_id = cont.function_id
    )
    return res

def to_model_function(cont: contract.Function) -> FunctionModel:
    res = FunctionModel(
        id = cont.id,
        name = cont.name,
        type = cont.type,
        code = cont.code,
        gene_type = cont.gene_type,
        vars = list(map(lambda x: to_model_var(x), cont.vars))
    )
    return res


def get_function(name: str) -> contract.Function | None:
    with Session(engine) as session:
        fun = session.query(FunctionModel).filter_by(name=name).first()
        if fun is None:
            res = None
        else:
            res = to_contract_function(fun)
        session.commit()
    return res


def get_functions_by_type(type: str) -> list[contract.Function]:
    with Session(engine) as session:
        funs = session.query(FunctionModel).filter_by(type=type).all()
        res = list(map(lambda x: to_contract_function(x), funs))
        session.commit()
    return res


def get_functions() -> list[contract.Function]:
    with Session(engine) as session:
        funs = session.query(FunctionModel).all()
        res = list(map(lambda x: to_contract_function(x), funs))
        session.commit()
    return res


def add_function(fun: contract.Function) -> bool:
    if (get_function(fun.name) is not None):
        return False
    with Session(engine) as session:
        model = to_model_function(fun)
        session.add(model)
        session.commit()
    return True


def remove_function(name: str) -> None:
    with Session(engine) as session:
        if (fun := session.query(FunctionModel).filter_by(name=name).first()) is not None:
            for var in fun.vars:
                session.delete(var)
            session.delete(fun)
        session.commit()


def get_vars() -> list[contract.Var]:
    with Session(engine) as session:
        vars = session.query(VarModel).all()
        res = list(map(lambda x: to_contract_var(x), vars))
        session.commit()
    return res
