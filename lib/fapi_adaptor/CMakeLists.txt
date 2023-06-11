#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(mac)
add_subdirectory(phy)

set(SOURCES
        precoding_matrix_mapper.cpp
        precoding_matrix_repository.cpp
        precoding_matrix_table_generator.cpp)

add_library(srsran_fapi_precoding_matrix_tools STATIC ${SOURCES})
target_link_libraries(srsran_fapi_precoding_matrix_tools srsran_ran srslog)
