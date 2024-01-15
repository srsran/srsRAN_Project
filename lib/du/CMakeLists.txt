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

set(SOURCES du_cell_config_validation.cpp du_update_config_helpers.cpp)
add_library(srsran_du_config_validators STATIC ${SOURCES})

add_library(srsran_du STATIC
        du_impl.cpp
        du_factory.cpp
        du_low_impl.cpp
        adapters/fapi_factory.cpp)
target_link_libraries(srsran_du PUBLIC srsran_du_config_validators srsran_du_high)
