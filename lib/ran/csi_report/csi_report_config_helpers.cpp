/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/ran/csi_report/csi_report_config_helpers.h"

using namespace srsran;

csi_report_configuration srsran::create_csi_report_configuration(const csi_meas_config& csi_meas)
{
  csi_report_configuration csi_rep = {};
  csi_rep.pmi_codebook             = pmi_codebook_type::one;

  // TODO: support more CSI reports.
  const csi_report_config& csi_rep_cfg = csi_meas.csi_report_cfg_list[0];

  // TODO: support more CSI resource sets.
  nzp_csi_rs_res_set_id_t nzp_csi_set_id =
      variant_get<csi_resource_config::nzp_csi_rs_ssb>(
          csi_meas.csi_res_cfg_list[csi_rep_cfg.res_for_channel_meas].csi_rs_res_set_list)
          .nzp_csi_rs_res_set_list[0];
  csi_rep.nof_csi_rs_resources = csi_meas.nzp_csi_rs_res_set_list[nzp_csi_set_id].nzp_csi_rs_res.size();

  // Enable indicators
  switch (csi_rep_cfg.report_qty_type) {
    case csi_report_config::report_quantity_type_t::none:
      break;
    case csi_report_config::report_quantity_type_t::cri_ri_li_pmi_cqi:
      csi_rep.quantities = csi_report_quantities::cri_ri_li_pmi_cqi;
      break;
    case csi_report_config::report_quantity_type_t::cri_ri_pmi_cqi:
      csi_rep.quantities = csi_report_quantities::cri_ri_pmi_cqi;
      break;
    case csi_report_config::report_quantity_type_t::cri_ri_cqi:
      csi_rep.quantities = csi_report_quantities::cri_ri_cqi;
      break;
    default:
      csi_rep.quantities = csi_report_quantities::other;
      break;
  }

  if (csi_rep_cfg.codebook_cfg.has_value()) {
    if (variant_holds_alternative<codebook_config::type1>(csi_rep_cfg.codebook_cfg->codebook_type)) {
      const auto& type1 = variant_get<codebook_config::type1>(csi_rep_cfg.codebook_cfg->codebook_type);
      if (variant_holds_alternative<codebook_config::type1::single_panel>(type1.sub_type)) {
        using single_panel = codebook_config::type1::single_panel;
        const auto& panel  = variant_get<single_panel>(type1.sub_type);

        if (variant_holds_alternative<single_panel::two_antenna_ports_two_tx_codebook_subset_restriction>(
                panel.nof_antenna_ports)) {
          csi_rep.pmi_codebook = pmi_codebook_type::two;
        } else if (variant_holds_alternative<single_panel::more_than_two_antenna_ports>(panel.nof_antenna_ports)) {
          csi_rep.pmi_codebook = pmi_codebook_type::typeI_single_panel_4ports_mode1;
        } else {
          csi_rep.pmi_codebook = pmi_codebook_type::other;
        }

        csi_rep.ri_restriction = panel.typei_single_panel_ri_restriction;
      } else {
        report_fatal_error("Codebook panel type not supported");
      }
    } else {
      report_fatal_error("Codebook type not supported");
    }
  }

  return csi_rep;
}
