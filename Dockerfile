FROM gcc:latest

# Install PostgreSQL client libraries and libpqxx
RUN apt-get update && apt-get install -y \
    libpq-dev \
    libpqxx-dev \
    build-essential \
    postgresql-client \
    cmake

# Set the working directory inside the container
WORKDIR /app

# Copy your C++ source code
COPY . /app

# Create necessary directories if they don't exist
RUN mkdir -p src/dao src/db src/models src/utils

# Find all .cpp files in src subdirectories and compile them
RUN find /app/src -name "*.cpp" > /app/sources.txt

# Print found source files for debugging
RUN echo "Found source files:" && cat /app/sources.txt

# Compile your C++ application with all source files
RUN g++ -std=c++17 -I/app -I/app/src @/app/sources.txt -o app -lpqxx -lpq -lpthread

# Make sure the binary is executable
RUN chmod +x app

# Create a startup script
RUN echo '#!/bin/bash\n\
echo "Waiting for PostgreSQL to be ready..."\n\
until pg_isready -h postgres -p 5432 -U postgres; do\n\
  echo "Waiting for PostgreSQL..."\n\
  sleep 2\n\
done\n\
echo "PostgreSQL is ready!"\n\
echo "Starting C++ application..."\n\
./app' > /app/start.sh

RUN chmod +x /app/start.sh

CMD ["/app/start.sh"]