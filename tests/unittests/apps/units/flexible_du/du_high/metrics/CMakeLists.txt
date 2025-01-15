#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(du_high_scheduler_cell_metrics_consumer_stdout_test du_high_scheduler_cell_metrics_consumer_stdout_test.cpp)
target_link_libraries(du_high_scheduler_cell_metrics_consumer_stdout_test srsran_support srsran_du_high_unit_metrics_helpers srsran_app_services)
add_test(du_high_scheduler_cell_metrics_consumer_stdout_test du_high_scheduler_cell_metrics_consumer_stdout_test)
target_include_directories(du_high_scheduler_cell_metrics_consumer_stdout_test PRIVATE ${CMAKE_SOURCE_DIR})
