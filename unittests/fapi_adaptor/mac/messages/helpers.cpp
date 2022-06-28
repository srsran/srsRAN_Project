/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "helpers.h"
#include <random>

using namespace srsgnb;
using namespace unittests;

static std::mt19937 gen(0);

static unsigned generate_sfn()
{
  std::uniform_int_distribution<unsigned> dist(0, 1023);
  return dist(gen);
}

static unsigned generate_slot()
{
  std::uniform_int_distribution<unsigned> dist(0, 159);

  return dist(gen);
}

static pci_t generate_pci()
{
  std::uniform_int_distribution<unsigned> dist(0, 1007);

  return dist(gen);
}

static unsigned generate_binary()
{
  std::uniform_int_distribution<unsigned> dist(0, 1);

  return dist(gen);
}

static unsigned generate_block_index()
{
  std::uniform_int_distribution<unsigned> dist(0, 63);

  return dist(gen);
}

static unsigned generate_subcarrier_offset()
{
  std::uniform_int_distribution<unsigned> dist(0, 31);

  return dist(gen);
}

static unsigned generate_offset_point_A()
{
  std::uniform_int_distribution<unsigned> dist(0, 2199);

  return dist(gen);
}

static unsigned generate_byte()
{
  std::uniform_int_distribution<unsigned> dist(0, 255);

  return dist(gen);
}

static ssb_pattern_case generate_case_pattern()
{
  std::uniform_int_distribution<unsigned> dist(0, 4);

  return static_cast<ssb_pattern_case>(dist(gen));
}

static bool generate_bool()
{
  std::uniform_int_distribution<unsigned> dist(0, 1);
  return dist(gen);
}

dl_ssb_pdu unittests::build_valid_dl_ssb_pdu()
{
  dl_ssb_pdu pdu;

  pdu.pci                             = generate_pci();
  pdu.beta_pss_profile_nr             = static_cast<srsgnb::ssb_beta_pss>(generate_binary());
  pdu.ssb_index                       = generate_block_index();
  pdu.ssb_subcarrier_offset           = generate_subcarrier_offset();
  pdu.offset_to_point_A               = generate_offset_point_A();
  pdu.ssb_case                        = generate_case_pattern();
  pdu.L_max                           = 4;
  pdu.scs                             = subcarrier_spacing::kHz240;
  pdu.mib_data.dmrs_typeA_position    = generate_binary();
  pdu.mib_data.pdcch_config_sib1      = generate_byte();
  pdu.mib_data.cell_barred            = generate_bool();
  pdu.mib_data.intra_freq_reselection = generate_bool();

  return pdu;
}

mac_dl_sched_result unittests::build_valid_mac_dl_sched_result()
{
  mac_dl_sched_result result;

  result.slot = slot_point(4, generate_sfn(), generate_slot());

  // Add SSBs.
  result.ssb_pdu.push_back(build_valid_dl_ssb_pdu());
  result.ssb_pdu.push_back(build_valid_dl_ssb_pdu());

  return result;
}
