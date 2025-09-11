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

add_subdirectory(du_manager)
add_subdirectory(test_mode)

add_library(srsran_du_high STATIC
        du_high_impl.cpp
        du_high_factory.cpp
        du_high_clock_controller_factory.cpp
        du_high_executor_mapper.cpp)
target_link_libraries(srsran_du_high PUBLIC srsran_du_high_adapters)

add_library(srsran_o_du_high STATIC
        o_du_high_factory.cpp
        o_du_high_impl.cpp
        o_du_high_metrics_notifier_proxy.cpp)
target_link_libraries(srsran_o_du_high PUBLIC srsran_du_config_validators srsran_du_high srsran_mac_fapi_adaptor srsran_fapi srsran_e2)
