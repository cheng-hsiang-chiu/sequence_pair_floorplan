
## --------------------------------------------------------
## main program 1: sp floorplan
## --------------------------------------------------------
add_executable(sp main.cpp)
target_link_libraries(sp error_settings)
target_include_directories(sp PRIVATE ${SP_SRC_DIR})
