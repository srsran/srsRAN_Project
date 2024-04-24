#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        console_helper.cpp
        e2_metric_connector_manager.cpp
        metrics_hub.cpp
        metrics_plotter_json.cpp
        metrics_plotter_stdout.cpp
        rlc_metrics_plotter_json.cpp
        worker_manager.cpp)

add_library(srsran_app_services STATIC ${SOURCES})
target_include_directories(srsran_app_services PRIVATE ${CMAKE_SOURCE_DIR})
