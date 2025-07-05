cmake_minimum_required(VERSION 3.10)
project(TeamSpeakOverlayHook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(PROJECT_SOURCES main.cpp)

add_executable(TeamSpeakOverlayHook WIN32 main.cpp)
