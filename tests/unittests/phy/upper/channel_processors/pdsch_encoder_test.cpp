/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "pdsch_encoder_test_data.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/support/srsran_test.h"
#ifdef HWACC_PDSCH_ENABLED
#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include "srsran/hal/dpdk/bbdev/bbdev_acc_factory.h"
#include "srsran/hal/dpdk/dpdk_eal_factory.h"
#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_factories.h"
#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc_factory.h"
#endif // HWACC_PDSCH_ENABLED
#include <getopt.h>

using namespace srsran;
using namespace srsran::ldpc;

static std::string encoder_type = "generic";

#ifdef HWACC_PDSCH_ENABLED
static bool        cb_mode       = false;
static std::string hal_log_level = "ERROR";
static bool        std_out_sink  = true;
static std::string eal_arguments = "";
#endif // HWACC_PDSCH_ENABLED

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-T X] [-x] [-y] [-z error|warning|info|debug] [-h] [eal_args ...]\n", prog);
  fmt::print("\t-T       PDSCH encoder type [generic,acc100][Default {}]\n", encoder_type);
#ifdef HWACC_PDSCH_ENABLED
  fmt::print("\t-x       Force TB mode [Default {}]\n", cb_mode ? "cb_mode" : "tb_mode");
  fmt::print("\t-y       Force logging output written to a file [Default {}]\n", std_out_sink ? "std_out" : "file");
  fmt::print("\t-z       Set logging level for the HAL [Default {}]\n", hal_log_level);
  fmt::print("\teal_args EAL arguments\n");
#endif // HWACC_PDSCH_ENABLED
  fmt::print("\t-h       This help\n");
}

#ifdef HWACC_PDSCH_ENABLED
// Separates EAL and non-EAL arguments.
// The function assumes that 'eal_arg' flags the start of the EAL arguments and that no more non-EAL arguments follow.
static std::string capture_eal_args(int* argc, char*** argv)
{
  // Searchs for the EAL args (if any), flagged by 'eal_args', while removing all the rest (except argv[0]).
  bool        eal_found = false;
  char**      mod_argv  = *argv;
  std::string eal_argv  = {mod_argv[0]};
  int         opt_ind   = *argc;
  for (int j = 1; j < opt_ind; ++j) {
    // Search for the 'eal_args' flag (if any).
    if (!eal_found) {
      if (strcmp(mod_argv[j], "eal_args") == 0) {
        // 'eal_args' flag found.
        eal_found = true;
        // Remove all main app arguments starting from that point, while copying them to the EAL argument string.
        mod_argv[j] = NULL;
        for (int k = j + 1; k < opt_ind; ++k) {
          eal_argv += " ";
          eal_argv += mod_argv[k];
          mod_argv[k] = NULL;
        }
        *argc = j;
      }
    }
  }
  *argv = mod_argv;

  return eal_argv;
}
#endif // HWACC_PDSCH_ENABLED

