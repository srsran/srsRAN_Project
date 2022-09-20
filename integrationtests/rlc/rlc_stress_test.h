/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/rlc/rlc_rx.h"
#include "srsgnb/rlc/rlc_tx.h"
#include "srsgnb/srslog/srslog.h"
#include <getopt.h>
#include <random>

namespace srsgnb {

struct stress_test_args {
  std::string mode               = "UM12";
  int32_t     sdu_size           = -1;
  float       pdu_drop_rate      = 0.1;
  float       pdu_cut_rate       = 0.0;
  float       pdu_duplicate_rate = 0.0;
  uint32_t    avg_opp_size       = 1505;
  bool        random_opp         = true;
  uint32_t    seed               = 0;
  uint32_t    nof_pdu_tti        = 1;
  std::string log_filename       = "stdout";
  uint32_t    min_sdu_size       = 5;
  uint32_t    max_sdu_size       = 9000;
};

bool parse_args(stress_test_args& args, int argc, char* argv[])
{
  static const struct option long_options[] = {{"help", no_argument, nullptr, 'h'},
                                               {"mode", required_argument, nullptr, 'm'},
                                               {"log_filename", required_argument, nullptr, 'l'},
                                               {nullptr, 0, nullptr, 0}};

  static const char usage[] = "Usage: rlc_stress_test [options]\n"
                              "\n"
                              "  -h, --help                 Show help message and quit.\n"
                              "  -m, --mode <RLC mode>      Specify {TM, UM6, UM12, AM12, AM18}.\n"
                              "  --log_filename <filename>  Specify log filename. Use 'stdout' to print to console.\n"
                              "\n";
  // Parse arguments
  while (true) {
    int option_index = 0;
    int c            = getopt_long(argc, argv, "hm:l:", long_options, &option_index);
    if (c == -1) {
      break;
    }

    switch (c) {
      case 'h':
        fprintf(stdout, "%s", usage);
        break;
      case 'm':
        args.mode = std::string(optarg);
        fprintf(stdout, "RLC Mode %s\n", args.mode.c_str());
        break;
      case 'l':
        args.log_filename = std::string(optarg);
        fprintf(stdout, "Log filename %s\n", args.log_filename.c_str());
        break;
      default:
        fprintf(stderr, "error parsing arguments\n");
        return false;
    }
  }
  return true;
}

class pdcp_traffic_sink : public rlc_rx_upper_layer_data_notifier
{
  srslog::basic_logger& logger;

public:
  pdcp_traffic_sink() : logger(srslog::fetch_basic_logger("PDCP_RX", false)) {}

  // rlc_rx_upper_layer_data_notifier interface
  void on_new_sdu(byte_buffer_slice_chain pdu) final;
};

class pdcp_traffic_source : public rlc_tx_upper_layer_data_notifier, public rlc_tx_upper_layer_control_notifier
{
  stress_test_args& args;
  uint32_t          pdcp_sn  = 0;
  uint32_t          sdu_size = 0;
  uint8_t           payload  = 0x0; // increment for each SDU

  std::mt19937                    rgen;
  std::uniform_int_distribution<> int_dist;

  rlc_tx_upper_layer_data_interface* rlc_tx_upper = nullptr;

public:
  pdcp_traffic_source(stress_test_args& args) :
    args(args), rgen(args.seed), int_dist(args.min_sdu_size, args.max_sdu_size)
  {
  }

  void set_rlc_tx_upper(rlc_tx_upper_layer_data_interface* rlc_tx_upper) { this->rlc_tx_upper = rlc_tx_upper; }

  void send_pdu();

  // rlc_tx_upper_layer_data_notifier interface
  void on_delivered_sdu(uint32_t pdcp_count) final {}

  // rlc_tx_upper_layer_control_notifier interface
  void on_protocol_failure() final {}
  void on_max_retx() final {}
};

class mac_dummy : public rlc_tx_lower_layer_notifier
{
  stress_test_args&     args;
  srslog::basic_logger& logger;

  std::mt19937                          rgen;
  std::uniform_real_distribution<float> real_dist;

  std::atomic<unsigned>                bsr;
  std::vector<byte_buffer_slice_chain> pdu_list;

  rlc_tx_lower_layer_interface* rlc_tx_lower = nullptr;
  rlc_rx_lower_layer_interface* rlc_rx_lower = nullptr;

public:
  mac_dummy(stress_test_args& args) :
    args(args), logger(srslog::fetch_basic_logger("MAC", false)), rgen(args.seed), bsr(0){};

  void run_tx_tti();
  void run_rx_tti();

  // rlc_tx_lower_layer_notifier interface
  void on_buffer_state_update(unsigned bsr) final { this->bsr.store(bsr, std::memory_order_relaxed); }

  void set_rlc_tx_lower(rlc_tx_lower_layer_interface* rlc_tx_lower) { this->rlc_tx_lower = rlc_tx_lower; }
  void set_rlc_rx_lower(rlc_rx_lower_layer_interface* rlc_rx_lower) { this->rlc_rx_lower = rlc_rx_lower; }
};

} // namespace srsgnb
