cmake_minimum_required (VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -Wall -g2 -ggdb")
aux_source_directory(./src DIR_SRC)
aux_source_directory(./src/graphics DIR_GRAPHICS)
LIST (APPEND FULL_SRC
  ${DIR_SRC}
  ${DIR_GRAPHICS}
)
link_libraries(png pthread xcb xcb-damage xcb-xtest xcb-composite boost_system boost_chrono boost_thread)
add_executable(pulsar ${FULL_SRC})
