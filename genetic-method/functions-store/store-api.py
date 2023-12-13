import json
import pathlib
import argparse


STORE_PATH = pathlib.Path("genetic-method\\functions-store")

class Object:
    def toJSON(self):
        return json.dumps(self, default=lambda o: o.__dict__, 
            sort_keys=True, indent=4)

class Function(Object):
    def __init__(
            self,
            name: str,
            gene_type: str,
            const_ref_vars: list[tuple[str, str]],
            const_vars: list[tuple[str, str]],
            ref_vars: list[tuple[str, str]],
            vars: list[tuple[str, str]],
            code: str,
            type: str):
        self.name = name
        self.gene_type = gene_type
        self.const_ref_vars = const_ref_vars
        self.const_vars = const_vars
        self.ref_vars = ref_vars
        self.vars = vars
        self.code = code
        self.type = type

    def __eq__(self, obj) -> bool:
        if (type(obj) == Function):
            return self.name == obj.name
        elif (type(obj) == str):
            return self.name == obj


def get_functions(store_path: pathlib.Path = STORE_PATH.joinpath("store.json")) -> list[Function]:
    json_file = json.load(open(store_path.absolute(), "r"))
    return list(map(lambda x: Function(**x), json_file))


def info(args):
    functions = get_functions()
    res = []
    for fun in functions:
        res.append((fun.type, fun.gene_type, fun.name))
    print(res)
    # print("Info:", "...")


def add(args):
    json_path = pathlib.Path(args.json_path)
    json_file = open(json_path.absolute(), "r")
    new_function_js = json.load(json_file)
    json_file.close()
    new_function = Function(**new_function_js)
    functions = get_functions()

    if (new_function in functions):
        raise Exception(f"function {new_function.name} already exists in store")
    functions.append(new_function)
    
    store = open(STORE_PATH.joinpath("store.json").absolute(), "w")
    store.write(json.dumps(list(map(lambda x: x.__dict__, functions)), indent=4, sort_keys=True))
    store.close()


def remove(args):
    function_name = str(args.function_name)
    functions = get_functions()
    try:
        functions.remove(function_name)
    except ValueError:
        raise ValueError(f"function {function_name} not in store")

    store = open(STORE_PATH.joinpath("store.json").absolute(), "w")
    store.write(json.dumps(list(map(lambda x: x.__dict__, functions)), indent=4, sort_keys=True))
    store.close()

def main():

    main_parser = argparse.ArgumentParser(
        prog        = "store-api",
        description = "API for genetic algorithm functions store"
    )
    subparsers = main_parser.add_subparsers()

    parser_add = subparsers.add_parser(
        name    = "add",
        help    = "add new function",
    )
    parser_add.add_argument(
        dest            = "json_path",
        type            = str,
        help            = "path to json with data"
    )
    parser_add.set_defaults(func=add)

    parser_info = subparsers.add_parser(
        name    = "info",
        help    = "list of existing functions"
    )
    parser_info.set_defaults(func=info)

    parser_remove = subparsers.add_parser(
        name    = "remove",
        help    = "remove function"
    )
    parser_remove.add_argument(
        dest            = "function_name",
        type            = str,
        help            = "function name"
    )
    parser_remove.set_defaults(func=remove)

    args = main_parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()