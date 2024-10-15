#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
    procedures/e2_setup_procedure.cpp
    procedures/e2_subscription_setup_procedure.cpp
    procedures/e2_subscription_delete_procedure.cpp
    procedures/e2_indication_procedure.cpp
    procedures/e2_ric_control_procedure.cpp
    common/e2_connection_handler.cpp
    common/e2ap_asn1_packer.cpp
    common/e2_factory.cpp
    common/e2_impl.cpp
    common/e2_entity.cpp
    common/e2_subscription_manager_impl.cpp
    common/e2_du_metrics_connector.cpp
    common/e2_cu_metrics_connector.cpp
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
    e2sm/e2sm_kpm/e2sm_kpm_cu_meas_provider_impl.cpp
    gateways/e2_network_client_factory.cpp
 )

add_library(srsran_e2 STATIC ${SOURCES})
target_link_libraries(srsran_e2 e2ap_asn1 srsran_support)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
