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

/// \file
/// \brief LDPC decoder benchmark.

#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/srsvec/bit.h"
#include "srsran/support/benchmark_utils.h"
#include "srsran/support/srsran_test.h"
#include <getopt.h>
#include <random>

static std::mt19937 rgen(0);
static std::string  dec_type        = "generic";
static unsigned     nof_repetitions = 1000;
static unsigned     nof_iterations  = 6;
static bool         silent          = false;
static bool         use_crc         = false;
static unsigned     l_size          = 0;

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-R repetitions] [-s silent]\n", prog);
  fmt::print("\t-R Repetitions [Default {}]\n", nof_repetitions);
  fmt::print("\t-T Encoder type generic, avx2, avx512 or neon [Default {}]\n", dec_type);
  fmt::print("\t-I Number of min-sum iterations [Default {}]\n", nof_iterations);
  fmt::print("\t-s Toggle silent operation [Default {}]\n", silent);
  fmt::print("\t-C Toggle early stopping with CRC [Default {}]\n", use_crc);
  fmt::print("\t-L Lifting size - 0 for all [Default {}]\n", l_size);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:T:I:L:Csh")) != -1) {
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
      case 'L':
        l_size = std::strtol(optarg, nullptr, 10);
        break;
      case 'C':
        use_crc = (!use_crc);
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

  benchmarker perf_meas_generic(fmt::format("LDPC decoder {}, {} MS iterations", dec_type, nof_iterations),
                                nof_repetitions);

  span<const lifting_size_t>    use_ls;
  std::array<lifting_size_t, 1> one_ls = {lifting_size_t::LS384};
  if (l_size != 0) {
    one_ls[0] = static_cast<lifting_size_t>(l_size);
    use_ls    = span<const lifting_size_t>(one_ls);
  } else {
    use_ls = span<const lifting_size_t>(all_lifting_sizes);
  }

  for (const ldpc_base_graph_type& bg : {ldpc_base_graph_type::BG1, ldpc_base_graph_type::BG2}) {
    for (const lifting_size_t& ls : use_ls) {
      std::unique_ptr<crc_calculator> crc16   = nullptr;
      std::unique_ptr<ldpc_encoder>   encoder = nullptr;
      // Decoder.
      std::shared_ptr<ldpc_decoder_factory> decoder_factory = create_ldpc_decoder_factory_sw(dec_type);
      TESTASSERT(decoder_factory);
      std::unique_ptr<ldpc_decoder> decoder = decoder_factory->create();
      TESTASSERT(decoder);

      if (use_crc) {
        // Encoder.
        std::shared_ptr<ldpc_encoder_factory> enc_factory = create_ldpc_encoder_factory_sw("auto");
        TESTASSERT(enc_factory);
        encoder = enc_factory->create();
        TESTASSERT(encoder);
        // CRC.
        std::shared_ptr<crc_calculator_factory> crc_factory = create_crc_calculator_factory_sw("auto");
        crc16                                               = crc_factory->create(crc_generator_poly::CRC16);
        TESTASSERT(crc16);
      }

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
        std::vector<log_likelihood_ratio> codeblock(cb_length);
        if (!use_crc) {
          // Generate a random codeblock.
          std::generate(
              codeblock.begin(), codeblock.end(), [&]() { return static_cast<int8_t>((rgen() & 1) * 20 - 10); });
        } else {
          // Generate random message, attach its CRC and encode.
          dynamic_bit_buffer to_encode(msg_length);
          dynamic_bit_buffer encoded(cb_length);
          // Generate a random message.
          unsigned   msg_len_minus_crc = msg_length - 16;
          bit_buffer msg_span          = to_encode.first(msg_len_minus_crc);
          for (unsigned i_bit = 0; i_bit != msg_length; ++i_bit) {
            msg_span.insert(rgen() & 1, i_bit, 1);
          }
          // Add CRC bits at the end.
          unsigned checksum = crc16->calculate(msg_span);
          to_encode.insert(checksum, msg_len_minus_crc, 16);
          // Encode entire message.
          srsran::codeblock_metadata::tb_common_metadata cfg_enc;
          cfg_enc = {bg, ls};
          encoder->encode(encoded, to_encode, cfg_enc);

          // Convert codeblock bits to LLRs.
          for (unsigned i_bit = 0; i_bit != cb_length; ++i_bit) {
            codeblock[i_bit] = log_likelihood_ratio::copysign(10, 1 - 2 * encoded.extract(i_bit, 1));
          }
        }

        // Prepare message storage.
        dynamic_bit_buffer message(msg_length);

        srsran::codeblock_metadata cfg_dec  = {};
        cfg_dec.tb_common.lifting_size      = ls;
        cfg_dec.tb_common.base_graph        = bg;
        cfg_dec.cb_specific.nof_crc_bits    = 16;
        cfg_dec.cb_specific.nof_filler_bits = 0;

        fmt::memory_buffer descr_buffer;
        fmt::format_to(descr_buffer, "BG={} LS={:<3} cb_len={}", bg, ls, cb_length);
        perf_meas_generic.new_measure(to_string(descr_buffer), codeblock.size(), [&]() {
          decoder->decode(message, codeblock, crc16.get(), {cfg_dec, {nof_iterations, 0.8}});
          do_not_optimize(codeblock);
        });
      }
    }
  }
  perf_meas_generic.print_percentiles_throughput("bits");
}
