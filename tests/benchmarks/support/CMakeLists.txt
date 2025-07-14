#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(../../..)

add_executable(worker_pool_benchmark worker_pool_benchmark.cpp)
target_link_libraries(worker_pool_benchmark srsran_support srslog)
add_test(worker_pool_benchmark worker_pool_benchmark)
