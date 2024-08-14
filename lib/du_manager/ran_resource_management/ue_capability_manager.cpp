/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_capability_manager.h"
#include "srsran/asn1/rrc_nr/ul_dcch_msg_ies.h"

using namespace srsran;
using namespace srs_du;

expected<ue_capability_summary, std::string>
srsran::srs_du::decode_ue_nr_cap_container(const byte_buffer& ue_cap_container)
{
  asn1::rrc_nr::ue_nr_cap_s ue_cap;
  {
    asn1::cbit_ref bref{ue_cap_container};
    if (ue_cap.unpack(bref) != asn1::SRSASN_SUCCESS) {
      return make_unexpected(std::string("Couldn't unpack UE NR Capability RRC container"));
    }
  }

  ue_capability_summary ue_caps;

  // fill UE capability summary.
  if (ue_cap.phy_params.phy_params_fr1_present) {
    ue_caps.pdsch_qam256_supported = ue_cap.phy_params.phy_params_fr1.pdsch_256_qam_fr1_present;
  }
  for (const auto& band : ue_cap.rf_params.supported_band_list_nr) {
    // TODO: save per-band capabilities.
    ue_caps.pusch_qam256_supported |= band.pusch_256_qam_present;
  }

  return ue_caps;
}

ue_capability_manager::ue_capability_manager(const scheduler_expert_config& scheduler_cfg_,
                                             srslog::basic_logger&          logger_) :
  scheduler_cfg(scheduler_cfg_), logger(logger_)
{
  (void)scheduler_cfg;
}

void ue_capability_manager::update(du_ue_resource_config& ue_res_cfg, const byte_buffer& ue_cap_rat_list)
{
  if (not decode_ue_capability_list(ue_cap_rat_list)) {
    // No updates.
    return;
  }
  if (not ue_caps.has_value()) {
    // No UE capabilities decoded.
    return;
  }

  serving_cell_config& pcell_cfg = ue_res_cfg.cell_group.cells[0].serv_cell_cfg;

  // Enable 256QAM for PDSCH, if supported.
  if (ue_caps->pdsch_qam256_supported) {
    // Set MCS index table 2 for PDSCH. See TS 38.214, Table 5.1.3.1-2.
    if (pcell_cfg.init_dl_bwp.pdsch_cfg.has_value()) {
      pcell_cfg.init_dl_bwp.pdsch_cfg->mcs_table = pdsch_mcs_table::qam256;
    }

    if (pcell_cfg.csi_meas_cfg.has_value()) {
      for (auto& csi_report_cfg : pcell_cfg.csi_meas_cfg.value().csi_report_cfg_list) {
        if (ue_caps->pdsch_qam256_supported) {
          // Enable CQI table 2. See TS 38.214, Table 5.2.2.1-3.
          csi_report_cfg.cqi_table = cqi_table_t::table2;
        }
      }
    }
  }

  // Enable 256QAM for PUSCH, if supported.
  if (ue_caps->pusch_qam256_supported) {
    if (pcell_cfg.ul_config.has_value() and pcell_cfg.ul_config->init_ul_bwp.pusch_cfg.has_value()) {
      pcell_cfg.ul_config->init_ul_bwp.pusch_cfg->mcs_table = pusch_mcs_table::qam256;
    }
  }
}

bool ue_capability_manager::decode_ue_capability_list(const byte_buffer& ue_cap_rat_list)
{
  using namespace asn1::rrc_nr;

  ue_cap_rat_container_list_l asn1_cap_list;
  {
    asn1::cbit_ref bref{ue_cap_rat_list};
    if (asn1::unpack_dyn_seq_of(asn1_cap_list, bref, 0, 8) != asn1::SRSASN_SUCCESS) {
      logger.error("Couldn't unpack UE Capability RAT Container List RRC container");
      return false;
    }
  }

  for (const ue_cap_rat_container_s& ue_cap_rat : asn1_cap_list) {
    if (ue_cap_rat.rat_type.value != rat_type_opts::nr) {
      logger.warning("Unsupported RAT type in UE Capability RAT Container List RRC container");
      continue;
    }
    expected<ue_capability_summary, std::string> asn1_cap = decode_ue_nr_cap_container(ue_cap_rat.ue_cap_rat_container);
    if (not asn1_cap.has_value()) {
      logger.warning("{}", asn1_cap.error());
      continue;
    }
    ue_caps = asn1_cap.value();
    return true;
  }

  return false;
}
