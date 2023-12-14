from enum import Enum
from sqlalchemy.orm import as_declarative, Mapped, mapped_column, DeclarativeBase
from sqlalchemy.orm import relationship
from sqlalchemy import ForeignKey

class StoreFunction(Enum):
    Fitness = 0,
    StartPopulation = 1,
    Selection = 2,
    ConditionsForStopping = 3,
    PoolingPopulations = 4,
    Mutation = 5,
    Crossingover = 6,
    Any = 7,
    EndNode = 8,
    StartNode = 9,
    NewGenerationLog = 10,
    StartNodeLog = 11,
    EndNodeLog = 12


class Base(DeclarativeBase):
    pass


class FunctionModel(Base):
    __tablename__ = "functions"
    id: Mapped[int] = mapped_column(primary_key=True, unique=True, autoincrement=True)
    name: Mapped[str] = mapped_column(unique=True,  nullable=False)
    gene_type: Mapped[str] = mapped_column(nullable=False)
    type: Mapped[StoreFunction] = mapped_column(nullable=False)
    code: Mapped[str] = mapped_column(nullable=False)
    vars: Mapped[list["VarModel"]] = relationship(back_populates="function", uselist=True)


class VarModel(Base):
    __tablename__ = "vars"
    id: Mapped[int] = mapped_column(primary_key=True, unique=True, autoincrement=True)
    type: Mapped[str] = mapped_column(nullable=False)
    name: Mapped[str] = mapped_column(nullable=False)
    is_const: Mapped[bool] = mapped_column(nullable=False)
    is_ref: Mapped[bool] = mapped_column(nullable=False)
    function_fk: Mapped[int] = mapped_column(ForeignKey("functions.id"))
    function: Mapped["FunctionModel"] = relationship(back_populates="vars", uselist=False)
