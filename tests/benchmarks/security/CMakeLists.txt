#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(s3g_benchmark s3g_benchmark.cpp)
target_link_libraries(s3g_benchmark srslog srsran_security)
add_test(s3g_benchmark s3g_benchmark)
