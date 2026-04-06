FROM python:3.11-slim

WORKDIR /app

RUN pip install --no-cache-dir -r requirements.txt

# Переменная окружения для PostgreSQL
ENV DATABASE_URL=postgresql://user:password@db:5432/ozon_store

EXPOSE 8000

CMD ["python", "main.py"]