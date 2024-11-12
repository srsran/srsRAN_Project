#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#


add_subdirectory(helpers)

# Build the flexible du split 7.2 library when it is selected in the definition.
if (DU_SPLIT_7_2)
    set(SOURCES
            split_7_2_du_application_unit_impl.cpp
            split_7_2_du_factory.cpp
            split_7_2_du_unit_cli11_schema.cpp
            split_7_2_du_unit_config_validator.cpp
            split_7_2_du_unit_config_yaml_writer.cpp)

    add_library(srsran_flexible_du STATIC ${SOURCES})
    target_include_directories(srsran_flexible_du PRIVATE ${CMAKE_SOURCE_DIR})

    target_link_libraries(srsran_flexible_du
            srsran_pcap
            srsran_app_services
            srsran_split_7_2_app_unit_helpers
            srsran_flexible_du_helpers)

endif ()
