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


add_library(srsran_e1ap_cu_cp
        e1ap_cu_cp_factory.cpp
        e1ap_cu_cp_impl.cpp
        procedures/bearer_context_setup_procedure.cpp
        procedures/bearer_context_release_procedure.cpp
        procedures/bearer_context_modification_procedure.cpp
        procedures/cu_cp_e1_reset_procedure.cpp
        procedures/e1_release_procedure.cpp
        procedures/e1ap_stop_procedure.cpp
        ue_context/e1ap_cu_cp_ue_context.cpp)
target_include_directories(srsran_e1ap_cu_cp PRIVATE ..)
target_link_libraries(srsran_e1ap_cu_cp e1ap_asn1 srsran_e1ap_common srsran_cu_cp)
