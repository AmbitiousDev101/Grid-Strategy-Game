# Stage 1: Build
FROM gcc:latest AS builder
WORKDIR /app
COPY sim.c sim_header.h ./
RUN gcc -o monopoly sim.c -Wall -Wextra -pedantic

# Stage 2: Runtime
FROM ubuntu:22.04
WORKDIR /app
COPY --from=builder /app/monopoly .
CMD ["./monopoly"]
