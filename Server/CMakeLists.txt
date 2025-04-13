project(NEOCOOP_Server VERSION 0.1.0 LANGUAGES CXX)
set(SRC src)

message(STATUS "Configuring ${PROJECT_NAME}:SERVER...")

find_package(fmt CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS asio)
find_package(Boost REQUIRED COMPONENTS static_string)

add_executable(${PROJECT_NAME} ${SRC}/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::asio)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::static_string)


