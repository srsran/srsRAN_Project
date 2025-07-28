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
        worker_manager.cpp
        worker_manager_cli11_schema.cpp
        worker_manager_appconfig_validator.cpp)

add_library(srsran_cpu_affinities_helper STATIC cli11_cpu_affinities_parser_helper.cpp)

add_library(srsran_worker_manager_app_service STATIC ${SOURCES})
target_include_directories(srsran_worker_manager_app_service PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_worker_manager_app_service
        srsran_cpu_affinities_helper
        srsran_cu_up      # TODO: Temp
        srsran_du_high    # TODO: Temp
        srsran_cu_cp      # TODO: Temp
        srsran_o_du_low   # TODO: Temp
        srsran_ru_dummy   # TODO: Temp
        srsran_ru_generic # TODO: Temp
        srsran_ru_ofh)    # TODO: Temp
