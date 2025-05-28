#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        du_low_appconfig_cli11_schema.cpp
        du_low_appconfig_validators.cpp
        du_low_appconfig_translators.cpp
        du_low_appconfig_yaml_writer.cpp)


add_library(srsdu_low_base STATIC ${SOURCES})
target_link_libraries(srsdu_low_base PRIVATE
        srsran_app_helpers
        srsran_app_services
        srsran_support
        srsran_versioning)

target_include_directories(srsdu_low_base PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)

add_executable(srsdu_low du_low.cpp)
add_backward(srsdu_low)
target_include_directories(srsdu_low PRIVATE ${CMAKE_SOURCE_DIR})
install(TARGETS srsdu_low RUNTIME)
target_link_libraries(srsdu_low PRIVATE srsdu_low_base srsran_flexible_o_du_low_split_6)
notify_binary_target(srsdu_low)
