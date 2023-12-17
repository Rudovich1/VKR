# import json
# from pathlib import Path
# from genetic_method.function_store.controller import add_function
# from genetic_method.function_store import contract

# STARTUP_PATH = Path("genetic_method\\function_store\\startup_data\\startup.json")


# def startup_db(path: Path = STARTUP_PATH):
#     with open(path.absolute()) as file:
#         json_data: list[dict] = json.load(file)

#     funs = list(map(lambda x: contract.MainFunction(**x), json_data))
#     for fun in funs:
#         add_function(fun)


# TODO