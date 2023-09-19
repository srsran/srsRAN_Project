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

#include "lib/pdcp/pdcp_entity_tx.h"
#include "pdcp_test_vectors.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/pdcp/pdcp_tx.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/timers.h"
#include <cstdlib>
#include <getopt.h>
#include <queue>

using namespace srsran;

struct pdcp_gen_helper_args {
  std::string sn_size = {};
  uint32_t    count   = {};
};

/// Mocking class of the surrounding layers invoked by the PDCP.
class pdcp_tx_gen_frame : public pdcp_tx_lower_notifier, public pdcp_tx_upper_control_notifier
{
public:
  std::queue<pdcp_tx_pdu> pdu_queue   = {};
  uint32_t                pdu_counter = 0;

  /// PDCP TX upper layer control notifier
  void on_max_count_reached() final {}
  void on_protocol_failure() final {}

  /// PDCP TX lower layer data notifier
  void on_new_pdu(pdcp_tx_pdu pdu) final { pdu_queue.push(std::move(pdu)); }
  void on_discard_pdu(uint32_t pdcp_sn) final {}
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
        fprintf(stdout, "PDCP COUNT %u\n", args.count);
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
  logger.set_hex_dump_max_size(1500);

  pdcp_sn_size sn_size = args.sn_size == "12" ? pdcp_sn_size::size12bits : pdcp_sn_size::size18bits;
  logger.info("Creating PDCP TX ({} bit)", sn_size);

  timer_manager      timers;
  manual_task_worker worker{64};

  // Set TX config
  pdcp_tx_config config         = {};
  config.rb_type                = pdcp_rb_type::drb;
  config.rlc_mode               = pdcp_rlc_mode::am;
  config.sn_size                = sn_size;
  config.direction              = pdcp_security_direction::downlink;
  config.discard_timer          = pdcp_discard_timer::ms10;
  config.status_report_required = false;

  security::sec_128_as_config sec_cfg = {};

  // Set security domain
  sec_cfg.domain = security::sec_domain::up; // DRB

  // Set security keys
  sec_cfg.k_128_int = k_128_int;
  sec_cfg.k_128_enc = k_128_enc;

  // Set encription/integrity algorithms
  sec_cfg.integ_algo  = security::integrity_algorithm::nia1;
  sec_cfg.cipher_algo = security::ciphering_algorithm::nea1;

  pdcp_tx_gen_frame frame = {};
  // Create RLC entities
  std::unique_ptr<pdcp_entity_tx> pdcp_tx =
      std::make_unique<pdcp_entity_tx>(0, drb_id_t::drb1, config, frame, frame, timer_factory{timers, worker});
  pdcp_tx_state st = {args.count};
  pdcp_tx->set_state(st);
  pdcp_tx->configure_security(sec_cfg);

  // Write SDU
  byte_buffer sdu = {sdu1};
  pdcp_tx->handle_sdu(std::move(sdu));
  logger.info(frame.pdu_queue.front().buf.begin(), frame.pdu_queue.front().buf.end(), "PDCP PDU");
  return 0;
}
