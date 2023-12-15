from fastapi import FastAPI, HTTPException
from starlette import status
from genetic_method.API import controller
from genetic_method.API import contract
from genetic_method.API.startup_db import startup_db

app = FastAPI()

@app.get("/test")
async def test():
    return {"message": "Hello World"}


@app.on_event("startup")
async def startup():
    startup_db()


@app.get("/function/get_all_functions")
async def get_all_functions():
    return list(map(lambda x: x.__dict__, controller.get_functions()))


@app.get("/function/get_function/{name}")
async def get_function(name: str):
    fun = controller.get_function(name)
    if fun is None:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="function does not exist")
    return fun.__dict__


@app.get("/function/get_functions_by_type")
async def get_functions_by_type(type: contract.StoreFunction):
    return list(map(lambda x: x.__dict__, controller.get_functions_by_type(type)))


@app.post("/function/post_function")
async def post_function(fun: contract.MainFunction):
    res = controller.add_function(fun)
    if not res:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="function already exists")
    

@app.delete("/function/delete_function")
async def delete_function(name: str):
    controller.remove_function(name)


@app.get("/vars/get_all_vars")
async def gen_all_vars():
    return list(map(lambda x: x.__dict__, controller.get_vars()))
