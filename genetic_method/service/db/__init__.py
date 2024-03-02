from sqlalchemy import create_engine
from sqlalchemy.orm import Session
from genetic_method.service.db.models import Base
from pathlib import Path


DB_PATH = "/genetic_method/service/db/database.db"
DB_URL = f"sqlite://{DB_PATH}"


engine = create_engine(DB_URL.__str__(), echo=True, pool_timeout=30)


if not Path(DB_PATH).is_file():
    with Session(engine) as session:
        with session.begin():
            Base.metadata.create_all(engine)
