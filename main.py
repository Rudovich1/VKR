from genetic_method.API.api import start
from genetic_method.function_store.db import init_db

if __name__ == "__main__":
    init_db()
    start()