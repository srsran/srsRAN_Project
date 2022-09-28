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
  bool        const_opp          = false;
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
                                               {"sdu_size", required_argument, nullptr, 's'},
                                               {"min_sdu_size", required_argument, nullptr, 'z'},
                                               {"max_sdu_size", required_argument, nullptr, 'Z'},
                                               {"const_opp", no_argument, nullptr, 'o'},
                                               {"avg_opp_size", required_argument, nullptr, 'p'},
                                               {"pdu_drop_rate", required_argument, nullptr, 'd'},
                                               {"pdu_cut_rate", required_argument, nullptr, 'c'},
                                               {"pdu_duplicate_rate", required_argument, nullptr, 'D'},
                                               {"log_filename", required_argument, nullptr, 'l'},
                                               {"seed", required_argument, nullptr, 'S'},
                                               {"nof_pdu_tti", required_argument, nullptr, 'T'},
                                               {nullptr, 0, nullptr, 0}};
  // clang-format off
  static const char usage[] =
    "Usage: rlc_stress_test [options]\n"
    "\n"
    "  -h, --help                      Show help message and quit.\n"
    "  -m, --mode <RLC mode>           Set RLC mode {TM, UM6, UM12, AM12, AM18}.\n"
    "  -s, --sdu_size <size>           Set SDU size (-1 means random).\n"
    "  -z, --min_sdu_size <size>       Set minimum SDU size (for randomly selected SDU size).\n"
    "  -Z, --max_sdu_size <size>       Set maximum SDU size (for randomly selected SDU size).\n"
    "  -o, --const_opp                 Generate constant-size MAC opportunities (default: random).\n"
    "  -p, --avg_opp_size <size>       Set average MAC opportunity size.\n"
    "  -d, --pdu_drop_rate <rate>      Set rate at which RLC PDUs are dropped.\n"
    "  -c, --pdu_cut_rate <rate>       Set rate at which RLC PDUs are chopped in length.\n"
    "  -D, --pdu_duplicate_rate <rate> Set rate at which RLC PDUs are dropped.\n"
    "  -l, --log_filename <filename>   Set log filename. Use 'stdout' to print to console.\n"
    "  -S, --seed <seed>               Set seed to use in run. 0 means the seed is randomly generated.\n"
    "  -T, --nof_pdu_tti <num>         Set number of PDUs processed in a TTI.\n"
    "\n";
  // clang-format on

  // Parse arguments
  while (true) {
    int option_index = 0;
    int c            = getopt_long(argc, argv, "hm:s:z:Z:op:d:c:D:l:S:T:", long_options, &option_index);
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
      case 's':
        args.sdu_size = std::strtol(optarg, nullptr, 10);
        fprintf(stdout, "SDU size %d\n", args.sdu_size);
        break;
      case 'z':
        args.min_sdu_size = std::strtol(optarg, nullptr, 10);
        fprintf(stdout, "Minimum SDU size %d\n", args.min_sdu_size);
        break;
      case 'Z':
        args.max_sdu_size = std::strtol(optarg, nullptr, 10);
        fprintf(stdout, "Maximum SDU size %d\n", args.max_sdu_size);
        break;
      case 'o':
        args.const_opp = true;
        fprintf(stdout, "Using constant-size MAC opportunities\n");
        break;
      case 'p':
        args.avg_opp_size = std::strtol(optarg, nullptr, 10);
        fprintf(stdout, "Average MAC opportunity size %d\n", args.avg_opp_size);
        break;
      case 'd':
        args.pdu_drop_rate = std::strtod(optarg, nullptr);
        fprintf(stdout, "RLC PDU drop rate %f\n", args.pdu_drop_rate);
        break;
      case 'c':
        args.pdu_cut_rate = std::strtod(optarg, nullptr);
        fprintf(stdout, "RLC PDU cut rate %f\n", args.pdu_cut_rate);
        break;
      case 'D':
        args.pdu_duplicate_rate = std::strtod(optarg, nullptr);
        fprintf(stdout, "RLC PDU duplicate rate %f\n", args.pdu_duplicate_rate);
        break;
      case 'l':
        args.log_filename = std::string(optarg);
        fprintf(stdout, "Log filename %s\n", args.log_filename.c_str());
        break;
      case 'S':
        args.seed = std::strtol(optarg, nullptr, 10);
        fprintf(stdout, "Seed %d\n", args.seed);
        break;
      case 'T':
        args.nof_pdu_tti = std::strtol(optarg, nullptr, 10);
        fprintf(stdout, "PDUs per TTI %d\n", args.nof_pdu_tti);
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
  uint32_t          pdcp_count = 0;
  uint32_t          sdu_size   = 0;
  uint8_t           payload    = 0x0; // increment for each SDU byte

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
