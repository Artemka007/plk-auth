cmake_minimum_required(VERSION 3.16)
project(UserManagementSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)


find_package(Boost 1.89.0 REQUIRED COMPONENTS system filesystem)

find_package(PostgreSQL 16.0 REQUIRED)

find_path(LIBPQXX_INCLUDE_DIR pqxx)
find_library(LIBPQXX_LIBRARY pqxx)
find_library(BCRYPT_LIBRARY bcrypt)


include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${LIBPQXX_INCLUDE_DIR}
    ${PostgreSQL_INCLUDE_DIRS}
)

file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*/*.cpp"
    "src/*/*/*.cpp"
)


add_executable(user_management_system ${SOURCES})

target_link_libraries(user_management_system
    Boost::system
    Boost::filesystem
    ${LIBPQXX_LIBRARY}
    PostgreSQL::PostgreSQL
    pthread
    ${BCRYPT_LIBRARY}
)

target_compile_definitions(user_management_system PRIVATE
    BOOST_ASIO_STANDALONE
    USE_POSTGRES=1
    PG_MAJOR_VERSION=18
)

set_target_properties(user_management_system PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_custom_target(run
    COMMAND ./bin/user_management_system
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS user_management_system
)