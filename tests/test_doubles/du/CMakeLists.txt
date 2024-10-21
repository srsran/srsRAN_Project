#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(du_test_doubles test_du_high_worker_manager.cpp)
target_link_libraries(du_test_doubles srsran_support srsran_du_high srslog)