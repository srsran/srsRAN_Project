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

static unsigned generate_case_pattern()
{
  std::uniform_int_distribution<unsigned> dist(0, 4);

  return dist(gen);
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
  pdu.ssb_case                        = static_cast<ssb_pattern_case>(generate_case_pattern());
  pdu.L_max                           = 4;
  pdu.scs                             = subcarrier_spacing::kHz240;
  pdu.mib_data.dmrs_typeA_position    = generate_binary();
  pdu.mib_data.pdcch_config_sib1      = generate_byte();
  pdu.mib_data.cell_barred            = generate_bool();
  pdu.mib_data.intra_freq_reselection = generate_bool();

  return pdu;
}

static dci_payload generate_dci_payload()
{
  dci_payload payload;
  for (unsigned i = 0, e = payload.max_size(); i != e; ++i) {
    payload.push_back(generate_bool());
  }

  return payload;
}

static crb_interval generate_crb_interval()
{
  unsigned init = generate_block_index();
  return {init, init + generate_subcarrier_offset()};
}

static bwp_configuration generate_bwp_configuration()
{
  bwp_configuration config;

  config.cp_extended = generate_bool();
  config.scs         = subcarrier_spacing::kHz240;
  config.crbs        = generate_crb_interval();

  return config;
}

static coreset_id generate_coreset_id()
{
  std::uniform_int_distribution<unsigned> dist(0, 11);
  return static_cast<coreset_id>(dist(gen));
}

static unsigned generate_duration()
{
  std::uniform_int_distribution<unsigned> dist(1, 3);
  return dist(gen);
}

static unsigned generate_reg_bundle_sz()
{
  static const std::vector<unsigned>      values = {2, 3, 6};
  std::uniform_int_distribution<unsigned> dist(0, 2);
  return values[dist(gen)];
}

static coreset_configuration generate_coreset_configuration()
{
  coreset_configuration config;

  config.id                   = generate_coreset_id();
  config.duration             = generate_duration();
  config.precoder_granurality = static_cast<coreset_configuration::precoder_granularity_type>(generate_binary());
  config.coreset0_rb_start    = 0;
  if (generate_bool()) {
    config.pdcch_dmrs_scrambling_id.emplace(10);
    config.interleaved.emplace(
        coreset_configuration::interleaved_mapping_type{generate_reg_bundle_sz(), generate_reg_bundle_sz(), {15}});
  }

  for (unsigned i = 0, e = config.freq_domain_resources.max_size(); i != e; ++i) {
    config.freq_domain_resources.push_back(generate_bool());
  }

  return config;
}

static dci_1_0_si_rnti_configuration generate_si_f1_0()
{
  // :TODO: improve this in the future.
  return {1, 2, 3, 1, 0, 2, 1};
}

static dci_1_0_ra_rnti_configuration generate_ra_f1_0()
{
  // :TODO: improve this in the future.
  return {1, 2, 3, 0, 1, 2};
}

static dci_dl_info generate_dci_dl_info()
{
  dci_dl_info info;
  info.type = static_cast<dci_dl_rnti_config_type>(generate_binary());
  if (info.type == dci_dl_rnti_config_type::si_f1_0) {
    info.si_f1_0 = generate_si_f1_0();
  } else {
    info.ra_f1_0 = generate_ra_f1_0();
  }

  return info;
}

static rnti_t generate_rnti()
{
  std::uniform_int_distribution<unsigned> dist(1, 65535);

  return to_rnti(dist(gen));
}

static dci_dl_context_information generate_dci_context(const bwp_configuration&     bwp,
                                                       const coreset_configuration& coreset)
{
  dci_dl_context_information info;
  info.bwp_cfg     = &bwp;
  info.coreset_cfg = &coreset;

  info.rnti              = generate_rnti();
  info.n_rnti_pdcch_data = 2;
  info.n_rnti_pdcch_data = 4;
  info.cces.ncce         = 2;
  info.cces.aggr_lvl     = static_cast<aggregation_level>(generate_case_pattern());

  return info;
}

static void add_pdcch_pdus_to_result(mac_dl_sched_result& result)
{
  static const unsigned                                nof_pdus    = 4;
  static const static_vector<coreset_configuration, 2> coreset_cfg = {generate_coreset_configuration(),
                                                                      generate_coreset_configuration()};
  static const static_vector<bwp_configuration, 2>     bwp_config  = {generate_bwp_configuration(),
                                                                      generate_bwp_configuration()};

  static dl_sched_result result_in_mem;
  if (result_in_mem.dl_pdcchs.empty()) {
    for (unsigned i = 0; i != nof_pdus; ++i) {
      pdcch_dl_information info{generate_dci_context(bwp_config[(i == 1) ? 1U : 0U], coreset_cfg[(i == 1) ? 1U : 0U]),
                                generate_dci_dl_info()};
      result_in_mem.dl_pdcchs.push_back(std::move(info));
    }
  }

  // Add the DCIs payload.
  for (unsigned i = 0; i != nof_pdus; ++i) {
    result.pdcch_pdus.push_back(generate_dci_payload());
  }

  result.dl_res = &result_in_mem;
}

mac_dl_sched_result unittests::build_valid_mac_dl_sched_result()
{
  mac_dl_sched_result result;

  result.slot = slot_point(4, generate_sfn(), generate_slot());

  // Add PDCCH PDUs.
  add_pdcch_pdus_to_result(result);

  // Add SSBs.
  result.ssb_pdu.push_back(build_valid_dl_ssb_pdu());
  result.ssb_pdu.push_back(build_valid_dl_ssb_pdu());

  return result;
}
