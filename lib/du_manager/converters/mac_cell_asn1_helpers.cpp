
#include "mac_cell_asn1_helpers.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/asn1/rrc_nr/serving_cell.h"

using namespace srsgnb;

namespace {

void fill_ssb_configuration_from_asn1(ssb_configuration&                                 out,
                                      const asn1::rrc_nr::mib_s&                         mib_in,
                                      const asn1::rrc_nr::serving_cell_cfg_common_sib_s& sib_in,
                                      subcarrier_spacing                                 ssb_scs)
{
  // Parameters taken from ServingCellConfigCommonSIB
  out.ssb_power = sib_in.ss_pbch_block_pwr;
  out.ssb_offset_to_point_A = sib_in.dl_cfg_common.freq_info_dl.offset_to_point_a;
  out.ssb_period            = (uint8_t)sib_in.ssb_periodicity_serving_cell;
  // SSB subcarrier is taken from MIB.ssb-SubcarrierOffset.
  out.ssb_subcarrier_offset = mib_in.ssb_subcarrier_offset;

  // The SSB subcarrierSpacing should be passed as input parameter.
  out.scs = ssb_scs;

  // Hard-coded for the time being. Should be taken either from
  // - ServingCellConfigCommonSIB.ssb-PositionsInBurst, or
  // - ServingCellConfigCommon.ssb-PositionsInBurst.
  // TODO: derive bitmap from correct fields.
  out.ssb_bitmap.set(0);

  // TODO: define which Beam ID should be used
  out.beam_ids[0] = 0;

  // This is not imported from RRC
  // TODO: define where to get this value
  out.beta_pss = 0;
}

void fill_tdd_cfg_from_asn1(const asn1::rrc_nr::tdd_ul_dl_cfg_common_s& in, tdd_configuration& out)
{
  // TODO
}

} // namespace

void srsgnb::fill_mac_cell_cfg_from_asn1(mac_cell_configuration&                            out,
                                         const asn1::rrc_nr::serving_cell_cfg_common_sib_s& serv_cell_sib_in,
                                         const asn1::rrc_nr::mib_s&                         mib_in,
                                         subcarrier_spacing                                 ssb_scs)
{
  // Carrier Configuration.
  // TODO

  // TDD Configuration.
  if (serv_cell_sib_in.tdd_ul_dl_cfg_common_present) {
    out.tdd_cfg.emplace();
    fill_tdd_cfg_from_asn1(serv_cell_sib_in.tdd_ul_dl_cfg_common, out.tdd_cfg.value());
  }

  fill_ssb_configuration_from_asn1(out.ssb_cfg, mib_in, serv_cell_sib_in, ssb_scs);
}
