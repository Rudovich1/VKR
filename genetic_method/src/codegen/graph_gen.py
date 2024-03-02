from genetic_method.src.codegen.header_gen import GA, GAI, GAT

GAE = GA + "EvolutionTree::"


class Node(object):
    def __init__(
        self,
        id: str,
        gene_type: str,
        fitness: tuple[str, list[str]] | None = None,
        selection: tuple[str, list[str]] | None = None,
        mutation: tuple[str, list[str]] | None = None,
        crossingover: tuple[str, list[str]] | None = None,
        conditions_for_stopping: tuple[str, list[str]] | None = None,
        any: tuple[str, list[str]] | None = None,
        end_node: tuple[str, list[str]] | None = None,
        start_node: tuple[str, list[str]] | None = None,
        start_node_log: tuple[str, list[str]] | None = None,
        new_generation_log: tuple[str, list[str]] | None = None,
        end_node_log: tuple[str, list[str]] | None = None,
    ):
        self.id = id
        self.gene_type = gene_type
        self.fitness = fitness
        self.selection = selection
        self.mutation = mutation
        self.crossingover = crossingover
        self.conditions_for_stopping = conditions_for_stopping
        self.any = any
        self.end_node = end_node
        self.start_node = start_node
        self.start_node_log = start_node_log
        self.new_generation_log = new_generation_log
        self.end_node_log = end_node_log

    def set_gen_(self, fun: tuple[str, list[str]], set_name: str) -> str:
        return f"{self.id}->{set_name}({fun[0]}()({', '.join(fun[1])}));\n"

    def gen(self) -> str:
        return f"\
    {self.set_gen_(self.fitness, 'setFitnessFunction') if self.fitness is not None else ''}\
    {self.set_gen_(self.selection, 'setSelectionFunction') if self.selection is not None else ''}\
    {self.set_gen_(self.mutation, 'setMutationFunction') if self.mutation is not None else ''}\
    {self.set_gen_(self.crossingover, 'setCrossingoverFunction') if self.crossingover is not None else ''}\
    {self.set_gen_(self.conditions_for_stopping, 'setConditionsForStoppingFunction') if self.conditions_for_stopping is not None else ''}\
    {self.set_gen_(self.any, 'setAnyFunction') if self.any is not None else ''}\
    {self.set_gen_(self.end_node, 'setEndNode') if self.end_node is not None else ''}\
    {self.set_gen_(self.start_node, 'setStartNode') if self.start_node is not None else ''}\
    {self.set_gen_(self.start_node_log, 'setStartNodeLog') if self.start_node_log is not None else ''}\
    {self.set_gen_(self.new_generation_log, 'setNewGenerationLog') if self.new_generation_log is not None else ''}\
    {self.set_gen_(self.end_node_log, 'setEndNodeLog') if self.end_node_log is not None else ''}\n"


class UnaryNode(Node):
    def __init__(self, next_node: Node, **kwargs):
        super.__init__(**kwargs)
        self.next_node = next_node

    def gen(self) -> str:
        return f"""\
{self.next_node.gen()}
    std::unique_ptr<{GAE}UnaryNode<{self.gene_type}>> {self.id} = 
        std::make_unique<{GAE}UnaryNode<{self.gene_type}>>(\"{self.id}\", std::move({self.next_node.id}));
{super().gen()}
"""


class K_Node(Node):
    def __init__(
        self,
        next_nodes: list[Node],
        pooling_populations: tuple[str, list[str]] | None = None,
        **kwargs,
    ):
        super().__init__(**kwargs)
        self.next_nodes = next_nodes
        self.pooling_populations = pooling_populations

    def gen(self) -> str:
        return f"""\
{''.join(map(lambda x: x.gen(), self.next_nodes))}
    std::array<std::unique_ptr<{GAE}Node<{self.gene_type}>>, {len(self.next_nodes)}> _NODES_FOR_{self.id} = {{
        {', '.join(map(lambda x: f'std::move({x.id})', self.next_nodes))}}};
    std::unique_ptr<{GAE}K_Node<{self.gene_type}, {len(self.next_nodes)}>> {self.id} = 
        std::make_unique<{GAE}K_Node<{self.gene_type}, {len(self.next_nodes)}>>
            (\"{self.id}\", std::move(_NODES_FOR_{self.id}));
    {self.name}.setPoolingPopulations({self.pooling_populations[0]}<{len(self.pooling_populations)}>()({', '.join(self.pooling_populations[1])}));"
{super().gen()}
"""


class PopulationNode(Node):
    def __init__(self, start_population: tuple[str, list[str]] | None = None, **kwargs):
        super().__init__(**kwargs)
        self.start_population = start_population

    def gen(self) -> str:
        return f"""\
    std::unique_ptr<{GAE}PopulationNode<{self.gene_type}>> {self.id} = 
        std::make_unique<{GAE}PopulationNode<{self.gene_type}>>(\"{self.id}\");
    {self.set_gen_(self.start_population, "setStartPopulationFunction") if self.start_population is not None else ''}\
{super().gen()}
"""
