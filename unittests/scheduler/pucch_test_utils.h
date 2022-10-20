/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "config_generators.h"
#include "lib/du_manager/converters/mac_cell_configuration_helpers.h"
#include "lib/mac/mac_ctrl/sched_config_helpers.h"
#include "lib/scheduler/pucch_scheduling/pucch_scheduler_impl.h"
#include "scheduler_test_suite.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include <gtest/gtest.h>

namespace srsgnb {

////////////    Builder of PUCCH scheduler output     ////////////

// Class that builds the PUCCH scheduler output for test.
class pucch_output_test_builder
{
public:
  pucch_output_test_builder(const bwp_configuration* bwp_cfg,
                            unsigned                 pci_,
                            pucch_format             format,
                            prb_interval             prbs,
                            prb_interval             second_hop_prbs,
                            ofdm_symbol_range        symbols,
                            bool                     intra_slot_freq_hop,
                            uint8_t                  initial_cyclic_shift,
                            sr_nof_bits              sr_bits,
                            unsigned                 harq_ack_nof_bits,
                            uint8_t                  time_domain_occ) :
    pci(pci_)
  {
    pucch_test.crnti   = to_rnti(0x4601);
    pucch_test.bwp_cfg = bwp_cfg;

    if (format == pucch_format::FORMAT_0) {
      build_format_0(
          prbs, second_hop_prbs, symbols, intra_slot_freq_hop, initial_cyclic_shift, sr_bits, harq_ack_nof_bits);
    } else {
      build_format_1(prbs,
                     second_hop_prbs,
                     symbols,
                     intra_slot_freq_hop,
                     initial_cyclic_shift,
                     sr_bits,
                     harq_ack_nof_bits,
                     time_domain_occ);
    }
  }

  const pucch_info& get_expected_pucch_output() const { return pucch_test; };

private:
  void build_format_0(prb_interval      prbs,
                      prb_interval      second_hop_prbs,
                      ofdm_symbol_range symbols,
                      bool              intra_slot_freq_hop,
                      uint8_t           initial_cyclic_shift,
                      sr_nof_bits       sr_bits,
                      unsigned          harq_ack_nof_bits);

  void build_format_1(prb_interval      prbs,
                      prb_interval      second_hop_prbs,
                      ofdm_symbol_range symbols,
                      bool              intra_slot_freq_hop,
                      uint8_t           initial_cyclic_shift,
                      sr_nof_bits       sr_bits,
                      unsigned          harq_ack_nof_bits,
                      uint8_t           time_domain_occ);

