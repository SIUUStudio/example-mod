cmake_minimum_required(VERSION 3.25)
project(PredictiveInputBuffer)
find_package(Geode REQUIRED)
add_geode_mod(
    yourname.predictive_input_buffer
    VERSION 1.0.0
    CONTAINS_GEODE_FILES
)
target_sources(${PROJECT_NAME} PRIVATE src/main.cpp)
