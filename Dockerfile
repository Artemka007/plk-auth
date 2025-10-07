FROM gcc:latest

# Install PostgreSQL client libraries and libpqxx
RUN apt-get update && apt-get install -y \
    libpq-dev \
    libpqxx-dev \
    build-essential \
    postgresql-client

# Set the working directory inside the container
WORKDIR /app

# Copy your C++ source code
COPY . /app

# Compile your C++ application
RUN g++ -std=c++17 main.cpp -o app -lpqxx -lpq

# Make sure the binary is executable
RUN chmod +x app

# Create a startup script
RUN echo '#!/bin/bash\n\
until pg_isready -h postgres -p 5432 -U postgres; do\n\
  echo "Waiting for PostgreSQL..."\n\
  sleep 2\n\
done\n\
echo "PostgreSQL is ready!"\n\
./app' > /app/start.sh

RUN chmod +x /app/start.sh

CMD ["/app/start.sh"]