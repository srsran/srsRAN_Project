/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief LDPC decoder benchmark.

#include "srsgnb/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsgnb/support/benchmark_utils.h"
#include "srsgnb/support/srsgnb_test.h"
#include <getopt.h>
#include <random>

static std::mt19937 rgen(0);
static std::string  dec_type        = "generic";
static unsigned     nof_repetitions = 1000;
static unsigned     nof_iterations  = 6;
static bool         silent          = false;

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-R repetitions] [-s silent]\n", prog);
  fmt::print("\t-R Repetitions [Default {}]\n", nof_repetitions);
  fmt::print("\t-T Encoder type generic or avx2 [Default {}]\n", dec_type);
  fmt::print("\t-I Number of min-sum iterations [Default {}]\n", nof_iterations);
  fmt::print("\t-s Toggle silent operation [Default {}]\n", silent);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:T:I:sh")) != -1) {
    switch (opt) {
      case 'R':
        nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'T':
        dec_type = std::string(optarg);
        break;
      case 'I':
        nof_iterations = std::strtol(optarg, nullptr, 10);
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

using namespace srsgnb;
using namespace srsgnb::ldpc;

int main(int argc, char** argv)
{
  parse_args(argc, argv);

  benchmarker perf_meas_generic(fmt::format("LDPC decoder {}, {} MS iterations", dec_type, nof_iterations),
                                nof_repetitions);

  for (const ldpc_base_graph_type& bg : {ldpc_base_graph_type::BG1, ldpc_base_graph_type::BG2}) {
    for (const lifting_size_t& ls : all_lifting_sizes) {
      std::shared_ptr<ldpc_decoder_factory> decoder_factory = create_ldpc_decoder_factory_sw(dec_type);
      TESTASSERT(decoder_factory);
      std::unique_ptr<ldpc_decoder> decoder = decoder_factory->create();
      TESTASSERT(decoder);

      // Set base-graph message and codeblock lengths.
      unsigned min_cb_length_bg = 24;
      unsigned max_cb_length_bg = 66;
      unsigned msg_length_bg    = 22;
      if (bg == srsgnb::ldpc_base_graph_type::BG2) {
        min_cb_length_bg = 12;
        max_cb_length_bg = 50;
        msg_length_bg    = 10;
      }

      // Compute lifted messages and codeblock lengths.
      unsigned min_cb_length = min_cb_length_bg * ls;
      unsigned max_cb_length = max_cb_length_bg * ls;
      unsigned msg_length    = msg_length_bg * ls;

      for (unsigned cb_length : {min_cb_length, max_cb_length}) {
        // Generate a random codeblock.
        std::vector<log_likelihood_ratio> codeblock(cb_length);
        std::generate(
            codeblock.begin(), codeblock.end(), [&]() { return static_cast<int8_t>((rgen() & 1) * 20 - 10); });

        // Prepare message storage.
        std::vector<uint8_t> message(msg_length);

        srsgnb::codeblock_metadata cfg_dec  = {};
        cfg_dec.tb_common.lifting_size      = ls;
        cfg_dec.tb_common.base_graph        = bg;
        cfg_dec.cb_specific.nof_crc_bits    = 16;
        cfg_dec.cb_specific.nof_filler_bits = 0;

        fmt::memory_buffer descr_buffer;
        fmt::format_to(descr_buffer, "BG={} LS={:<3} cb_len={}", bg, ls, cb_length);

        perf_meas_generic.new_measure(to_string(descr_buffer), codeblock.size(), [&]() {
          decoder->decode(message, codeblock, nullptr, {cfg_dec, {nof_iterations, 0.8}});
          do_not_optimize(codeblock);
        });
      }
    }
  }
  perf_meas_generic.print_percentiles_throughput("bits");
}