static void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "T:xyz:h")) != -1) {
    switch (opt) {
      case 'T':
        encoder_type = std::string(optarg);
        break;
#ifdef HWACC_PDSCH_ENABLED
      case 'x':
        cb_mode = true;
        break;
      case 'y':
        std_out_sink = false;
        break;
      case 'z':
        hal_log_level = std::string(optarg);
        break;
#endif // HWACC_PDSCH_ENABLED
      case 'h':
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

static std::shared_ptr<pdsch_encoder_factory> create_generic_pdsch_encoder_factory()
{
  std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
  TESTASSERT(crc_calc_factory);

  std::shared_ptr<ldpc_encoder_factory> ldpc_encoder_factory = create_ldpc_encoder_factory_sw("generic");
  TESTASSERT(ldpc_encoder_factory);

  std::shared_ptr<ldpc_rate_matcher_factory> ldpc_rate_matcher_factory = create_ldpc_rate_matcher_factory_sw();
  TESTASSERT(ldpc_rate_matcher_factory);

  std::shared_ptr<ldpc_segmenter_tx_factory> segmenter_factory = create_ldpc_segmenter_tx_factory_sw(crc_calc_factory);
  TESTASSERT(segmenter_factory);

  pdsch_encoder_factory_sw_configuration encoder_factory_config;
  encoder_factory_config.encoder_factory      = ldpc_encoder_factory;
  encoder_factory_config.rate_matcher_factory = ldpc_rate_matcher_factory;
  encoder_factory_config.segmenter_factory    = segmenter_factory;
  return create_pdsch_encoder_factory_sw(encoder_factory_config);
}

static std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> create_hw_accelerator_pdsch_enc_factory()
{
#ifdef HWACC_PDSCH_ENABLED
  srslog::sink* log_sink =
      std_out_sink ? srslog::create_stdout_sink() : srslog::create_file_sink("pdsch_encoder_vectortest.log");
  srslog::set_default_sink(*log_sink);
  srslog::init();
  srslog::basic_logger& logger = srslog::fetch_basic_logger("HAL", false);
  logger.set_level(srslog::str_to_basic_level(hal_log_level));

  // Pointer to a dpdk-based hardware-accelerator interface.
  static std::unique_ptr<dpdk::dpdk_eal> dpdk_interface = nullptr;
  if (!dpdk_interface) {
    dpdk_interface = dpdk::create_dpdk_eal(eal_arguments, logger);
    TESTASSERT(dpdk_interface, "Failed to open DPDK EAL with arguments.");
  }

  // Intefacing to the bbdev-based hardware-accelerator.
  dpdk::bbdev_acc_configuration bbdev_config;
  bbdev_config.id                                    = 0;
  bbdev_config.nof_ldpc_enc_lcores                   = 1;
  bbdev_config.nof_ldpc_dec_lcores                   = 0;
  bbdev_config.nof_fft_lcores                        = 0;
  bbdev_config.nof_mbuf                              = static_cast<unsigned>(pow2(log2_ceil(MAX_NOF_SEGMENTS)));
  std::shared_ptr<dpdk::bbdev_acc> bbdev_accelerator = create_bbdev_acc(bbdev_config, logger);
  TESTASSERT(bbdev_accelerator);

  // Set the hardware-accelerator configuration.
  hw_accelerator_pdsch_enc_configuration hw_encoder_config;
  hw_encoder_config.acc_type          = "acc100";
  hw_encoder_config.bbdev_accelerator = bbdev_accelerator;

  // ACC100 hardware-accelerator implementation.
  return create_hw_accelerator_pdsch_enc_factory(hw_encoder_config);
#else  // HWACC_PDSCH_ENABLED
  return nullptr;
#endif // HWACC_PDSCH_ENABLED
}

static std::shared_ptr<pdsch_encoder_factory> create_acc100_pdsch_encoder_factory()
{
  std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
  TESTASSERT(crc_calc_factory);

  std::shared_ptr<ldpc_segmenter_tx_factory> segmenter_factory = create_ldpc_segmenter_tx_factory_sw(crc_calc_factory);
  TESTASSERT(segmenter_factory);

  std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> hw_encoder_factory = create_hw_accelerator_pdsch_enc_factory();
  TESTASSERT(hw_encoder_factory, "Failed to create a HW acceleration encoder factory.");

  // Set the hardware-accelerated PDSCH encoder configuration.
  pdsch_encoder_factory_hw_configuration encoder_hw_factory_config;
#ifdef HWACC_PDSCH_ENABLED
  encoder_hw_factory_config.cb_mode     = cb_mode;
  encoder_hw_factory_config.max_tb_size = RTE_BBDEV_LDPC_E_MAX_MBUF;
#endif // HWACC_PDSCH_ENABLED
  encoder_hw_factory_config.crc_factory        = crc_calc_factory;
  encoder_hw_factory_config.segmenter_factory  = segmenter_factory;
  encoder_hw_factory_config.hw_encoder_factory = hw_encoder_factory;
  return create_pdsch_encoder_factory_hw(encoder_hw_factory_config);
}

static std::shared_ptr<pdsch_encoder_factory> create_pdsch_encoder_factory()
{
  if (encoder_type == "generic") {
    return create_generic_pdsch_encoder_factory();
  }

  if (encoder_type == "acc100") {
    return create_acc100_pdsch_encoder_factory();
  }

  return nullptr;
}

int main(int argc, char** argv)
{
#ifdef HWACC_PDSCH_ENABLED
  // Separate EAL and non-EAL arguments.
  eal_arguments = capture_eal_args(&argc, &argv);
#endif // HWACC_PDSCH_ENABLED

  // Parse generic arguments.
  parse_args(argc, argv);

  std::shared_ptr<pdsch_encoder_factory> pdsch_enc_factory = create_pdsch_encoder_factory();
  TESTASSERT(pdsch_enc_factory, "Failed to create PDSCH encoder factory of type {}.", encoder_type);

  std::unique_ptr<pdsch_encoder> pdsch_encoder = pdsch_enc_factory->create();
  TESTASSERT(pdsch_encoder);

  for (const test_case_t& test_case : pdsch_encoder_test_data) {
    // Load the TB.
    std::vector<uint8_t> transport_block = test_case.transport_block.read();

    // Load the golden data.
    std::vector<uint8_t> expected_codeword = test_case.encoded.read();

    // Assert codeword length.
    constexpr unsigned MAX_CW_LENGTH = 40000;
    unsigned           cw_length =
        test_case.config.nof_ch_symbols * static_cast<unsigned>(test_case.config.mod) * test_case.config.nof_layers;
    TESTASSERT_EQ(cw_length, expected_codeword.size(), "Wrong codeword length.");
    static_vector<uint8_t, MAX_CW_LENGTH> codeword(cw_length);

    pdsch_encoder::configuration config;
    config.base_graph     = test_case.config.base_graph;
    config.rv             = test_case.config.rv;
    config.mod            = test_case.config.mod;
    config.Nref           = test_case.config.Nref;
    config.nof_layers     = test_case.config.nof_layers;
    config.nof_ch_symbols = test_case.config.nof_ch_symbols;

    // Encode the TB.
    pdsch_encoder->encode(codeword, transport_block, config);

    // Assert encoded data.
    TESTASSERT_EQ(span<const uint8_t>(codeword), span<const uint8_t>(expected_codeword));

    // Repeat test reusing the buffer.
    pdsch_encoder->encode(codeword, transport_block, config);
    TESTASSERT_EQ(span<const uint8_t>(codeword), span<const uint8_t>(expected_codeword));
  }
}
