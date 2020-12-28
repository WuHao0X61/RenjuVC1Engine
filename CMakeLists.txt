cmake_minimum_required (VERSION 3.10)

project (RenjuVC1)

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

aux_source_directory (src SRC_LIST)

add_executable (pbrain-RenjuVC1 ${SRC_LIST})