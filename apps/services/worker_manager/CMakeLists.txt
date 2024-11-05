#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        worker_manager.cpp
        worker_manager_cli11_schema.cpp)

add_library(srsran_cpu_affinities_helper STATIC cli11_cpu_affinities_parser_helper.cpp)

add_library(srsran_worker_manager_service STATIC ${SOURCES})
target_include_directories(srsran_worker_manager_service PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_worker_manager_service
        srsran_cpu_affinities_helper
        srsran_du_high # TODO: Temp
        srsran_cu_up) # TODO: Temp
