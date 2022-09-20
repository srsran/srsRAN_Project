/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rlc_stress_test.h"
#include "srsgnb/rlc/rlc_factory.h"
#include <random>

using namespace srsgnb;

void pdcp_traffic_sink::on_new_sdu(byte_buffer_slice_chain pdu)
{
  logger.info(pdu.begin(), pdu.end(), "Received PDU ({} B)", pdu.length());
}

void pdcp_traffic_source::send_pdu()
{
  rlc_sdu sdu;
  sdu.pdcp_count = pdcp_sn++;

  // random or fixed SDU size
  if (args.sdu_size < 1) {
    sdu_size = int_dist(rgen);
  } else {
    sdu_size = args.sdu_size;
  }

  for (uint32_t i = 0; i < sdu_size; i++) {
    sdu.buf.append(payload);
    payload++;
  }

  rlc_tx_upper->handle_sdu(sdu);
}

void mac_dummy::run_tx_tti()
{
  // Generate A number of MAC PDUs
  for (uint32_t i = 0; i < args.nof_pdu_tti; i++) {
    // Get MAC PDU size
    float factor = 1.0f;
    if (args.random_opp) {
      factor = 0.5f + real_dist(rgen);
    }
    int opp_size = static_cast<int>(args.avg_opp_size * factor);

    // Request data to transmit
    if (bsr.load(std::memory_order_relaxed) > 0) {
      byte_buffer_slice_chain pdu = rlc_tx_lower->pull_pdu(opp_size);
      // Push PDU in the list
      if (pdu.length() > 0) {
        pdu_list.push_back(std::move(pdu));
      }
    }
  }
}

void mac_dummy::run_rx_tti()
{
  auto pdu_it      = pdu_list.begin(); // PDU iterator
  bool skip_action = false;            // Avoid discarding a duplicated or duplicating a discarded

  while (pdu_it != pdu_list.end()) {
    // Drop
    float rnd = real_dist(rgen);
    if (std::isnan(rnd) || ((rnd > args.pdu_drop_rate) || skip_action)) {
      // Cut/Trim
      byte_buffer_slice_chain::iterator pdu_end = pdu_it->end();
      if ((real_dist(rgen) < args.pdu_cut_rate)) {
        size_t pdu_len     = pdu_it->length();
        size_t trimmed_len = static_cast<size_t>(pdu_len * real_dist(rgen));
        if (trimmed_len < pdu_len) {
          pdu_end = pdu_it->begin() + trimmed_len;
        }
        logger.info("Cutting MAC PDU len ({} B -> {} B)", pdu_it->length(), pdu_len);
      }

      // Write PDU copy in RX
      rlc_rx_lower->handle_pdu(byte_buffer(pdu_it->begin(), pdu_end));

      // Write PCAP
      // TODO: write PCAP
    } else {
      logger.info(pdu_it->begin(), pdu_it->end(), "Dropping RLC PDU ({} B)", pdu_it->length());
      skip_action = true; // Avoid drop duplicating this PDU
    }

    // Duplicate
    if (real_dist(rgen) > args.pdu_duplicate_rate || skip_action) {
      pdu_it++;
      skip_action = false; // Allow action on the next PDU
    } else {
      logger.info(pdu_it->begin(), pdu_it->end(), "Duplicating RLC PDU ({} B)", pdu_it->length());
      skip_action = true; // Avoid drop of this PDU
    }
  }

  // clear pdu_list
  pdu_list.clear();
}

