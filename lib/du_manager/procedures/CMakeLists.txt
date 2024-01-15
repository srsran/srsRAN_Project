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

set(SOURCES
        initial_du_setup_procedure.cpp
        du_stop_procedure.cpp
        ue_creation_procedure.cpp
        ue_configuration_procedure.cpp
        ue_deletion_procedure.cpp
        du_ue_ric_configuration_procedure.cpp)
add_library(srsran_du_manager_procedures STATIC ${SOURCES})
target_link_libraries(srsran_du_manager_procedures sched_config srsran_ran mac_configuration_helpers du_ue)

