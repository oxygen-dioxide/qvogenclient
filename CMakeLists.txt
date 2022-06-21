# Open the project in src is suggested.

cmake_minimum_required(VERSION 3.17)

project(QVogenClient LANGUAGES CXX)

set(PROJECT_DEPLOY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/release)

add_subdirectory(src)