add_subdirectory(upper)
add_subdirectory(lower)

add_library(resource_grid_pool STATIC resource_grid_pool_impl.cpp resource_grid_impl.cpp)
target_link_libraries(resource_grid_pool srsvec)
