from pydantic import BaseModel, Field


class Var(BaseModel):
    name: str
    type: str
    is_const: bool
    is_ref: bool


class Function(BaseModel):
    name: str
    type: str
    gene_type: str
    code: str
    vars: list[Var]
