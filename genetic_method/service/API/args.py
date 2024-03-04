import argparse


def function_arg_parser(fun_parser: argparse.ArgumentParser):
    sub_parser: argparse._SubParsersAction = fun_parser.add_subparsers("functions subcommands")
    get: argparse.ArgumentParser = sub_parser.add_parser("get")
    add: argparse.ArgumentParser = sub_parser.add_parser("add")
    remove: argparse.ArgumentParser = sub_parser.add_parser("remove")
    
    get.add_argument("--name", type=str, default=None)
    get.add_argument("--type", type=str, default=None)
    get.add_argument("--gene_type", type=str, default=None)
    get.add_argument("--node", type=str, default=None)
    
    add.add_argument("")
    
    return fun_parser


def node_arg_parser(node_parser: argparse.ArgumentParser):
    sub_parser: argparse._SubParsersAction = node_parser.add_subparsers("nodes subcommands")
    sub_parser.add_parser("get")
    sub_parser.add_parser("add")
    sub_parser.add_parser("remove")
    return node_parser


def graph_arg_parser(graph_parser: argparse.ArgumentParser):
    sub_parser: argparse._SubParsersAction = graph_parser.add_subparsers("graphs subcommands")
    sub_parser.add_parser("get")
    sub_parser.add_parser("add")
    sub_parser.add_parser("remove")
    return graph_parser


def program_arg_parser(program_parser: argparse.ArgumentParser):
    sub_parser: argparse._SubParsersAction = program_parser.add_subparsers("programs subcommands")
    sub_parser.add_parser("get")
    sub_parser.add_parser("add")
    sub_parser.add_parser("remove")
    return program_parser


def link_arg_parser(links_parser: argparse.ArgumentParser):
    sub_parser: argparse._SubParsersAction = links_parser.add_subparsers("links subcommands")
    sub_parser.add_parser("add")
    sub_parser.add_parser("remove")
    return links_parser


def main_arg_parser():
    parser = argparse.ArgumentParser("Genetic method")
    sub_parsers: argparse._SubParsersAction = parser.add_subparsers("subcommands")
    sub_parsers.add_parser("functions")
    sub_parsers.add_parser("nodes")
    sub_parsers.add_parser("graphs")
    sub_parsers.add_parser("programs")
    sub_parsers.add_parser("links")
    return parser