# Use a base image with a C++ compiler and build tools
FROM gcc:latest

# Install PostgreSQL client libraries and libpqxx
RUN apt-get update && apt-get install -y \
    libpq-dev \
    libpqxx-dev \
    build-essential

# Set the working directory inside the container
WORKDIR /app

# Copy your C++ source code
COPY . /app

# Compile your C++ application, linking against libpqxx and libpq
# Replace 'your_app.cpp' with your actual source file name
# Replace 'your_app' with your desired executable name
RUN g++ main.cpp -o your_app -lpqxx -lpq

# Command to run your application when the container starts
CMD ["./your_app"]