/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "rlc_stress_test.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
pthread_barrier_t barrier;
stress_stack::stress_stack(const stress_test_args& args_, uint32_t id, rb_id_t rb_id) :
  stack_id(id),
  args(args_),
  ue_name("UE-Worker-" + std::to_string(id)),
  pcell_name("PCell-Worker-" + std::to_string(id)),
  ue_worker{ue_name, task_worker_queue_size},
  pcell_worker{pcell_name, task_worker_queue_size},
  logger("STACK", {id, rb_id, "DL/UL"})
{
  ue_executor    = make_task_executor(ue_worker);
  pcell_executor = make_task_executor(pcell_worker);

  // MAC
  mac = std::make_unique<mac_dummy>(args_, id, rb_id);

  // F1AP
  f1ap = std::make_unique<f1ap_dummy>(id);

  // RRC dummy for notifications
  rrc = std::make_unique<rrc_dummy>(id);

  // Trafic generators
  traffic_sink   = std::make_unique<stress_traffic_sink>(id, rb_id);
  traffic_source = std::make_unique<stress_traffic_source>(args_, id, rb_id);

  sec_cfg = get_security_config_from_args(args_);

  // PDCP
  pdcp_config                  pdcp_cnfg = get_pdcp_config_from_args(id, args_);
  pdcp_entity_creation_message pdcp_msg  = {};
  pdcp_msg.ue_index                      = id;
  pdcp_msg.rb_id                         = rb_id;
  pdcp_msg.config                        = pdcp_cnfg;
  pdcp_msg.tx_lower                      = f1ap.get();
  pdcp_msg.tx_upper_cn                   = rrc.get();
  pdcp_msg.rx_upper_dn                   = traffic_sink.get();
  pdcp_msg.rx_upper_cn                   = rrc.get();
  pdcp_msg.timers                        = timer_factory{timers, *ue_executor};
  pdcp                                   = create_pdcp_entity(pdcp_msg);
  traffic_source->set_pdcp_tx_upper(&pdcp->get_tx_upper_data_interface());
  f1ap->set_pdcp_rx_lower(&pdcp->get_rx_lower_interface());

  pdcp_tx_upper_control_interface& rrc_tx_if = pdcp->get_tx_upper_control_interface();
  rrc_tx_if.enable_security(sec_cfg);
  pdcp_rx_upper_control_interface& rrc_rx_if = pdcp->get_rx_upper_control_interface();
  rrc_rx_if.enable_security(sec_cfg);

  // RLC
  rlc_config                  rlc_cnfg = get_rlc_config_from_args(args_);
  rlc_entity_creation_message rlc_msg  = {};
  rlc_msg.ue_index                     = static_cast<du_ue_index_t>(stack_id);
  rlc_msg.rb_id                        = rb_id;
  rlc_msg.rx_upper_dn                  = f1ap.get();
  rlc_msg.tx_upper_cn                  = f1ap.get();
  rlc_msg.tx_upper_dn                  = f1ap.get();
  rlc_msg.tx_lower_dn                  = mac.get();
  rlc_msg.config                       = rlc_cnfg;
  rlc_msg.timers                       = &timers;
  rlc_msg.pcell_executor               = pcell_executor.get();
  rlc_msg.ue_executor                  = ue_executor.get();
  rlc                                  = create_rlc_entity(rlc_msg);
  f1ap->set_rlc_tx_upper_data(rlc->get_tx_upper_layer_data_interface());

  mac->set_rlc_tx_lower(rlc->get_tx_lower_layer_interface());
  mac->set_rlc_rx_lower(rlc->get_rx_lower_layer_interface());
}

void stress_stack::start()
{
  srsran_assert(peer_stack != nullptr, "Peer stack was not set when starting the stack.");
  srsran_assert(peer_stack != this, "Peer cannot be itself.");

  for (uint32_t i = 0; i < 20; i++) {
    traffic_source->send_pdu();
  }

  // Schedule the TTI when the thread are started.
  pcell_executor->defer([this]() { run_lower_tti(0); });
}

void stress_stack::wait_for_finish()
{
  logger.log_debug("Waiting for PCell worker to finish. Stack id={}\n", stack_id);
  {
    std::unique_lock<std::mutex> lk_pcell(mutex_pcell);
    cv_pcell.wait(lk_pcell, [this] { return stopping_pcell.load(); });
  }
  logger.log_debug(
      "PCell worker no longer processing. Stack id={}, stopping={}\n", stack_id, stopping_pcell ? "true" : "false");

  logger.log_debug("Waiting to stop upper worker. Stack id={}\n", stack_id);
  {
    std::unique_lock<std::mutex> lk_ue(mutex_ue);
    cv_ue.wait(lk_ue, [this] { return stopping_ue.load(); });
  }
  logger.log_debug("Upper thread no longer processing. Stack id={}", stack_id);

  pcell_worker.stop();
  ue_worker.stop();
}

