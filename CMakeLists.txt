cmake_minimum_required(VERSION 3.16)
project(METS CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Wt REQUIRED COMPONENTS Wt HTTP)

add_executable(mets src/main.cpp)
target_link_libraries(mets Wt::Wt Wt::HTTP)