/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/srsvec/add.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;

static unsigned              nof_prb_ul_grid       = 52;
static unsigned              dft_size_15kHz        = 1024;
static unsigned              root_sequence_index   = 0;
static unsigned              frequency_offset      = 0;
static preamble_format       format                = preamble_format::FORMAT0;
static restricted_set_config restricted_set        = restricted_set_config::UNRESTRICTED;
static unsigned              zero_correlation_zone = 0;
static subcarrier_spacing    pusch_scs             = subcarrier_spacing::kHz15;
static unsigned              preamble_index        = 0;
static unsigned              sequence_delay_n      = 123;

static void test_case()
{
  srsvec::aligned_vec<cf_t> buffer(4 * 15 * dft_size_15kHz);

  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_generic();
  TESTASSERT(dft_factory);

  std::shared_ptr<prach_generator_factory> prach_gen_factory =
      create_prach_generator_factory_sw(dft_factory, nof_prb_ul_grid, dft_size_15kHz);
  TESTASSERT(prach_gen_factory);

  std::shared_ptr<prach_detector_factory> prach_detector_factory =
      create_prach_detector_factory_simple(dft_factory, prach_gen_factory, dft_size_15kHz);
  TESTASSERT(prach_detector_factory);

  std::unique_ptr<prach_generator> generator = prach_gen_factory->create();
  std::unique_ptr<prach_detector>  detector  = prach_detector_factory->create();

  // Generate sequence.
  prach_generator::configuration preamble_config;
  preamble_config.root_sequence_index   = root_sequence_index;
  preamble_config.preamble_index        = preamble_index;
  preamble_config.restricted_set        = restricted_set;
  preamble_config.zero_correlation_zone = zero_correlation_zone;
  preamble_config.rb_offset             = frequency_offset;
  preamble_config.pusch_scs             = pusch_scs;
  preamble_config.frequency_domain      = false;
  span<const cf_t> sequence             = generator->generate(preamble_config);
  srsvec::add(
      buffer.subspan(sequence_delay_n, sequence.size()), sequence, buffer.subspan(sequence_delay_n, sequence.size()));

  // Generate another sequence.
  preamble_config.preamble_index = (preamble_index + 1) % 64;
  sequence                       = generator->generate(preamble_config);
  srsvec::add(
      buffer.subspan(sequence_delay_n, sequence.size()), sequence, buffer.subspan(sequence_delay_n, sequence.size()));

  prach_detector::slot_configuration detector_config;
  detector_config.root_sequence_index   = root_sequence_index;
  detector_config.frequency_offset      = frequency_offset;
  detector_config.format                = format;
  detector_config.restricted_set        = restricted_set;
  detector_config.zero_correlation_zone = zero_correlation_zone;
  detector_config.pusch_scs             = pusch_scs;
  detector_config.start_preamble_index  = 0;
  detector_config.nof_preamble_indices  = 64;

  prach_detector::detection_result result = detector->detect(buffer, detector_config);

  fmt::print("RSSI: {} dB\n", result.rssi_dB);
  for (const prach_detector::preamble_indication& preamble : result.preambles) {
    fmt::print("--    Preamble: {}\n", preamble.preamble_index);
    fmt::print("  Time advance: {} us ({} samples)\n",
               preamble.time_advance_us,
               preamble.time_advance_us * static_cast<float>(dft_size_15kHz) * 15e3F / 1e6F);
    fmt::print("         Power: {} dB\n", preamble.power_dB);
    fmt::print("           SNR: {} dB\n", preamble.snr_dB);
  }
}

int main()
{
  test_case();
}