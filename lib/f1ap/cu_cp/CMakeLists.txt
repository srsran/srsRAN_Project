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


add_library(srsran_f1ap_cu
        f1ap_cu_impl.cpp
        f1ap_cu_factory.cpp
        procedures/f1_setup_procedure.cpp
        procedures/f1_removal_procedure.cpp
        procedures/f1ap_stop_procedure.cpp
        procedures/gnb_cu_configuration_update_procedure.cpp
        procedures/ue_context_setup_procedure.cpp
        procedures/ue_context_modification_procedure.cpp
        procedures/ue_context_release_procedure.cpp
        ue_context/f1ap_cu_ue_context.cpp
        ue_context/ue_ul_bearer_manager.cpp)

target_include_directories(srsran_f1ap_cu PRIVATE ..)
target_link_libraries(srsran_f1ap_cu srsran_support srsran_ran srsran_f1ap_common f1ap_asn1)
