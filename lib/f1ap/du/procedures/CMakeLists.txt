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
        gnb_cu_configuration_update_procedure.cpp
        f1ap_du_setup_procedure.cpp
        f1ap_du_removal_procedure.cpp
        f1ap_du_ue_context_setup_procedure.cpp
        f1ap_du_ue_context_modification_procedure.cpp
        f1ap_du_ue_context_release_procedure.cpp
        f1ap_du_reset_procedure.cpp
        f1ap_du_initiated_reset_procedure.cpp
        f1ap_du_gnbdu_config_update_procedure.cpp
        f1ap_du_positioning_procedures.cpp
)

add_library(srsran_f1ap_du_procedures STATIC ${SOURCES})
target_link_libraries(srsran_f1ap_du_procedures srsran_f1ap_common srsran_f1ap_du_helpers f1ap_asn1)