void stress_test(stress_test_args args)
{
  srslog::sink* log_sink =
      (args.log_filename == "stdout") ? srslog::create_stdout_sink() : srslog::create_file_sink(args.log_filename);
  if (log_sink == nullptr) {
    return;
  }
  srslog::log_channel* chan = srslog::create_log_channel("main_channel", *log_sink);
  if (chan == nullptr) {
    return;
  }
  srslog::set_default_sink(*log_sink);

  timer_manager timers;

  rlc_config cnfg = {};
  if (args.mode == "TM") {
    cnfg.mode = srsgnb::rlc_mode::tm;
  } else if (args.mode == "UM6") {
    cnfg.mode                  = srsgnb::rlc_mode::um_bidir;
    cnfg.um.rx.sn_field_length = rlc_um_sn_size::size6bits;
    cnfg.um.tx.sn_field_length = rlc_um_sn_size::size6bits;
  } else if (args.mode == "UM12") {
    cnfg.mode                  = srsgnb::rlc_mode::um_bidir;
    cnfg.um.rx.sn_field_length = rlc_um_sn_size::size12bits;
    cnfg.um.tx.sn_field_length = rlc_um_sn_size::size12bits;
  } else if (args.mode == "AM12") {
    cnfg.mode                  = srsgnb::rlc_mode::am;
    cnfg.am.rx.sn_field_length = rlc_am_sn_size::size12bits;
    cnfg.am.tx.sn_field_length = rlc_am_sn_size::size12bits;
  } else if (args.mode == "AM18") {
    cnfg.mode                  = srsgnb::rlc_mode::am;
    cnfg.am.rx.sn_field_length = rlc_am_sn_size::size18bits;
    cnfg.am.tx.sn_field_length = rlc_am_sn_size::size18bits;
  } else {
    fprintf(stderr, "Unsupported RLC mode %s, exiting.\n", args.mode.c_str());
    exit(-1);
  }

  // MAC12: RLC1->MAC12->RLC2
  std::unique_ptr<mac_dummy> mac12 = std::make_unique<mac_dummy>(args);

  // MAC21: RLC2->MAC21->RLC1
  std::unique_ptr<mac_dummy> mac21 = std::make_unique<mac_dummy>(args);

  // RLC1
  std::unique_ptr<pdcp_traffic_sink>   traffic_sink1   = std::make_unique<pdcp_traffic_sink>();
  std::unique_ptr<pdcp_traffic_source> traffic_source1 = std::make_unique<pdcp_traffic_source>(args);
  rlc_entity_creation_message          msg1;
  msg1.rx_upper_dn                 = traffic_sink1.get();
  msg1.tx_upper_cn                 = traffic_source1.get();
  msg1.tx_upper_dn                 = traffic_source1.get();
  msg1.tx_lower_dn                 = mac12.get();
  msg1.config                      = cnfg;
  msg1.timers                      = &timers;
  std::unique_ptr<rlc_entity> rlc1 = create_rlc_entity(msg1);
  traffic_source1->set_rlc_tx_upper(rlc1->get_tx_upper_layer_data_interface());
  mac12->set_rlc_tx_lower(rlc1->get_tx_lower_layer_interface());
  mac21->set_rlc_rx_lower(rlc1->get_rx_lower_layer_interface());

  // RLC2
  std::unique_ptr<pdcp_traffic_sink>   traffic_sink2   = std::make_unique<pdcp_traffic_sink>();
  std::unique_ptr<pdcp_traffic_source> traffic_source2 = std::make_unique<pdcp_traffic_source>(args);
  rlc_entity_creation_message          msg2;
  msg2.rx_upper_dn                 = traffic_sink2.get();
  msg2.tx_upper_cn                 = traffic_source2.get();
  msg2.tx_upper_dn                 = traffic_source2.get();
  msg2.tx_lower_dn                 = mac21.get();
  msg2.config                      = cnfg;
  msg2.timers                      = &timers;
  std::unique_ptr<rlc_entity> rlc2 = create_rlc_entity(msg2);
  traffic_source2->set_rlc_tx_upper(rlc2->get_tx_upper_layer_data_interface());
  mac21->set_rlc_tx_lower(rlc2->get_tx_lower_layer_interface());
  mac12->set_rlc_rx_lower(rlc2->get_rx_lower_layer_interface());

  // Launch transmission
  // Fixme: create worker thread and executer
  for (uint32_t n = 0; n < 100; n++) {
    traffic_source1->send_pdu();
    traffic_source2->send_pdu();
  }

  while (rlc1->get_tx_lower_layer_interface()->get_buffer_state() > 0) {
    // RLC1->MAC12->RLC2
    mac12->run_tx_tti();
    mac12->run_rx_tti();

    // RLC2->MAC21->RLC1
    mac21->run_tx_tti();
    mac21->run_rx_tti();

    timers.tick_all();
  }

  // Print and analyse metrics
  // TODO
}

int main(int argc, char** argv)
{
  // srsgnb_debug_handle_crash(argc, argv);

  stress_test_args args = {};
  if (not parse_args(args, argc, argv)) {
    fprintf(stderr, "Could not parse command line options\n");
    return -1;
  }

  srslog::init();

  stress_test(args);

  return 0;
}
