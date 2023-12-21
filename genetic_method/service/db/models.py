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
    function_id:    Mapped[int]     = mapped_column(ForeignKey("Function.id", ondelete="CASCADE"))

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
    global_var_id:   Mapped[int] = mapped_column(ForeignKey("GlobalVar.id", ondelete="CASCADE"))

    _global_var: Mapped["GlobalVar"] = relationship(back_populates="_args")

    def make_model(data: dict[str, ]) -> "GlobalVarArg":
        return GlobalVarArg(
            arg = data["arg"]
        )
    
    def to_dict(self) -> dict[str, ]:
        res = super().to_dict()
        res.update({
            "arg":          self.arg,
            "global_var_id": self.global_var_id
        })
        return res
    

class Library(Base, Id):
    __tablename__ = "Library"

    name:       Mapped[str]     = mapped_column()
    is_stl:     Mapped[bool]    = mapped_column()
    program_id: Mapped[int]     = mapped_column(ForeignKey("Program.id", ondelete="CASCADE"))

    _program:    Mapped["Program"]  = relationship(back_populates="_libraries")

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
            "program_id":   self.program_id
        })
        return res


class Function_NodeArg(Base, Id):
    __tablename__ = "Function_NodeArg"

    arg:                Mapped[str] = mapped_column()
    function_node_id:   Mapped[int] = mapped_column(ForeignKey("Function_Node.id", ondelete="CASCADE"))

    _function_node: Mapped["Function_Node"] = relationship(back_populates="_args")

    def make_model(data: dict[str, ]) -> "Function_NodeArg":
        return Function_NodeArg(
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
    program_id: Mapped[int]         = mapped_column(ForeignKey("Program.id", ondelete="CASCADE"))

    _program:    Mapped["Program"]             = relationship(back_populates="_global_vars")
    _args:      Mapped[list["GlobalVarArg"]]   = relationship(back_populates="_global_var", passive_deletes=True, cascade="all, delete")

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
            "program_id":   self.program_id,
            "arg_ids":      Base._get_ids(self._args)
        })
        return res


class Program_Graph(Base, Id):
    __tablename__ = "Program_Graph"

    program_id: Mapped[int] = mapped_column(ForeignKey("Program.id", ondelete="CASCADE"))
    graph_id:   Mapped[int] = mapped_column(ForeignKey("Graph.id", ondelete="CASCADE"))

    _program:   Mapped["Program"]   = relationship(back_populates="_graph_associations")
    _graph:     Mapped["Graph"]     = relationship(back_populates="_program_associations")

    def make_model(data: dict[str, ]):
        return Program_Graph()
    
    def to_dict(self) -> dict[str, ]:
        res = super().to_dict()
        res.update({
            "program_id":   self.program_id,
            "graph_id":     self.graph_id
        })
        return res


class Function_Node(Base, Id):
    __tablename__ = "Function_Node"

    function_id:    Mapped[int] = mapped_column(ForeignKey("Function.id", ondelete="CASCADE"))
    node_id:        Mapped[int] = mapped_column(ForeignKey("Node.id", ondelete="CASCADE"))

    _function:  Mapped["Function"]                  = relationship(back_populates="_node_associations")
    _node:      Mapped["Node"]                      = relationship(back_populates="_function_associations")
    _args:      Mapped[list["Function_NodeArg"]]    = relationship(back_populates="_function_node", passive_deletes=True, cascade="all, delete")

    def make_model(data: dict[str, ]) -> "Function_Node":
        return Function_Node()
    
    def to_dict(self) -> dict[str, ]:
        res = super().to_dict()
        res.update({
            "function_id":  self.function_id,
            "node_id":      self.node_id,
            "arg_ids":      Base._get_ids(self._args)
        })
        return res


class Node_Graph(Base):
    __tablename__ = "Node_Graph"

    id:                     Mapped[int] = mapped_column(primary_key=True, unique=True, autoincrement=True)
    name:                   Mapped[str] = mapped_column()
    parent_node_graph_id:   Mapped[int] = mapped_column(ForeignKey("Node_Graph.id", ondelete="CASCADE"), nullable=True)
    node_id:                Mapped[int] = mapped_column(ForeignKey("Node.id", ondelete="CASCADE"))
    graph_id:               Mapped[int] = mapped_column(ForeignKey("Graph.id", ondelete="CASCADE"))

    _parent_node_graph: Mapped["Node_Graph"]        = relationship(back_populates="_child_node_graphs", remote_side=[id])
    _child_node_graphs: Mapped[list["Node_Graph"]]  = relationship(back_populates="_parent_node_graph", passive_deletes=True, cascade="all, delete")
    _node:              Mapped["Node"]              = relationship(back_populates="_node_graphs")
    _graph:             Mapped["Graph"]             = relationship(back_populates="_node_graphs")

    def make_model(data: dict[str, ]) -> "Node_Graph":
        return Node_Graph(
            name = data["name"]
        )
    
    def to_dict(self) -> dict[str, ]:
        res = {
            "id":                   self.id,
            "name":                 self.name,
            "parent_node_graph_id": self.parent_node_graph_id,
            "node_id":              self.node_id,
            "graph_id":             self.graph_id
        }
        return res


