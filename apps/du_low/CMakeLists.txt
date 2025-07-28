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

set(SOURCES
        du_low_appconfig_cli11_schema.cpp
        du_low_appconfig_validators.cpp
        du_low_appconfig_translators.cpp
        du_low_appconfig_yaml_writer.cpp)


add_library(srsdu_low_base STATIC ${SOURCES})
target_link_libraries(srsdu_low_base PRIVATE
        srsran_app_helpers
        srsran_app_services
        srsran_support
        srsran_versioning)

target_include_directories(srsdu_low_base PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/external)

if (DPDK_FOUND)
    add_definitions(-DDPDK_FOUND)
    target_link_libraries(srsdu_low_base PRIVATE hal_dpdk)
endif (DPDK_FOUND)


add_executable(srsdu_low du_low.cpp)
add_backward(srsdu_low)
target_include_directories(srsdu_low PRIVATE ${CMAKE_SOURCE_DIR})
install(TARGETS srsdu_low RUNTIME)
target_link_libraries(srsdu_low PRIVATE srsdu_low_base srsran_flexible_o_du_low_split_6)
notify_binary_target(srsdu_low)
