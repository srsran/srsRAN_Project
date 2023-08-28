/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_link_adaptation_controller.h"
#include "../support/mcs_calculator.h"

using namespace srsran;

void ue_link_adaptation_controller::handle_dl_ack_info(mac_harq_ack_report_status ack_value,
                                                       sch_mcs_index              used_mcs,
                                                       pdsch_mcs_table            mcs_table)
{
  static constexpr unsigned MAX_CQI = 15;

  if (ack_value == mac_harq_ack_report_status::dtx or not dl_olla.has_value()) {
    return;
  }

  const sch_mcs_index max_mcs = map_cqi_to_mcs(MAX_CQI, mcs_table).value();
  //  const interval<sch_mcs_index, true> mcs_bounds{expert_cfg.dl_mcs.start(),
  //                                                 std::min(expert_cfg.dl_mcs.stop(), max_mcs)};
  const interval<sch_mcs_index, true> mcs_bounds{0, std::min(sch_mcs_index{0}, max_mcs)};

  dl_olla->update(ack_value == mac_harq_ack_report_status::ack, used_mcs, mcs_bounds);
}

void ue_link_adaptation_controller::handle_ul_crc_info(bool crc, sch_mcs_index used_mcs, pusch_mcs_table mcs_table) {}

cqi_value ue_link_adaptation_controller::get_effective_cqi() const
{
  if (not dl_olla.has_value()) {
    // For the case of no DL outer loop link adaptation.
    return ue_ch_st.get_wideband_cqi();
  }

  float cqi = ue_ch_st.get_wideband_cqi().to_uint();
  cqi += dl_olla.value().offset_db();
  return cqi_value{std::min(static_cast<uint8_t>(std::max(cqi, 0.0f)), cqi_value::max())};
}
