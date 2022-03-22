
add_executable(modulation_mapper_test modulation_mapper_test.cpp)
target_link_libraries(modulation_mapper_test modulation_mapper srsvec srslog)
add_test(modulation_mapper_test modulation_mapper_test)

add_custom_command(
        TARGET modulation_mapper_test
        POST_BUILD
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/unittests/phy/upper/channel_modulation/
        COMMAND ${CMAKE_COMMAND} -E tar xvf ${CMAKE_CURRENT_SOURCE_DIR}/modulation_mapper_test/data/modulation_mapper_test_data.tar.gz
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/modulation_mapper_test/data/modulation_mapper_test_data.tar.gz
        VERBATIM)