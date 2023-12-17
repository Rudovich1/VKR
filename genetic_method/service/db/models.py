from sqlalchemy.orm import Mapped, mapped_column, DeclarativeBase
from sqlalchemy.orm import relationship
from sqlalchemy import ForeignKey


class Id:
    id: Mapped[int] = mapped_column(primary_key=True, unique=True, autoincrement=True)

    def to_dict(self) -> dict[str, ]:
        res = {
            "id": self.id
        }
        return res


class Base(DeclarativeBase):
    def _get_ids(models: list["Base"], field: str = "id") -> list[int]:
        return list(map(lambda x: x.__dict__[field], models))
    
    def make_model(data: dict[str, ]):
        return Base()


class FunctionParam(Base, Id):
    __tablename__ = "FunctionParam"

    name:           Mapped[str]     = mapped_column()
    type:           Mapped[str]     = mapped_column()
    is_const:       Mapped[bool]    = mapped_column()
    is_ref:         Mapped[bool]    = mapped_column()
    function_id:    Mapped[int]     = mapped_column(ForeignKey("Function.id"))

    _function:  Mapped["Function"] = relationship(back_populates="_params")

    def make_model(data: dict[str, ]) -> "FunctionParam":
        return FunctionParam(
            name        = data["name"],
            type        = data["type"],
            is_const    = data["is_const"],
            is_ref      = data["is_ref"]
        )   
    
    def to_dict(self) -> dict[str, ]:
        res = super().to_dict()
        res.update({
            "name":         self.name,
            "type":         self.type,
            "is_const":     self.is_const,
            "is_ref":       self.is_ref,
            "function_id":  self.function_id
        })
        return res


class GlobalVarArg(Base, Id):
    __tablename__ = "GlobalVarArg"

    arg:            Mapped[str] = mapped_column()
    globalVar_id:   Mapped[int] = mapped_column(ForeignKey("GlobalVar.id"))

    _globalVar: Mapped["GlobalVar"] = relationship(back_populates="_args")

    def make_model(data: dict[str, ]) -> "GlobalVarArg":
        return GlobalVarArg(
            arg = data["arg"]
        )
    
    def to_dict(self) -> dict[str, ]:
        res = super().to_dict()
        res.update({
            "arg":          self.arg,
            "globalVar_id": self.globalVar_id
        })
        return res
    

class Library(Base, Id):
    __tablename__ = "Library"

    name:       Mapped[str]     = mapped_column()
    is_stl:     Mapped[bool]    = mapped_column()
    header_id:  Mapped[int]     = mapped_column(ForeignKey("Header.id"))

    _header:    Mapped["Header"]    = relationship(back_populates="_libraries")

    def make_model(data: dict[str, ]) -> "Library":
        return Library(
            name    = data["name"],
            is_stl  = data["is_stl"]
        )
    
    def to_dict(self) -> dict[str, ]:
        res = super().to_dict()
        res.update({
            "name":         self.name,
            "is_stl":       self.is_stl,
            "header_id":    self.header_id
        })
        return res


class Function_NodeArg(Base, Id):
    __tablename__ = "Function_NodeArg"

    arg:                Mapped[str] = mapped_column()
    function_node_id:   Mapped[int] = mapped_column(ForeignKey("Function_Node.id"))

    _function_node: Mapped["Function_Node"] = relationship(back_populates="_args")

    def make_model(data: dict[str, ]) -> "Function_NodeArg":
        return Function_Node(
            arg = data["arg"]
        )
    
    def to_dict(self) -> dict[str, ]:
        res = super().to_dict()
        res.update({
            "arg":          self.arg,
            "function_id":  self._function_node.function_id,
            "node_id":      self._function_node.node_id
        })
        return res


