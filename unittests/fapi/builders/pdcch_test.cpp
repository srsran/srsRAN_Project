#include "srsgnb/fapi/message_builders.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;
using namespace fapi;

static void test_bwp_params()
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);

  unsigned           bwp_size  = 100;
  unsigned           bwp_start = 100;
  subcarrier_spacing scs       = subcarrier_spacing::kHz60;
  cyclic_prefix_type cyclic_p  = cyclic_prefix_type::normal;

  builder.set_bwp_parameters(bwp_size, bwp_start, scs, cyclic_p);

  TESTASSERT_EQ(bwp_size, pdu.coreset_bwp_size);
  TESTASSERT_EQ(bwp_start, pdu.coreset_bwp_start);
  TESTASSERT_EQ(scs, pdu.scs);
  TESTASSERT_EQ(cyclic_p, pdu.cyclic_prefix);
}

static void test_coreset_params()
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);

  unsigned                  start_symb_id    = 7;
  unsigned                  symb_duration    = 2;
  unsigned                  reg_size         = 3;
  unsigned                  interleaver_size = 3;
  unsigned                  shift_index      = 100;
  cce_to_reg_mapping_type   mapping_type     = cce_to_reg_mapping_type::interleaved;
  pdcch_coreset_type        coreset_type     = pdcch_coreset_type::pbch_or_sib1;
  precoder_granularity_type granularity      = precoder_granularity_type::same_as_reg_bundle;
  std::array<uint8_t, 6>    freq_domain      = {1, 1, 1, 0, 0, 0};

  builder.set_coreset_parameters(start_symb_id,
                                 symb_duration,
                                 freq_domain,
                                 mapping_type,
                                 reg_size,
                                 interleaver_size,
                                 coreset_type,
                                 shift_index,
                                 granularity);

  TESTASSERT_EQ(start_symb_id, pdu.start_symbol_index);
  TESTASSERT_EQ(symb_duration, pdu.duration_symbols);
  TESTASSERT(freq_domain == pdu.freq_domain_resource);
  TESTASSERT_EQ(mapping_type, pdu.cce_reg_mapping_type);
  TESTASSERT_EQ(reg_size, pdu.reg_bundle_size);
  TESTASSERT_EQ(interleaver_size, pdu.interleaver_size);
  TESTASSERT_EQ(coreset_type, pdu.coreset_type);
  TESTASSERT_EQ(shift_index, pdu.shift_index);
  TESTASSERT_EQ(granularity, pdu.precoder_granularity);
}

static void test_add_dl_dci()
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);

  TESTASSERT(pdu.dl_dci.empty());
  TESTASSERT(pdu.maintenance_v3.info.empty());
  TESTASSERT(pdu.parameters_v4.params.empty());

  builder.add_dl_dci();
  TESTASSERT_EQ(1, pdu.dl_dci.size());
  TESTASSERT_EQ(1, pdu.maintenance_v3.info.size());
  TESTASSERT_EQ(1, pdu.parameters_v4.params.size());
  TESTASSERT_EQ(0, pdu.maintenance_v3.info[0].dci_index);
}

static void test_dci_basic_params()
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

  TESTASSERT_EQ(rnti, pdu.dl_dci[0].rnti);
  TESTASSERT_EQ(nid_pdcch_data, pdu.dl_dci[0].nid_pdcch_data);
  TESTASSERT_EQ(nrnti_pdcch_data, pdu.dl_dci[0].nrnti_pdcch_data);
  TESTASSERT_EQ(cce_index, pdu.dl_dci[0].cce_index);
  TESTASSERT_EQ(aggregation_level, pdu.dl_dci[0].aggregation_level);
}

static void test_dci_tx_power_params()
{
  for (int i = -8; i != 9; ++i) {
    dl_pdcch_pdu         pdu;
    dl_pdcch_pdu_builder builder(pdu);
    dl_dci_pdu_builder   builder_dci = builder.add_dl_dci();

    // Leave the optional power unset for -8 to test it is being set internally to -127.
    optional<float> power;
    if (i != -8) {
      power = i;
    }

    builder_dci.set_tx_power_info_parameter(power);

    TESTASSERT_EQ(power ? power.value() : -127, pdu.dl_dci[0].power_control_offset_ss_profile_nr);
  }
}

static void test_maintenance_v3_dci_params()
{
  bool collocated = true;

  for (int dmrs_power = -4; dmrs_power != 4; ++dmrs_power) {
    for (int data_power = -4; data_power != 4; ++data_power) {
      dl_pdcch_pdu         pdu;
      dl_pdcch_pdu_builder builder(pdu);
      dl_dci_pdu_builder   builder_dci = builder.add_dl_dci();

      // Leave the optional power unset for 0 to test it is being set internally to -32768.
      optional<float> dmrs;
      if (dmrs_power != 0) {
        dmrs = dmrs_power;
      }

      // Leave the optional power unset for 0 to test it is being set internally to -32768.
      optional<float> data;
      if (data_power != 0) {
        data = data_power;
      }

      builder_dci.set_maintenance_v3_dci_parameters(collocated, dmrs, data);

      TESTASSERT_EQ(collocated, pdu.maintenance_v3.info[0].collocated_AL16_candidate);
      TESTASSERT_EQ(dmrs ? dmrs.value() * 1000 : -32768,
                    pdu.maintenance_v3.info[0].pdcch_dmrs_power_offset_profile_sss);
      TESTASSERT_EQ(data ? data.value() * 1000 : -32768,
                    pdu.maintenance_v3.info[0].pdcch_data_power_offset_profile_sss);
    }
  }
}

static void test_parameters_v4_dci()
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);
  dl_dci_pdu_builder   builder_dci = builder.add_dl_dci();

  unsigned nid = 100;
  builder_dci.set_parameters_v4_dci(nid);

  TESTASSERT_EQ(nid, pdu.parameters_v4.params[0].nid_pdcch_dmrs);
}

static void test_dl_dci_params()
{
  test_dci_basic_params();
  fmt::print("[PDCCH Builder] - DCI Basic params tested -> OK\n");
  test_dci_tx_power_params();
  fmt::print("[PDCCH Builder] - DCI Tx power params tested -> OK\n");
  // :TODO: add test for the payload.
  test_maintenance_v3_dci_params();
  fmt::print("[PDCCH Builder] - Maintenance v3 DCI params tested -> OK\n");
  test_parameters_v4_dci();
  fmt::print("[PDCCH Builder] - Parameters v4 DCI tested -> OK\n");
}

static void test_pdcch_builder()
{
  test_bwp_params();
  fmt::print("[PDCCH Builder] - BWP params tested -> OK\n");
  test_coreset_params();
  fmt::print("[PDCCH Builder] - Coreset params tested -> OK\n");
  test_add_dl_dci();
  fmt::print("[PDCCH Builder] - Add DL DCI PDU -> OK\n");
  test_dl_dci_params();
  fmt::print("[PDCCH Builder] - DL DCI params tested -> OK\n");
}

int main()
{
  test_pdcch_builder();

  return 0;
}
