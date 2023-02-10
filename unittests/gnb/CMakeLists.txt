#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(metrics_plotter_stdout_test metrics_plotter_stdout_test.cpp ${CMAKE_SOURCE_DIR}/apps/gnb/helpers/metrics_plotter_stdout.cpp)
target_link_libraries(metrics_plotter_stdout_test srsgnb_support)
add_test(metrics_plotter_stdout_test metrics_plotter_stdout_test)
include_directories(${CMAKE_SOURCE_DIR}/apps/gnb/helpers)