class GlobalVar(Base, Id):
    __tablename__ = "GlobalVar"

    name:       Mapped[str]         = mapped_column()
    type:       Mapped[str]         = mapped_column()
    is_const:   Mapped[bool]        = mapped_column()
    header_id:  Mapped[int]         = mapped_column(ForeignKey("Header.id"))

    _header:    Mapped["Header"]               = relationship(back_populates="_globalVars")
    _args:      Mapped[list["GlobalVarArg"]]   = relationship(back_populates="_globalVar")

    def make_model(data: dict[str, ]) -> "GlobalVar":
        return GlobalVar(
            name        = data["name"],
            type        = data["type"],
            is_const    = data["is_const"]
        )
    
    def to_dict(self) -> dict[str, ]:
        res = super().to_dict()
        res.update({
            "name":         self.name,
            "type":         self.type,
            "is_const":     self.is_const,
            "header_id":    self.header_id,
            "arg_ids":      Base._get_ids(self._args)
        })
        return res


class Function_Header(Base, Id):
    __tablename__ = "Function_Header"

    function_id:    Mapped[int] = mapped_column(ForeignKey("Function.id"))
    header_id:      Mapped[int] = mapped_column(ForeignKey("Header.id"))

    _function:  Mapped["Function"] = relationship(back_populates="_header_associations")
    _header:    Mapped["Header"]   = relationship(back_populates="_function_associations")

    def make_model(data: dict[str, ]) -> "Function_Header":
        return Function_Header()
    
    def to_dict(self) -> dict[str, ]:
        res = {
            "function_id":  self.function_id,
            "header_id":    self.header_id
        }
        return res


class Function_Node(Base, Id):
    __tablename__ = "Function_Node"

    function_id:    Mapped[int] = mapped_column(ForeignKey("Function.id"))
    node_id:        Mapped[int] = mapped_column(ForeignKey("Node.id"))

    _function:  Mapped["Function"]                  = relationship(back_populates="_node_associations")
    _node:      Mapped["Node"]                      = relationship(back_populates="_function_associations")
    _args:      Mapped[list["Function_NodeArg"]]   = relationship(back_populates="_function_node")

    def make_model(data: dict[str, ]) -> "Function_Node":
        return Function_Node()
    
    def to_dict(self) -> dict[str, ]:
        res = {
            "function_id":  self.function_id,
            "node_id":      self.node_id,
            "arg_ids":      Base._get_ids(self._args)
        }
        return res


class Node_Graph(Base, Id):
    __tablename__ = "Node_Graph"

    parent_node_name:   Mapped[str] = mapped_column(nullable=True)
    node_id:            Mapped[int] = mapped_column(ForeignKey("Node.id"))
    graph_id:           Mapped[int] = mapped_column(ForeignKey("Graph.id"))

    _node:  Mapped["Node"]  = relationship(back_populates="_graph_associations")
    _graph: Mapped["Graph"] = relationship(back_populates="_node_associations")

    def make_model(data: dict[str, ]) -> "Node_Graph":
        return Node_Graph(
            parent_node_name    = data["parent_node_name"]
        )
    
    def to_dict(self) -> dict[str, ]:
        res = {
            "parent_node_name": self.parent_node_name,
            "node_id":          self.node_id,
            "graph_id":         self.graph_id
        }
        return res


class Program(Base, Id):
    __tablename__ = "Program"

    name:       Mapped[str] = mapped_column(unique=True)
    header_id:  Mapped[int] = mapped_column(ForeignKey("Header.id"))
    graph_id:   Mapped[int] = mapped_column(ForeignKey("Graph.id"))

    _header:    Mapped["Header"]   = relationship(back_populates="_programs")
    _graph:     Mapped["Graph"]    = relationship(back_populates="_programs")

    def make_model(data: dict[str, ]) -> "Program":
        return Program(
            name    = data["name"]
        )
    
    def to_dict(self) -> dict[str, ]:
        res = super().to_dict()
        res.update({
            "name":         self.name,
            "header_id":    self.header_id,
            "graph_id":     self.graph_id
        })
        return res    