  pucch_info pucch_test;
  unsigned   pci;
};

void pucch_output_test_builder::build_format_0(prb_interval      prbs,
                                               prb_interval      second_hop_prbs,
                                               ofdm_symbol_range symbols,
                                               bool              intra_slot_freq_hop,
                                               uint8_t           initial_cyclic_shift,
                                               sr_nof_bits       sr_bits,
                                               unsigned          harq_ack_nof_bits)
{
  pucch_test.resources.prbs                = prbs;
  pucch_test.resources.second_hop_prbs     = second_hop_prbs;
  pucch_test.resources.symbols             = symbols;
  pucch_test.resources.intra_slot_freq_hop = intra_slot_freq_hop;

  pucch_test.format_0.initial_cyclic_shift = initial_cyclic_shift;
  pucch_test.format_0.sr_bits              = sr_bits;
  pucch_test.format_0.harq_ack_nof_bits    = harq_ack_nof_bits;

  pucch_test.format_0.group_hopping = pucch_group_hopping::NEITHER;
  pucch_test.format                 = pucch_format::FORMAT_0;
  pucch_test.format_0.n_id_hopping  = pci;
}

void pucch_output_test_builder::build_format_1(prb_interval      prbs,
                                               prb_interval      second_hop_prbs,
                                               ofdm_symbol_range symbols,
                                               bool              intra_slot_freq_hop,
                                               uint8_t           initial_cyclic_shift,
                                               sr_nof_bits       sr_bits,
                                               unsigned          harq_ack_nof_bits,
                                               uint8_t           time_domain_occ)
{
  pucch_test.resources.prbs                = prbs;
  pucch_test.resources.second_hop_prbs     = second_hop_prbs;
  pucch_test.resources.symbols             = symbols;
  pucch_test.resources.intra_slot_freq_hop = intra_slot_freq_hop;

  pucch_test.format_1.initial_cyclic_shift = initial_cyclic_shift;
  pucch_test.format_1.sr_bits              = sr_bits;
  pucch_test.format_1.harq_ack_nof_bits    = harq_ack_nof_bits;
  pucch_test.format_1.time_domain_occ      = time_domain_occ;

  pucch_test.format_1.group_hopping   = pucch_group_hopping::NEITHER;
  pucch_test.format                   = pucch_format::FORMAT_1;
  pucch_test.format_1.n_id_hopping    = pci;
  pucch_test.format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
}

////////////      Utilities      ////////

// Verify if the PUCCH scheduler output (or PUCCH PDU) is correct.
bool assess_ul_pucch_info(const pucch_info& expected, const pucch_info& test)
{
  bool is_equal = expected.crnti == test.crnti && *expected.bwp_cfg == *test.bwp_cfg && expected.format == test.format;
  is_equal      = is_equal && expected.resources.prbs == test.resources.prbs &&
             expected.resources.symbols == test.resources.symbols &&
             expected.resources.intra_slot_freq_hop == test.resources.intra_slot_freq_hop;
  if (expected.resources.intra_slot_freq_hop == true) {
    is_equal = is_equal && expected.resources.second_hop_prbs == test.resources.second_hop_prbs;
  }
  switch (expected.format) {
    case pucch_format::FORMAT_0: {
      const pucch_format_0& expected_f = expected.format_0;
      const pucch_format_0& test_f     = test.format_0;
      is_equal                         = is_equal && expected_f.group_hopping == test_f.group_hopping &&
                 expected_f.n_id_hopping == test_f.n_id_hopping &&
                 expected_f.initial_cyclic_shift == test_f.initial_cyclic_shift &&
                 expected_f.sr_bits == test_f.sr_bits && expected_f.harq_ack_nof_bits == test_f.harq_ack_nof_bits;

      break;
    }
    case pucch_format::FORMAT_1: {
      const pucch_format_1& expected_f = expected.format_1;
      const pucch_format_1& test_f     = test.format_1;
      is_equal                         = is_equal && expected_f.group_hopping == test_f.group_hopping &&
                 expected_f.n_id_hopping == test_f.n_id_hopping &&
                 expected_f.initial_cyclic_shift == test_f.initial_cyclic_shift &&
                 expected_f.sr_bits == test_f.sr_bits && expected_f.harq_ack_nof_bits == test_f.harq_ack_nof_bits &&
                 expected_f.slot_repetition == test_f.slot_repetition &&
                 expected_f.time_domain_occ == test_f.time_domain_occ;
      break;
    }
    default: {
      return false;
    };
  }

  return is_equal;
}

// Builds a default DCI with a given n_CCEs.
inline pdcch_dl_information make_default_dci(unsigned n_cces, const coreset_configuration* coreset_cfg)
{
  pdcch_dl_information dci;
  dci.ctx.cces.ncce   = n_cces;
  dci.ctx.coreset_cfg = coreset_cfg;
  return dci;
}

// Makes a \c sched_cell_configuration_request_message object with a given \c pucch_res_common parameter.
inline sched_cell_configuration_request_message make_custom_sched_cell_configuration_request(unsigned pucch_res_common)
{
  sched_cell_configuration_request_message cell_req = make_default_sched_cell_configuration_request();
  cell_req.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common = pucch_res_common;
  return cell_req;
}

/////////        TEST BENCH for PUCCH scheduler        /////////

// Test bench with all that is needed for the PUCCH.
class test_bench
{
public:
  test_bench(unsigned pucch_res_common = 11, unsigned n_cces = 0) :
    cell_req{make_custom_sched_cell_configuration_request(pucch_res_common)},
    cell_cfg{cell_req},
    user{cell_cfg, make_scheduler_ue_creation_request(test_helpers::make_default_ue_creation_request())},
    ue_cell_cfg{cell_cfg, serving_cell_config{}},
    coreset_cfg{config_helpers::make_default_coreset_config()},
    dci_info{make_default_dci(n_cces, &coreset_cfg)},
    k0{0},
    k1{4},
    pucch_sched{cell_cfg, config_helpers::make_default_ue_uplink_config(), ues},
    sl_tx{to_numerology_value(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs), 0}
  {
    res_grid.slot_indication(sl_tx);
    sched_ue_creation_request_message ue_req =
        make_scheduler_ue_creation_request(test_helpers::make_default_ue_creation_request());
    ues.insert(du_ue_index_t::MIN_DU_UE_INDEX, std::make_unique<ue>(cell_cfg, ue_req));
  }

  pdcch_dl_information make_default_dci(unsigned n_cces, const coreset_configuration* coreset_cfg_)
  {
    pdcch_dl_information dci;
    dci.ctx.cces.ncce   = n_cces;
    dci.ctx.coreset_cfg = coreset_cfg_;
    return dci;
  }

  void slot_indication()
  {
    sl_tx++;
    mac_logger.set_context(sl_tx.to_uint());
    test_logger.set_context(sl_tx.to_uint());
    res_grid.slot_indication(sl_tx);
  }

  sched_cell_configuration_request_message make_custom_sched_cell_configuration_request(unsigned pucch_res_common)
  {
    sched_cell_configuration_request_message req = make_default_sched_cell_configuration_request();
    cell_req.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common = pucch_res_common;
    return req;
  }

  // Variables that are needed to generate the PUCCH scheduler.
  sched_cell_configuration_request_message cell_req;
  cell_configuration                       cell_cfg;
  // Variables that are needed for calling the PUCCH allocator.
  cell_resource_allocator res_grid{cell_cfg};
  ue                      user;
  ue_cell_configuration   ue_cell_cfg;
  coreset_configuration   coreset_cfg;
  pdcch_dl_information    dci_info;
  unsigned                k0;
  unsigned                k1;
  ue_list                 ues;
  // Variables that are needed for the PUCCH scheduler.
  pucch_scheduler_impl  pucch_sched;
  slot_point            sl_tx;
  srslog::basic_logger& mac_logger  = srslog::fetch_basic_logger("MAC");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

} // namespace srsgnb
