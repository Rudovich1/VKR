from sqlalchemy.orm import Session
from genetic_method.function_store.models import FunctionModel, StoreFunction, VarModel
from genetic_method.function_store.db import engine

def get_function(name: str) -> FunctionModel | None:
    with Session(engine) as session:
        with session.begin():
            return session.query(FunctionModel).filter_by(name=name).scalar()


def get_functions_by_type(type: StoreFunction) -> list[FunctionModel]:
    with Session(engine) as session:
        with session.begin():
            return session.query(FunctionModel).filter_by(type=type).all()


def get_functions() -> list[FunctionModel]:
    with Session(engine) as session:
        with session.begin():
            return session.query(FunctionModel).all()


def add_function(function: FunctionModel) -> bool:
    if (get_function(function.name) is not None):
        return False
    with Session(engine) as session:
        with session.begin():
            fun = FunctionModel(**function.__dict__)
            session.add(fun)
            return True


def remove_function(name: str) -> None:
    if res := get_function() is None:
        return
    with Session(engine) as session:
        with session.begin():
            session.delete(res)
