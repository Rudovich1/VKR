import uvicorn


def start():
    uvicorn.run(
        "genetic_method.service.API.app:app",
        host='localhost',
        port=8080,
        reload=False)
    
    
    
