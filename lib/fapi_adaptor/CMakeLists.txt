#
# Copyright 2021-2023 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
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
