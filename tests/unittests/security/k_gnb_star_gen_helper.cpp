/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/expected.h"
#include "srsran/security/security.h"
#include "srsran/srslog/logger.h"
#include <cstdlib>
#include <getopt.h>

using namespace srsran;

struct k_star_gen_helper_args {
  std::string k;
  uint32_t    t_ssb_arfcn = {};
  uint16_t    pci         = 0;
};

/// Converts a hex string (e.g. 01FA02) to a sec_as_key.
expected<security::sec_key> make_sec_key(const std::string& hex_str)
{
  expected<byte_buffer> ret = make_byte_buffer(hex_str);
  if (not ret.has_value()) {
    return make_unexpected(default_error_t{});
  }
  byte_buffer key_buf = std::move(ret.value());
  if (key_buf.length() != 32) {
    return make_unexpected(default_error_t{});
  }
  security::sec_key key = {};
  std::copy(key_buf.begin(), key_buf.end(), key.begin());
  return key;
}

bool parse_args(k_star_gen_helper_args& args, int argc, char* argv[])
{
  static const struct option long_options[] = {{"help", no_argument, nullptr, 'h'},
                                               {"sn_size", required_argument, nullptr, 's'},
                                               {"count", required_argument, nullptr, 'c'},
                                               {"algo", required_argument, nullptr, 'a'},
                                               {nullptr, 0, nullptr, 0}};

  static const char usage[] = "Usage: k_gnb_star_gen_helper [options]\n"
                              "\n"
                              "  -h, --help                  Show help message and quit.\n"
                              "  -p, --pci <PCI>             PCI used to regenerate key\n"
                              "  -s, --ssb-arfcn <SSB-ARFCN> SSB-ARFCN used to generate K_gNB star\n"
                              "  -k, --key <KEY>             K_gNB from which to derive K_gNB star\n"
                              "\n";
  // Parse arguments
  while (true) {
    int option_index = 0;
    int c            = getopt_long(argc, argv, "hp:s:k:", long_options, &option_index);
    if (c == -1) {
      break;
    }

    switch (c) {
      case 'h':
        fprintf(stdout, "%s", usage);
        break;
      case 'k':
        args.k = std::string(optarg);
        fprintf(stdout, "K_gNB %s\n", args.k.c_str());
        break;
      case 'p':
        args.pci = strtod(optarg, nullptr);
        fprintf(stdout, "PCI %u\n", args.pci);
        break;
      case 's':
        args.t_ssb_arfcn = strtod(optarg, nullptr);
        fprintf(stdout, "SSB-ARFCN %u\n", args.t_ssb_arfcn);
        break;
      default:
        fprintf(stderr, "error parsing arguments\n");
        return false;
    }
  }
  return true;
}

int main(int argc, char** argv)
{
  k_star_gen_helper_args args;
  parse_args(args, argc, argv);

  srslog::init();
  srslog::basic_logger& logger = srslog::fetch_basic_logger("SEC", false);
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(1500);

  // Validate args
  expected<security::sec_key> exp_key = make_sec_key(args.k);
  if (not exp_key.has_value()) {
    logger.error("Invalid security key provided");
    return -1;
  }
  if (args.pci > 1007) {
    logger.error("Invalid PCI");
    return -1;
  }

  security::sec_key k      = exp_key.value();
  security::sec_key k_star = {};
  pci_t             t_pci{args.pci};
  uint32_t          t_ssb_arfcn{args.t_ssb_arfcn};

  security::generate_k_ng_ran_star(k_star, k, t_pci, t_ssb_arfcn);
  logger.info("k_star = {}", k_star);
  logger.info("k = {}", k);
  return 0;
}
