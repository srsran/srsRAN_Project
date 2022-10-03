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

#include "srsgnb/rlc/rlc_config.h"
#include "srsgnb/rlc/rlc_rx.h"
#include "srsgnb/rlc/rlc_tx.h"
#include "srsgnb/srslog/srslog.h"
#include <getopt.h>
#include <random>

namespace srsgnb {

struct stress_test_args {
  std::string          mode               = "UM12";
  int32_t              sdu_size           = -1;
  float                pdu_drop_rate      = 0.1;
  float                pdu_cut_rate       = 0.0;
  float                pdu_duplicate_rate = 0.0;
  uint32_t             avg_opp_size       = 1505;
  bool                 const_opp          = false;
  uint32_t             seed               = 0;
  uint32_t             nof_pdu_tti        = 1;
  std::string          log_filename       = "stdout";
  srslog::basic_levels log_level          = srslog::basic_levels::debug;
  uint32_t             log_hex_limit      = 32;
  uint32_t             min_sdu_size       = 5;
  uint32_t             max_sdu_size       = 9000;
};

inline bool parse_args(stress_test_args& args, int argc, char* argv[])
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
                                               {"log_level", required_argument, nullptr, 'L'},
                                               {"log_hex_limit", required_argument, nullptr, 'H'},
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
    "  -L, --log_level <level>         Set log level (default: debug).\n"
    "  -H, --log_hex_limit <hex_limit> Set log limit for hex dumps (default: 32 bytes).\n"
    "  -S, --seed <seed>               Set seed to use in run. 0 means the seed is randomly generated.\n"
    "  -T, --nof_pdu_tti <num>         Set number of PDUs processed in a TTI.\n"
    "\n";
  // clang-format on

  // Parse arguments
  while (true) {
    int option_index = 0;
    int c            = getopt_long(argc, argv, "hm:s:z:Z:op:d:c:D:l:L:H:S:T:", long_options, &option_index);
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
      case 'L':
        args.log_level = srslog::str_to_basic_level(std::string(optarg));
        fprintf(stdout, "Log level %s\n", optarg);
        break;
      case 'H':
        args.log_hex_limit = std::strtol(optarg, nullptr, 10);
        fprintf(stdout, "Log hex limit %d\n", args.log_hex_limit);
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

inline rlc_config get_rlc_config_from_args(const stress_test_args& args)
{
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
  return cnfg;
}

inline void init_log_from_args(const stress_test_args& args)
{
  srslog::init();

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

  auto& log_stack = srslog::fetch_basic_logger("STACK", false);
  log_stack.set_level(args.log_level);
  log_stack.set_hex_dump_max_size(args.log_hex_limit);

  auto& log_pdcp = srslog::fetch_basic_logger("PDCP", false);
  log_pdcp.set_level(args.log_level);
  log_pdcp.set_hex_dump_max_size(args.log_hex_limit);

  auto& log_rlc = srslog::fetch_basic_logger("RLC", false);
  log_rlc.set_level(args.log_level);
  log_rlc.set_hex_dump_max_size(args.log_hex_limit);

  auto& log_mac = srslog::fetch_basic_logger("MAC", false);
  log_mac.set_level(args.log_level);
  log_mac.set_hex_dump_max_size(args.log_hex_limit);
}
} // namespace srsgnb
