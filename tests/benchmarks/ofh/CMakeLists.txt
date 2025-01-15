#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "ofh")

add_executable(ofh_compression_benchmark ofh_compression_benchmark.cpp)
target_link_libraries(ofh_compression_benchmark srslog srsran_ofh_compression)
