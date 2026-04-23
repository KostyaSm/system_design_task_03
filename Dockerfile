FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    build-essential cmake git libpq-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .
RUN mkdir -p build && cd build && cmake .. && make -j$(nproc)

FROM ubuntu:22.04
RUN apt-get update && apt-get install -y libpq5 && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=builder /app/build/service /app/service
COPY --from=builder /app/configs /app/configs

EXPOSE 8080
CMD ["/app/service", "-c", "/app/configs/config.yaml"]