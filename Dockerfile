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

# Create src directory if it doesn't exist
RUN mkdir -p src

# Find all .cpp files in src and main directories and compile them
RUN find /app/src -name "*.cpp" -o -name "src/main.cpp" > /app/sources.txt

# Compile your C++ application with all source files
RUN g++ -std=c++17 -I/app @/app/sources.txt -o app -lpqxx -lpq -lpthread

# Alternative method without using file list (if above doesn't work)
# RUN g++ -std=c++17 -I/app src/*.cpp main.cpp -o app -lpqxx -lpq -lpthread

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