#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(srscuup
        cu_up.cpp
        cu_up_appconfig_cli11_schema.cpp
        #cu_up_appconfig_validator.cpp
        #cu_up_appconfig_yaml_writer.cpp
)

install(TARGETS srscuup
        RUNTIME)

target_link_libraries(srscuup PRIVATE
        srsran_app_helpers
        srsran_app_services
        srsran_e1_gateway
        srsran_e2
        srsran_f1u_cu_up_split_connector
        srsran_o_cu_up_app_unit
        srsran_pcap
        srsran_support
        srsran_versioning
)

add_backward(srscuup)

target_include_directories(srscuup PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)
notify_binary_target(srscuup)
