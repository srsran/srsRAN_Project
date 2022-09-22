/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../lib/pdcp/pdcp_entity_impl.h"
#include "pdcp_test_vectors.h"
#include "srsgnb/pdcp/pdcp_config.h"
#include <cstdlib>
#include <getopt.h>
#include <queue>

using namespace srsgnb;

struct pdcp_gen_helper_args {
  std::string sn_size = {};
  uint32_t    count   = {};
};

/// Mocking class of the surrounding layers invoked by the PDCP.
class pdcp_tx_gen_frame : public pdcp_tx_lower_notifier, public pdcp_tx_upper_control_notifier
{
public:
  std::queue<byte_buffer> pdu_queue   = {};
  uint32_t                pdu_counter = 0;

  /// PDCP TX upper layer control notifier
  void on_max_hfn_reached() final {}
  void on_protocol_failure() final {}

  /// PDCP TX lower layer data notifier
  void on_new_pdu(byte_buffer pdu) final { pdu_queue.push(std::move(pdu)); }
  void on_discard_pdu(uint32_t count) final {}
};

bool parse_args(pdcp_gen_helper_args& args, int argc, char* argv[])
{
  static const struct option long_options[] = {{"help", no_argument, nullptr, 'h'},
                                               {"sn_size", required_argument, nullptr, 's'},
                                               {"count", required_argument, nullptr, 'c'},
                                               {nullptr, 0, nullptr, 0}};

  static const char usage[] = "Usage: pdcp_gen_helper [options]\n"
                              "\n"
                              "  -h, --help                 Show help message and quit.\n"
                              "  -s, --sn_size <SN size>    Specify 12bit, or 18bit\n"
                              "  -c, --count <COUNT>        Specify COUNT of PDU to generate\n"
                              "\n";
  // Parse arguments
  while (true) {
    int option_index = 0;
    int c            = getopt_long(argc, argv, "hs:c:", long_options, &option_index);
    if (c == -1) {
      break;
    }

    switch (c) {
      case 'h':
        fprintf(stdout, "%s", usage);
        break;
      case 's':
        args.sn_size = std::string(optarg);
        fprintf(stdout, "PDCP SN Size %s\n", args.sn_size.c_str());
        break;
      case 'c':
        args.count = strtod(optarg, nullptr);
        fprintf(stdout, "PDCP COUNT %d\n", args.count);
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
  pdcp_gen_helper_args args;
  parse_args(args, argc, argv);

  srslog::init();
  srslog::basic_logger& logger = srslog::fetch_basic_logger("PDCP", false);
  logger.set_level(srslog::basic_levels::debug);

  pdcp_sn_size sn_size = args.sn_size == "12" ? pdcp_sn_size::size12bits : pdcp_sn_size::size18bits;
  logger.info("Creating PDCP TX ({} bit)", sn_size);

  timer_manager timers;

  // Set TX config
  pdcp_config::pdcp_tx_config config = {};
  config.rb_type                     = pdcp_rb_type::drb;
  config.rlc_mode                    = pdcp_rlc_mode::am;
  config.sn_size                     = sn_size;
  config.discard_timer               = pdcp_discard_timer::ms10;

  // Set security keys
  sec_128_as_config sec_cfg = {};
  sec_cfg.k_128_rrc_int     = k_128_int;
  sec_cfg.k_128_up_int      = k_128_int;
  sec_cfg.k_128_rrc_enc     = k_128_enc;
  sec_cfg.k_128_up_enc      = k_128_enc;

  // Set encription/integrity algorithms
  sec_cfg.integ_algo  = integrity_algorithm::nia1;
  sec_cfg.cipher_algo = ciphering_algorithm::nea1;

  pdcp_tx_gen_frame frame = {};
  // Create RLC entities
  std::unique_ptr<pdcp_entity_tx> pdcp_tx =
      std::make_unique<pdcp_entity_tx>(0, LCID_SRB1, config, frame, frame, timers);
  pdcp_tx_state st = {args.count};
  pdcp_tx->set_state(st);
  pdcp_tx->set_as_security_config(sec_cfg);
  pdcp_tx->enable_or_disable_security(pdcp_integrity_enabled::enabled, pdcp_ciphering_enabled::enabled);

  // Write SDU
  byte_buffer sdu = {sdu1};
  pdcp_tx->handle_sdu(std::move(sdu));
  logger.info(frame.pdu_queue.front().begin(), frame.pdu_queue.front().end(), "PDCP PDU");
  return 0;
}