class Function(Base, Id):
    __tablename__ = "Function"

    name:       Mapped[str] = mapped_column(unique=True)
    type:       Mapped[str] = mapped_column()
    gene_type:  Mapped[str] = mapped_column()
    code:       Mapped[str] = mapped_column()

    _nodes:     Mapped[list["Node"]]            = relationship(secondary="Function_Node", back_populates="_functions", viewonly=True)
    _params:    Mapped[list["FunctionParam"]]   = relationship(back_populates="_function", passive_deletes=True, cascade="all, delete")
    
    _node_associations:     Mapped[list["Function_Node"]]   = relationship(back_populates="_function", passive_deletes=True, cascade="all, delete")
    
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
            "param_ids":    Base._get_ids(self._params)
        })
        return res


class Node(Base, Id):
    __tablename__ = "Node"

    name:       Mapped[str] = mapped_column(unique=True)
    type:       Mapped[str] = mapped_column()
    gene_type:  Mapped[str] = mapped_column()

    _functions:     Mapped[list["Function"]]    = relationship(secondary="Function_Node", back_populates="_nodes", viewonly=True)
    _graphs:        Mapped[set["Graph"]]        = relationship(secondary="Node_Graph", back_populates="_nodes", viewonly=True)
    _node_graphs:   Mapped[list["Node_Graph"]]  = relationship(back_populates="_node", passive_deletes=True, cascade="all, delete")

    _function_associations: Mapped[list["Function_Node"]]   = relationship(back_populates="_node", passive_deletes=True, cascade="all, delete")

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


class Graph(Base, Id):
    __tablename__ = "Graph"

    name:   Mapped[str] = mapped_column(unique=True)

    _programs:          Mapped[list["Program"]]     = relationship(secondary="Program_Graph", back_populates="_graphs", viewonly=True)
    _nodes:             Mapped[set["Node"]]        = relationship(secondary="Node_Graph", back_populates="_graphs", viewonly=True)
    _node_graphs:       Mapped[list["Node_Graph"]]  = relationship(back_populates="_graph", passive_deletes=True, cascade="all, delete")

    _program_associations:  Mapped[list["Program_Graph"]]   = relationship(back_populates="_graph", passive_deletes=True, cascade="all, delete")

    def make_model(data: dict[str, ]) -> "Graph":
        return Graph(
            name    = data["name"]
        )
    
    def to_dict(self) -> dict[str, ]:
        res = super().to_dict()
        res.update({
            "name":             self.name,
            "program_ids":      Base._get_ids(self._programs),
            "node_ids":         Base._get_ids(self._nodes),
            "node_graph_ids:":  Base._get_ids(self._node_graphs)
        })
        return res


class Program(Base, Id):
    __tablename__ = "Program"

    name:       Mapped[str] = mapped_column(unique=True)

    _libraries:     Mapped[list["Library"]]     = relationship(back_populates="_program", passive_deletes=True, cascade="all, delete")
    _global_vars:   Mapped[list["GlobalVar"]]   = relationship(back_populates="_program", passive_deletes=True, cascade="all, delete")
    _graphs:        Mapped[list["Graph"]]       = relationship(secondary="Program_Graph", back_populates="_programs", viewonly=True)

    _graph_associations:    Mapped[list["Program_Graph"]]   = relationship(back_populates="_program", passive_deletes=True, cascade="all, delete")

    def make_model(data: dict[str, ]) -> "Program":
        return Program(
            name    = data["name"]
        )
    
    def to_dict(self) -> dict[str, ]:
        res = super().to_dict()
        res.update({
            "name":             self.name,
            "library_ids":      Base._get_ids(self._libraries),
            "global_var_ids":   Base._get_ids(self._global_vars),
            "graph_ids":        Base._get_ids(self._graphs)
        })
        return res    