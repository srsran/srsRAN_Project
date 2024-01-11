#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(mac)
add_subdirectory(phy)

set(PRECODING_SOURCES
        precoding_matrix_mapper.cpp
        precoding_matrix_repository.cpp
        precoding_matrix_table_generator.cpp
        )

set(UCI_SOURCES
        uci_part2_correspondence_generator.cpp
        uci_part2_correspondence_mapper.cpp
        uci_part2_correspondence_repository.cpp
        )

add_library(srsran_fapi_precoding_matrix_tools STATIC ${PRECODING_SOURCES})
target_link_libraries(srsran_fapi_precoding_matrix_tools srsran_ran srslog)

add_library(srsran_fapi_uci_part2_tools STATIC ${UCI_SOURCES})
target_link_libraries(srsran_fapi_uci_part2_tools srsran_ran srslog)
