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

#include "srsran/fapi/message_builders.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi;

TEST(dl_pdcch_pdu_builder, valid_bwp_parameters_passes)
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);

  unsigned           bwp_size  = 100;
  unsigned           bwp_start = 100;
  subcarrier_spacing scs       = subcarrier_spacing::kHz60;
  cyclic_prefix      cp        = cyclic_prefix::NORMAL;

  builder.set_bwp_parameters(bwp_size, bwp_start, scs, cp);

  ASSERT_EQ(bwp_size, pdu.coreset_bwp_size);
  ASSERT_EQ(bwp_start, pdu.coreset_bwp_start);
  ASSERT_EQ(scs, pdu.scs);
  ASSERT_EQ(cp, pdu.cp);
}

TEST(dl_pdcch_pdu_builder, valid_coreset_parameters_passes)
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);

  unsigned                                         start_symb_id    = 7;
  unsigned                                         symb_duration    = 2;
  unsigned                                         reg_size         = 3;
  unsigned                                         interleaver_size = 3;
  unsigned                                         shift_index      = 100;
  cce_to_reg_mapping_type                          mapping_type     = cce_to_reg_mapping_type::interleaved;
  pdcch_coreset_type                               coreset_type     = pdcch_coreset_type::pbch_or_coreset0;
  coreset_configuration::precoder_granularity_type granularity =
      coreset_configuration::precoder_granularity_type::same_as_reg_bundle;

  freq_resource_bitmap freq_domain = {1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1,
                                      1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1};

  builder.set_coreset_parameters(start_symb_id,
                                 symb_duration,
                                 freq_domain,
                                 mapping_type,
                                 reg_size,
                                 interleaver_size,
                                 coreset_type,
                                 shift_index,
                                 granularity);

  ASSERT_EQ(start_symb_id, pdu.start_symbol_index);
  ASSERT_EQ(symb_duration, pdu.duration_symbols);
  ASSERT_EQ(mapping_type, pdu.cce_reg_mapping_type);
  ASSERT_EQ(reg_size, pdu.reg_bundle_size);
  ASSERT_EQ(interleaver_size, pdu.interleaver_size);
  ASSERT_EQ(coreset_type, pdu.coreset_type);
  ASSERT_EQ(shift_index, pdu.shift_index);
  ASSERT_EQ(granularity, pdu.precoder_granularity);
  ASSERT_EQ(freq_domain, pdu.freq_domain_resource);
}

TEST(dl_pdcch_pdu_builder, add_dl_dci)
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);

  ASSERT_TRUE(pdu.parameters_v4.params.empty());

  builder.add_dl_dci();
  ASSERT_EQ(1, pdu.dl_dci.size());
  ASSERT_EQ(1, pdu.maintenance_v3.info.size());
  ASSERT_EQ(1, pdu.parameters_v4.params.size());
  ASSERT_EQ(0, pdu.maintenance_v3.info[0].dci_index);
}

TEST(dl_pdcch_pdu_builder, valid_dci_basic_parameters_passes)
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);
  dl_dci_pdu_builder   builder_dci = builder.add_dl_dci();

  rnti_t   rnti              = to_rnti(100);
  unsigned nid_pdcch_data    = 200;
  unsigned nrnti_pdcch_data  = 200;
  unsigned cce_index         = 100;
  unsigned aggregation_level = 4;
  builder_dci.set_basic_parameters(rnti, nid_pdcch_data, nrnti_pdcch_data, cce_index, aggregation_level);

  ASSERT_EQ(rnti, pdu.dl_dci[0].rnti);
  ASSERT_EQ(nid_pdcch_data, pdu.dl_dci[0].nid_pdcch_data);
  ASSERT_EQ(nrnti_pdcch_data, pdu.dl_dci[0].nrnti_pdcch_data);
  ASSERT_EQ(cce_index, pdu.dl_dci[0].cce_index);
  ASSERT_EQ(aggregation_level, pdu.dl_dci[0].aggregation_level);
}

TEST(dl_pdcch_pdu_builder, valid_dci_tx_power_parameters_passes)
{
  for (int i = -8; i != 9; ++i) {
    dl_pdcch_pdu         pdu;
    dl_pdcch_pdu_builder builder(pdu);
    dl_dci_pdu_builder   builder_dci = builder.add_dl_dci();

    builder_dci.set_tx_power_info_parameter(i);

    ASSERT_EQ(i, pdu.dl_dci[0].power_control_offset_ss_profile_nr);
  }
}

TEST(dl_pdcch_pdu_builder, valid_dci_v4_parameters_passes)
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);
  dl_dci_pdu_builder   builder_dci = builder.add_dl_dci();

  unsigned nid = 100;
  builder_dci.set_parameters_v4_dci(nid);

  ASSERT_EQ(nid, pdu.parameters_v4.params[0].nid_pdcch_dmrs);
}

TEST(dl_pdcch_pdu_builder, add_valid_dci_payload_passes)
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);
  dl_dci_pdu_builder   builder_dci = builder.add_dl_dci();

  dci_payload payload = {1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1};
  builder_dci.set_payload(payload);

  ASSERT_EQ(payload, pdu.dl_dci.back().payload);
}
