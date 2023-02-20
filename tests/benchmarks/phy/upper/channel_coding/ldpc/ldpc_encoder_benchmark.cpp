/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsgnb/support/benchmark_utils.h"
#include "srsgnb/support/srsgnb_test.h"
#include <getopt.h>
#include <random>

namespace {
std::mt19937 rgen(0);
std::string  enc_type        = "generic";
unsigned     nof_repetitions = 1000;
bool         silent          = false;
} // namespace

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-R repetitions] [-s silent]\n", prog);
  fmt::print("\t-R Repetitions [Default {}]\n", nof_repetitions);
  fmt::print("\t-T Encoder type generic or avx2[Default {}]\n", enc_type);
  fmt::print("\t-s Toggle silent operation [Default {}]\n", silent);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:T:sh")) != -1) {
    switch (opt) {
      case 'R':
        nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'T':
        enc_type = std::string(optarg);
        break;
      case 's':
        silent = (!silent);
        break;
      case 'h':
      default:
        usage(argv[0]);
        exit(0);
    }
  }
}

using namespace srsran;
using namespace srsran::ldpc;

int main(int argc, char** argv)
{
  parse_args(argc, argv);

  benchmarker perf_meas_generic("LDPC encoder " + enc_type, nof_repetitions);

  for (const ldpc_base_graph_type& bg : {ldpc_base_graph_type::BG1, ldpc_base_graph_type::BG2}) {
    for (const lifting_size_t& ls : all_lifting_sizes) {
      std::shared_ptr<ldpc_encoder_factory> encoder_factory = create_ldpc_encoder_factory_sw(enc_type);
      TESTASSERT(encoder_factory);
      std::unique_ptr<ldpc_encoder> encoder = encoder_factory->create();
      TESTASSERT(encoder);

      // Set base-graph message and codeblock lengths.
      unsigned min_cb_length_bg = 24;
      unsigned max_cb_length_bg = 66;
      unsigned msg_length_bg    = 22;
      if (bg == srsran::ldpc_base_graph_type::BG2) {
        min_cb_length_bg = 12;
        max_cb_length_bg = 50;
        msg_length_bg    = 10;
      }

      // Compute lifted messages and codeblock lengths.
      unsigned min_cb_length = min_cb_length_bg * ls;
      unsigned max_cb_length = max_cb_length_bg * ls;
      unsigned msg_length    = msg_length_bg * ls;

      for (unsigned cb_length : {min_cb_length, max_cb_length}) {
        // Generate message data.
        std::vector<uint8_t> data(msg_length);
        std::generate(data.begin(), data.end(), [&]() { return static_cast<uint8_t>(rgen() & 1); });

        // Generate codeblock.
        std::vector<uint8_t> codeblock(cb_length);

        srsran::codeblock_metadata::tb_common_metadata cfg_enc = {bg, ls};

        fmt::memory_buffer descr_buffer;
        fmt::format_to(descr_buffer, "BG={} LS={:<3} cb_len={}", bg, ls, cb_length);

        perf_meas_generic.new_measure(to_string(descr_buffer), data.size(), [&]() {
          encoder->encode(codeblock, data, cfg_enc);
          do_not_optimize(codeblock);
        });
      }
    }
  }
  perf_meas_generic.print_percentiles_throughput("bits");
}
