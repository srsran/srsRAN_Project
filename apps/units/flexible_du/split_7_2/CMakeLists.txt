#
# Copyright 2021-2024 Software Radio Systems Limited
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

if (DPDK_FOUND)
    add_definitions(-DDPDK_FOUND)
endif (DPDK_FOUND)

set(SOURCES
            ru_ofh_config_cli11_schema.cpp
            ru_ofh_factories.cpp
            ru_ofh_config_translator.cpp
            ru_ofh_config_validator.cpp
            ru_ofh_config_yaml_writer.cpp)

add_library(srsran_split_7_2_app_unit_helpers STATIC ${SOURCES})
target_link_libraries(srsran_split_7_2_app_unit_helpers srsran_ru_ofh srsran_flexible_du_helpers)
target_include_directories(srsran_split_7_2_app_unit_helpers PRIVATE ${CMAKE_SOURCE_DIR})
