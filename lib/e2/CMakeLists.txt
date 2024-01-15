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
    procedures/e2_setup_procedure.cpp
    procedures/e2_subscription_setup_procedure.cpp
    procedures/e2_subscription_delete_procedure.cpp
    procedures/e2_indication_procedure.cpp
    procedures/e2_ric_control_procedure.cpp
    common/e2ap_asn1_packer.cpp
    common/e2_factory.cpp
    common/e2_impl.cpp
    common/e2_entity.cpp
    common/e2_subscription_manager_impl.cpp
    e2sm/e2sm_kpm/e2sm_kpm_impl.cpp
    e2sm/e2sm_kpm/e2sm_kpm_asn1_packer.cpp
    e2sm/e2sm_rc/e2sm_rc_asn1_packer.cpp
    e2sm/e2sm_rc/e2sm_rc_impl.cpp
    e2sm/e2sm_rc/e2sm_rc_control_service_impl.cpp
    e2sm/e2sm_rc/e2sm_rc_control_action_du_executor.cpp
    e2sm/e2sm_factory.cpp
    e2sm/e2sm_manager.cpp
    e2sm/e2sm_kpm/e2sm_kpm_utils.cpp
    e2sm/e2sm_kpm/e2sm_kpm_report_service_impl.cpp
    e2sm/e2sm_kpm/e2sm_kpm_du_meas_provider_impl.cpp
    common/e2_du_metrics_connector.cpp
 )

add_library(srsran_e2 STATIC ${SOURCES})
target_link_libraries(srsran_e2 e2ap_asn1 srsran_support)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
