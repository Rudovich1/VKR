from sqlalchemy import create_engine
from sqlalchemy.orm import Session
from genetic_method.function_store.models import Base
from pathlib import Path


DB_PATH = "/genetic_method/function_store/store.db"
DB_URL = f"sqlite://{DB_PATH}"

engine = create_engine(DB_URL.__str__(), echo=True) 


def init_db() -> None:   
    if not Path(DB_PATH).is_file():        
        with Session(engine) as session:
            with session.begin():
                Base.metadata.create_all(engine)
