import argparse


def function_args(sub_parser: argparse._SubParsersAction):
    sub_parser.add_parser("function")
    sub_parser


def argument_parse():
    parser = argparse.ArgumentParser()
    parser.add_subparsers()


def main():
    pass