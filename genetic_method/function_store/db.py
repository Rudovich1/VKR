from sqlalchemy import create_engine
from sqlalchemy.orm import Session
from genetic_method.function_store.models import Base

DB_URL = "sqlite:///genetic_method/function_store/store.db"

engine = create_engine(DB_URL, echo=True)


def init_db() -> None:
    with Session(engine) as session:
        with session.begin():
            Base.metadata.create_all(engine)


if __name__ == "__main__":
    init_db()