# Monopolizing Salesperson

## Project Overview
This project is a turn-based strategy game written in C, managing two players competing across a dynamic grid of cities. It uses dynamic memory allocation and pointer-based data structures for efficient state and collision management. The Dockerfile provided uses a multi-stage build to compile and run the program inside an isolated Linux container.

## Contents
- ex7q1.c - Game source code
- ex7q1.h - Header file
- Dockerfile - Instructions to build the container image

## Usage

To build and run the Docker container for this project, use the following commands:

# docker build -t monopoly-game .

# docker run -it monopoly-game

The build command compiles the C program in a containerized environment using GCC and creates a lightweight runtime image. The run command launches the compiled game executable inside a Docker container, allowing you to interact with the game through your terminal.