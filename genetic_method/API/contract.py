from pydantic import BaseModel
from enum import Enum


class StoreFunction(str, Enum):
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


class MainVar(BaseModel):
    name: str
    type: str
    is_const: bool
    is_ref: bool


class Var(MainVar):
    id: int | None
    function_id: int | None


class MainFunction(BaseModel):
    name: str
    type: StoreFunction = StoreFunction.mutation
    gene_type: str
    code: str
    vars: list[MainVar]


class Function(MainFunction):
    id: int | None
