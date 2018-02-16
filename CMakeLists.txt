cmake_minimum_required(VERSION 3.0.2)

project(PBOPT
        VERSION 1.0.0
        DESCRIPTION "Policy-based optimization toolbox."
        LANGUAGES CXX)

install(DIRECTORY pbopt/
        DESTINATION include/pbopt)
