import json
from pathlib import Path
from genetic_method.API.controller import add_function
from genetic_method.API import contract

STARTUP_PATH = Path("genetic_method\\function_store\\startup\\startup.json")
# TEST_STARTUP_PATH = Path("/genetic_method/function_store/startup/test_startup.json")



def startup_db(path: Path = STARTUP_PATH):
    with open(path.absolute()) as file:
        json_data: list[dict] = json.load(file)

    funs = list(map(lambda x: contract.MainFunction(**x), json_data))
    for fun in funs:
        add_function(fun)


# def get_test_startup_data() -> dict:
#     return get_startup_data(TEST_STARTUP_PATH)