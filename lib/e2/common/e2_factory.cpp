/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/e2/e2_factory.h"
#include "e2_entity.h"
#include "e2_impl.h"
#include "e2ap_asn1_packer.h"
#include "srsran/e2/e2_cu_factory.h"
#include "srsran/e2/e2_du_factory.h"

using namespace srsran;

std::unique_ptr<e2_interface> srsran::create_e2(e2ap_configuration&      e2ap_cfg_,
                                                timer_factory            timers_,
                                                e2_connection_client&    e2_client_,
                                                e2_subscription_manager& e2_subscription_mngr_,
                                                e2sm_manager&            e2sm_mngr_,
                                                task_executor&           task_exec_)
{
  auto e2 = std::make_unique<e2_impl>(e2ap_cfg_, timers_, e2_client_, e2_subscription_mngr_, e2sm_mngr_, task_exec_);
  return e2;
}

std::unique_ptr<e2_agent> srsran::create_e2_du_agent(e2ap_configuration&            e2ap_cfg_,
                                                     e2_connection_client&          e2_client_,
                                                     e2_du_metrics_interface*       e2_metrics_var,
                                                     srs_du::f1ap_ue_id_translator* f1ap_ue_id_translator_,
                                                     srs_du::du_configurator*       du_configurator_,
                                                     timer_factory                  timers_,
                                                     task_executor&                 e2_exec_)
{
  std::variant<e2_du_metrics_interface*, e2_cu_metrics_interface*> e2_du_metrics_var = e2_metrics_var;
  auto                                                             e2_ext            = std::make_unique<e2_entity>(
      e2ap_cfg_, e2_client_, e2_du_metrics_var, f1ap_ue_id_translator_, du_configurator_, timers_, e2_exec_);
  return e2_ext;
}

std::unique_ptr<e2_agent> srsran::create_e2_cu_agent(e2ap_configuration&      e2ap_cfg_,
                                                     e2_connection_client&    e2_client_,
                                                     e2_cu_metrics_interface* e2_metrics_var,
                                                     timer_factory            timers_,
                                                     task_executor&           e2_exec_)
{
  std::variant<e2_du_metrics_interface*, e2_cu_metrics_interface*> e2_cu_metrics_var = e2_metrics_var;
  auto                                                             e2_ext =
      std::make_unique<e2_entity>(e2ap_cfg_, e2_client_, e2_cu_metrics_var, nullptr, nullptr, timers_, e2_exec_);
  return e2_ext;
}

std::unique_ptr<e2ap_packer>
srsran::create_e2ap_asn1_packer(sctp_network_gateway_data_handler& gw_, e2_message_handler& e2_, dlt_pcap& pcap_)
{
  auto packer = std::make_unique<e2ap_asn1_packer>(gw_, e2_, pcap_);
  return packer;
}
