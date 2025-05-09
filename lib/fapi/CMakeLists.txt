#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(decorator_helpers)
add_subdirectory(message_buffering)
add_subdirectory(loggers)
add_subdirectory(validators)

add_library(srsran_fapi_decorators STATIC decorator_factory.cpp)
target_link_libraries(srsran_fapi_decorators PRIVATE srsran_fapi_message_buffering srsran_fapi_loggers)
target_include_directories(srsran_fapi_decorators PRIVATE ${CMAKE_SOURCE_DIR}/lib/fapi)

set(CELL_SOURCES
        fapi_config_message_interface_collection_impl.cpp
        config_message_gateway_impl.cpp
        config_message_validator.cpp)

add_library(srsran_fapi_config_gateway STATIC ${CELL_SOURCES})
target_link_libraries(srsran_fapi_config_gateway PRIVATE srslog)

add_library(srsran_fapi INTERFACE)
target_link_libraries(srsran_fapi INTERFACE srsran_fapi_decorators srsran_fapi_validators srsran_fapi_config_gateway)
