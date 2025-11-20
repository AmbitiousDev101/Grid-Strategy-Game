FROM gcc:latest AS builder

WORKDIR /app

COPY src/ ./src/
COPY Makefile ./

RUN make

FROM ubuntu:22.04

RUN apt-get update && \
    apt-get install -y --no-install-recommends ca-certificates && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/monopoly .

ENTRYPOINT ["./monopoly"]
