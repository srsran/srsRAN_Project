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

set(SOURCES
        ru_ofh_config_validator.cpp
        ru_ofh_downlink_plane_handler_proxy.cpp
        ru_ofh_factory.cpp
        ru_ofh_impl.cpp
        ru_ofh_controller_impl.cpp
        ru_ofh_rx_symbol_handler_impl.cpp
        ru_ofh_uplink_plane_handler_proxy.cpp)

add_library(srsran_ru_ofh STATIC ${SOURCES})
target_link_libraries(srsran_ru_ofh srsran_ofh srsran_ofh_ethernet srsran_ofh_ecpri)