class Function(Base, Id):
    __tablename__ = "Function"

    name:       Mapped[str] = mapped_column(unique=True)
    type:       Mapped[str] = mapped_column()
    gene_type:  Mapped[str] = mapped_column()
    code:       Mapped[str] = mapped_column()

    _nodes:     Mapped[list["Node"]]            = relationship(secondary="Function_Node", back_populates="_functions")
    _params:    Mapped[list["FunctionParam"]]   = relationship(back_populates="_function")
    _headers:   Mapped[list["Header"]]          = relationship(secondary="Function_Header", back_populates="_functions")
    
    _node_associations:     Mapped[list["Function_Node"]]   = relationship(back_populates="_function")
    _header_associations:   Mapped[list["Function_Header"]] = relationship(back_populates="_function")

    def make_model(data: dict[str, ]) -> "Function":
        return Function(
            name        = data["name"],
            type        = data["type"],
            gene_type   = data["gene_type"],
            code        = data["code"]
        )
    
    def to_dict(self) -> dict[str, ]:
        res = super().to_dict()
        res.update({
            "name":         self.name,
            "type":         self.type,
            "gene_type":    self.gene_type,
            "code":         self.code,
            "node_ids":     Base._get_ids(self._nodes),
            "param_ids":    Base._get_ids(self._params),
            "header_ids":   Base._get_ids(self._headers)
        })
        return res


class Node(Base, Id):
    __tablename__ = "Node"

    name:       Mapped[str] = mapped_column(unique=True)
    type:       Mapped[str] = mapped_column()
    gene_type:  Mapped[str] = mapped_column()

    _functions: Mapped[list["Function"]]    = relationship(secondary="Function_Node", back_populates="_nodes")
    _graphs:    Mapped[list["Graph"]]       = relationship(secondary="Node_Graph", back_populates="_nodes")

    _function_associations: Mapped[list["Function_Node"]]   = relationship(back_populates="_node")
    _graph_associations:    Mapped[list["Node_Graph"]]      = relationship(back_populates="_node")

    def make_model(data: dict[str, ]) -> "Node":
        return Node(
            name        = data["name"],
            type        = data["type"],
            gene_type   = data["gene_type"]
        )
    
    def to_dict(self) -> dict[str, ]:
        res = super().to_dict()
        res.update({
            "name":         self.name,
            "type":         self.type,
            "gene_type":    self.gene_type,
            "function_ids": Base._get_ids(self._functions),
            "graph_ids":    Base._get_ids(self._graphs)
        })
        return res


class Header(Base, Id):
    __tablename__ = "Header"

    name:   Mapped[str] = mapped_column(unique=True)

    _programs:      Mapped[list["Program"]]     = relationship(back_populates="_header")
    _libraries:     Mapped[list["Library"]]     = relationship(back_populates="_header")
    _globalVars:    Mapped[list["GlobalVar"]]   = relationship(back_populates="_header")
    _functions:     Mapped[list["Function"]]    = relationship(secondary="Function_Header", back_populates="_headers")

    _function_associations: Mapped[list["Function_Header"]] = relationship(back_populates="_header")

    def make_model(data: dict[str, ]) -> "Header":
        return Header(
            name    = data["name"]
        )
    
    def to_dict(self) -> dict[str, ]:
        res = super().to_dict()
        res.update({
            "name":             self.name,
            "program_ids":      Base._get_ids(self._programs),
            "library_ids":      Base._get_ids(self._libraries),
            "globalVar_ids":   Base._get_ids(self._globalVars),
            "function_ids":     Base._get_ids(self._functions)
        })
        return res


class Graph(Base, Id):
    __tablename__ = "Graph"

    name:   Mapped[str] = mapped_column(unique=True)

    _programs:  Mapped[list["Program"]] = relationship(back_populates="_graph")
    _nodes:     Mapped[list["Node"]]    = relationship(secondary="Node_Graph", back_populates="_graphs")

    _node_associations: Mapped[list["Node_Graph"]]  = relationship(back_populates="_graph")

    def make_model(data: dict[str, ]) -> "Graph":
        return Graph(
            name    = data["name"]
        )
    
    def to_dict(self) -> dict[str, ]:
        res = super().to_dict()
        res.update({
            "name":         self.name,
            "program_ids":  Base._get_ids(self._programs),
            "node_ids":     Base._get_ids(self._nodes)
        })
        return res


