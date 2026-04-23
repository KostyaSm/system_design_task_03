cmake_minimum_required(VERSION 3.16)
project(fitness_tracker_api LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPQ REQUIRED libpq)

add_executable(fitness_api
    src/main.cpp
    src/handlers/user_handlers.cpp
    src/handlers/exercise_handlers.cpp
    src/handlers/workout_handlers.cpp
    src/storage/postgres_storage.cpp
)

target_include_directories(fitness_api PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(fitness_api SYSTEM PUBLIC ${LIBPQ_INCLUDE_DIRS})
target_link_libraries(fitness_api PRIVATE ${LIBPQ_LINK_LIBRARIES} pthread)