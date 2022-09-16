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
#include "srsgnb/srslog/srslog.h"
#include <getopt.h>

namespace srsgnb {
struct stress_test_args {
  std::string mode         = "UM12";
  std::string log_filename = "stdout";
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

class pdcp_rx_dummy : public rlc_rx_upper_layer_data_notifier
{
  void on_new_sdu(byte_buffer_slice_chain pdu) final {}
};

} // namespace srsgnb
