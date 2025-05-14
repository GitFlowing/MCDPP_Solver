FROM python:3.10-slim

# Update packages, install build-essential for compiling C extensions, g++ Compiler
# for C++ Solver and remove unnecessary files to reduce image size
RUN apt-get update && apt-get install -y \
    g++ \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Working directory
WORKDIR /app

# Copy requirements, app files and C++ source files
COPY requirements.txt .
COPY ./MCDPP_API /app
COPY ./BranchAndBoundProgrammGerman /build

# Compile C++ executable
RUN g++ -std=c++17 -O2 -o /app/mcdpp /build/*.cpp

# Install dependencies
RUN pip install --upgrade pip
RUN pip install --no-cache-dir -r requirements.txt

# Set c++ Solver executable
RUN chmod +x /app/mcdpp

# Start API
CMD uvicorn mcdpp_api:app --host 0.0.0.0 --port $PORT
