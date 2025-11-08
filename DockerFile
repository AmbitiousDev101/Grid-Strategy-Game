# Stage 1: Build
FROM gcc:latest AS builder
WORKDIR /app
COPY ex7q1.c ex7q1.h ./
RUN gcc -o monopoly ex7q1.c -Wall -Wextra -pedantic

# Stage 2: Runtime
FROM ubuntu:22.04
WORKDIR /app
COPY --from=builder /app/monopoly .
CMD ["./monopoly"]

# docker build -t monopoly-game
# docker run -it monopoly-game
