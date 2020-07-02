
cmake_minimum_required (VERSION 3.8)


add_library(Logger STATIC Logger.cpp)
set_property(TARGET Logger PROPERTY CXX_STANDARD 17)