#
# Copyright 2021-2025 Software Radio Systems Limited
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

add_subdirectory(consumers)

set(SOURCES
        du_high_rlc_metrics_producer.cpp
        du_high_rlc_metrics_consumers.cpp
        du_metrics_consumers.cpp
        du_metrics_producer.cpp)

add_library(srsran_du_high_unit_metrics_helpers STATIC ${SOURCES})
target_include_directories(srsran_du_high_unit_metrics_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_du_high_unit_metrics_helpers PRIVATE srsran_du_high_unit_metrics_consumers srsran_metrics_json_generators)