void stress_stack::run_upper_tti(uint32_t tti)
{
  logger.log_debug("Running upper TTI={}, PDU RX queue size={}", tti, mac->pdu_rx_list.size());
  if (tti < (args.nof_ttis - 1)) {
    for (uint32_t i = 0; i < 20; i++) {
      traffic_source->send_pdu();
    }
    mac->run_rx_tti();
    timers.tick(); // timers are run from the upper executor
  } else {
    std::unique_lock<std::mutex> lk(mutex_ue);
    stopping_ue = true;
    lk.unlock();
    cv_ue.notify_all();
  }
  pcell_executor->defer([this, tti]() { run_lower_tti(tti + 1); });
  logger.log_debug("Finished running upper TTI={}, PDU RX queue size={}", tti, mac->pdu_rx_list.size());
}

void stress_stack::run_lower_tti(uint32_t tti)
{
  logger.log_debug("Running lower TTI={}", tti);
  if (tti < args.nof_ttis) {
    std::vector<byte_buffer_chain> pdu_list = mac->run_tx_tti(tti);
    logger.log_debug("Generated PDU list size={}", pdu_list.size());
    ue_executor->defer([this, tti]() { run_upper_tti(tti); });
    peer_stack->push_pdus(std::move(pdu_list));
    tti++;
    pthread_barrier_wait(&barrier); // wait for other stack to finish
  } else {
    logger.log_debug("Stopping lower TTI={}", tti);
    std::unique_lock<std::mutex> lk(mutex_pcell);
    stopping_pcell = true;
    lk.unlock();
    cv_pcell.notify_all();
  }
  logger.log_debug("Finished running lower TTI={}", tti);
}

void stress_stack::push_pdus(std::vector<byte_buffer_chain> list_pdus)
{
  auto push_fnc = [this, list_pdus = std::move(list_pdus)]() mutable { mac->push_rx_pdus(std::move(list_pdus)); };
  if (!stopping_pcell.load() && !stopping_ue.load()) {
    ue_executor->defer(std::move(push_fnc));
  }
}

void stress_test(const stress_test_args& args)
{
  auto& log_stack = srslog::fetch_basic_logger("STACK", false);

  // Create the stack emulators
  stress_stack stack_emulator_0(args, 0, drb_id_t::drb1);
  stress_stack stack_emulator_1(args, 1, drb_id_t::drb1);
  stack_emulator_0.set_peer_stack(&stack_emulator_1);
  stack_emulator_1.set_peer_stack(&stack_emulator_0);

  //  Launch transmission
  stack_emulator_0.start();
  stack_emulator_1.start();

  //  Wait for test to finish
  stack_emulator_0.wait_for_finish();
  stack_emulator_1.wait_for_finish();

  // Print and analyse metrics
  pdcp_metrics_container stack0_pdcp_metrics = stack_emulator_0.get_pdcp_metrics();
  pdcp_metrics_container stack1_pdcp_metrics = stack_emulator_1.get_pdcp_metrics();
  rlc_metrics            stack0_rlc_metrics  = stack_emulator_0.get_rlc_metrics();
  rlc_metrics            stack1_rlc_metrics  = stack_emulator_1.get_rlc_metrics();

  // Print PDCP metrics
  log_stack.info("STACK 0 emulator PDCP TX metrics: {}", stack0_pdcp_metrics.tx);
  log_stack.info("STACK 0 emulator PDCP RX metrics: {}", stack0_pdcp_metrics.rx);
  log_stack.info("STACK 1 emulator PDCP TX metrics: {}", stack1_pdcp_metrics.tx);
  log_stack.info("STACK 1 emulator PDCP RX metrics: {}", stack1_pdcp_metrics.rx);

  // Print RLC metrics
  log_stack.info("STACK 0 emulator RLC TX metrics: {}", stack0_rlc_metrics.tx);
  log_stack.info("STACK 0 emulator RLC RX metrics: {}", stack0_rlc_metrics.rx);
  log_stack.info("STACK 1 emulator RLC TX metrics: {}", stack1_rlc_metrics.tx);
  log_stack.info("STACK 1 emulator RLC RX metrics: {}", stack1_rlc_metrics.rx);
}

int main(int argc, char** argv)
{
  // srsgnb_debug_handle_crash(argc, argv);
  pthread_barrier_init(&barrier, nullptr, 2);
  stress_test_args args = {};
  if (not parse_args(args, argc, argv)) {
    fprintf(stderr, "Could not parse command line options\n");
    return -1;
  }

  init_log_from_args(args);

  stress_test(args);
  pthread_barrier_destroy(&barrier);
  return 0;
}
