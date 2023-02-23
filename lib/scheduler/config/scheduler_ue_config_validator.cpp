/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/scheduler/config/scheduler_ue_config_validator.h"
#include "srsran/support/config/validator_helpers.h"

using namespace srsran;
using namespace config_validators;

#define VERIFY(cond, ...)                                                                                              \
  if (not(cond)) {                                                                                                     \
    return error_type<std::string>(fmt::format(__VA_ARGS__));                                                          \
  }
#define VERIFY_ID_EXISTS(cond_lambda, id_list, ...)                                                                    \
  if (std::find_if(id_list.begin(), id_list.end(), cond_lambda) == id_list.end()) {                                    \
    return error_type<std::string>(fmt::format(__VA_ARGS__));                                                          \
  }
#define HANDLE_CODE(cond)                                                                                              \
  {                                                                                                                    \
    auto ret = cond;                                                                                                   \
    if (ret.is_error()) {                                                                                              \
      return ret;                                                                                                      \
    }                                                                                                                  \
  }

error_type<std::string> srsran::config_validators::validate_pucch_cfg(const sched_ue_creation_request_message& msg)
{
  VERIFY(msg.cfg.cells.front().serv_cell_cfg.ul_config.has_value() and
             msg.cfg.cells.front().serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.has_value(),
         "Missing configuration for uplinkConfig or pucch-Config in spCellConfig");

  const auto& pucch_cfg = msg.cfg.cells.front().serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.value();

  VERIFY(pucch_cfg.format_1_common_param.has_value(), "Missing PUCCH-format1 parameters in PUCCH-Config");
  VERIFY(pucch_cfg.format_2_common_param.has_value(), "Missing PUCCH-format2 parameters in PUCCH-Config");

  // Verify that the PUCCH resources IDs of each PUCCH resource set point at a corresponding item in the PUCCH reource
  // list.
  VERIFY(pucch_cfg.pucch_res_set.size() >= 2, "At least 2 PUCCH resource sets need to be configured in PUCCH-Config");
  VERIFY(pucch_cfg.pucch_res_set[0].pucch_res_set_id == 0 and pucch_cfg.pucch_res_set[1].pucch_res_set_id == 1,
         "PUCCH resouce sets 0 and 1 are expected to have PUCCH-ResourceSetId 0 and 1, respectively");
  VERIFY((not pucch_cfg.pucch_res_set[0].pucch_res_id_list.empty()) and
             (not pucch_cfg.pucch_res_set[1].pucch_res_id_list.empty()),
         "PUCCH resouce sets 0 and 1 are expected to have a non-empty set of PUCCH resource id");
  for (size_t pucch_res_set_idx = 0; pucch_res_set_idx != 2; ++pucch_res_set_idx) {
    for (auto res_idx : pucch_cfg.pucch_res_set[pucch_res_set_idx].pucch_res_id_list) {
      const auto* it = std::find_if(pucch_cfg.pucch_res_list.begin(),
                                    pucch_cfg.pucch_res_list.end(),
                                    [res_idx](const pucch_resource& res) { return res_idx == res.res_id; });
      VERIFY(it != pucch_cfg.pucch_res_list.end(),
             "PUCCH res. index={} in PUCCH res. set id={} not found in the PUCCH resource list",
             res_idx,
             pucch_res_set_idx);
    }
  }

  // Check PUCCH Formats for each PUCCH Resource Set.
  for (auto res_idx : pucch_cfg.pucch_res_set[0].pucch_res_id_list) {
    VERIFY(pucch_cfg.pucch_res_list[res_idx].format == pucch_format::FORMAT_1,
           "Only PUCCH Resource Format 1 expected in PUCCH resource set 0.");
  }
  for (auto res_idx : pucch_cfg.pucch_res_set[1].pucch_res_id_list) {
    VERIFY(pucch_cfg.pucch_res_list[res_idx].format == pucch_format::FORMAT_2,
           "Only PUCCH Resource Format 2 expected in PUCCH resource set 1.");
  }

  // Verify the PUCCH resource id that indicated in the SR resource config exists in the PUCCH resource list.
  VERIFY(not pucch_cfg.sr_res_list.empty(), "SchedulingRequestResourceConfig list is empty");
  const auto* sr_pucch_res_id = std::find_if(pucch_cfg.pucch_res_list.begin(),
                                             pucch_cfg.pucch_res_list.end(),
                                             [sr_pucch_res_idx = pucch_cfg.sr_res_list.front().pucch_res_id](
                                                 const pucch_resource& res) { return sr_pucch_res_idx == res.res_id; });
  VERIFY(sr_pucch_res_id != pucch_cfg.pucch_res_list.end(),
         "PUCCH res. index={} given in SR resource config not found in the PUCCH resource list",
         pucch_cfg.sr_res_list.front().pucch_res_id);
  VERIFY(pucch_cfg.pucch_res_list[pucch_cfg.sr_res_list.front().pucch_res_id].format == pucch_format::FORMAT_1,
         "PUCCH resource used for SR is expected to be Format 1");

  // Verify that the PUCCH setting used for CSI report have been configured properly.
  if (msg.cfg.cells.front().serv_cell_cfg.csi_meas_cfg.has_value()) {
    const auto& csi_cfg = msg.cfg.cells.front().serv_cell_cfg.csi_meas_cfg.value();
    VERIFY(not csi_cfg.csi_report_cfg_list.empty() and
               variant_holds_alternative<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                   csi_cfg.csi_report_cfg_list.front().report_cfg_type) and
               not variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                       csi_cfg.csi_report_cfg_list.front().report_cfg_type)
                       .pucch_csi_res_list.empty(),
           "PUCCH-CSI-ResourceList has not been configured in the CSI-reportConfig",
           pucch_cfg.sr_res_list.front().pucch_res_id);

    const auto& csi = variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                          csi_cfg.csi_report_cfg_list.front().report_cfg_type)
                          .pucch_csi_res_list.front()
                          .pucch_res_id;
    // Verify the PUCCH resource id that indicated in the CSI resource config exists in the PUCCH resource list.
    const auto* csi_pucch_res_id = std::find_if(pucch_cfg.pucch_res_list.begin(),
                                                pucch_cfg.pucch_res_list.end(),
                                                [csi](const pucch_resource& res) { return csi == res.res_id; });
    VERIFY(csi_pucch_res_id != pucch_cfg.pucch_res_list.end(),
           "PUCCH res. index={} given in PUCCH-CSI-resourceList not found in the PUCCH resource list",
           csi);
    VERIFY(pucch_cfg.pucch_res_list[csi].format == pucch_format::FORMAT_2,
           "PUCCH resource used for CSI is expected to be Format 2");
  }

  for (const cell_config_dedicated& cell : msg.cfg.cells) {
    if (cell.serv_cell_cfg.ul_config.has_value()) {
      const auto& init_ul_bwp = cell.serv_cell_cfg.ul_config.value().init_ul_bwp;
      if (init_ul_bwp.pucch_cfg.has_value()) {
        const auto& dl_to_ack_lst = init_ul_bwp.pucch_cfg.value().dl_data_to_ul_ack;
        for (const auto& k1 : dl_to_ack_lst) {
          VERIFY(k1 <= SCHEDULER_MAX_K1, "k1={} value exceeds maximum supported k1", k1);
        }
      }
    }
  }

  return {};
}

error_type<std::string> srsran::config_validators::validate_pdsch_cfg(const sched_ue_creation_request_message& msg)
{
  for (const cell_config_dedicated& cell : msg.cfg.cells) {
    const auto& init_dl_bwp = cell.serv_cell_cfg.init_dl_bwp;
    if (init_dl_bwp.pdsch_cfg.has_value()) {
      const auto& dl_lst = init_dl_bwp.pdsch_cfg.value().pdsch_td_alloc_list;
      for (const auto& pdsch : dl_lst) {
        VERIFY(pdsch.k0 <= SCHEDULER_MAX_K0, "k0={} value exceeds maximum supported k0", pdsch.k0);
      }
    }
  }
  // TODO: Validate other parameters.
  return {};
}

error_type<std::string> srsran::config_validators::validate_csi_meas_cfg(const sched_ue_creation_request_message& msg)
{
  for (const cell_config_dedicated& cell : msg.cfg.cells) {
    if (cell.serv_cell_cfg.csi_meas_cfg.has_value()) {
      const csi_meas_config& csi_meas_cfg = cell.serv_cell_cfg.csi_meas_cfg.value();

      // Ensure no Resource and ResourceSet ID duplications.
      VERIFY(has_unique_ids(csi_meas_cfg.nzp_csi_rs_res_list, &nzp_csi_rs_resource::res_id),
             "Duplication of NZP-CSI-RS-ResourceId");
      VERIFY(has_unique_ids(csi_meas_cfg.nzp_csi_rs_res_set_list, &nzp_csi_rs_resource_set::res_set_id),
             "Duplication of NZP-CSI-RS-ResourceSetId");
      VERIFY(has_unique_ids(csi_meas_cfg.csi_im_res_list, &csi_im_resource::res_id),
             "Duplication of CSI-IM-ResourceId");
      VERIFY(has_unique_ids(csi_meas_cfg.csi_im_res_set_list, &csi_im_resource_set::res_set_id),
             "Duplication of CSI-IM-ResourceSetId");
      VERIFY(has_unique_ids(csi_meas_cfg.csi_ssb_res_set_list, &csi_ssb_resource_set::res_set_id),
             "Duplication of CSI-SSB-ResourceSetId");
      VERIFY(has_unique_ids(csi_meas_cfg.csi_res_cfg_list, &csi_resource_config::res_cfg_id),
             "Duplication of CSI-ResourceConfigId");
      VERIFY(has_unique_ids(csi_meas_cfg.csi_report_cfg_list, &csi_report_config::report_cfg_id),
             "Duplication of CSI-ReportConfigId");

      // NZP-CSI-RS-ResourceSet.
      for (const auto& res_set : csi_meas_cfg.nzp_csi_rs_res_set_list) {
        for (const auto& res_id : res_set.nzp_csi_rs_res) {
          VERIFY_ID_EXISTS([res_id](const nzp_csi_rs_resource& rhs) { return rhs.res_id == res_id; },
                           csi_meas_cfg.nzp_csi_rs_res_list,
                           "NZP CSI-RS resource id={} does not exist",
                           res_id);
        }
      }
      // CSI-IM-ResourceSet.
      for (const auto& res_set : csi_meas_cfg.csi_im_res_set_list) {
        for (const auto& res_id : res_set.csi_ims_resources) {
          VERIFY_ID_EXISTS([res_id](const csi_im_resource& rhs) { return rhs.res_id == res_id; },
                           csi_meas_cfg.csi_im_res_list,
                           "CSI-IM resource id={} does not exist",
                           res_id);
        }
      }
      // CSI-ResourceConfig.
      for (const auto& res_cfg : csi_meas_cfg.csi_res_cfg_list) {
        if (variant_holds_alternative<csi_resource_config::nzp_csi_rs_ssb>(res_cfg.csi_rs_res_set_list)) {
          const auto& variant_value = variant_get<csi_resource_config::nzp_csi_rs_ssb>(res_cfg.csi_rs_res_set_list);
          for (const auto& res_set_id : variant_value.nzp_csi_rs_res_set_list) {
            VERIFY_ID_EXISTS([res_set_id](const nzp_csi_rs_resource_set& rhs) { return rhs.res_set_id == res_set_id; },
                             csi_meas_cfg.nzp_csi_rs_res_set_list,
                             "NZP CSI-RS resource set id={} does not exist",
                             res_set_id);
          }
          for (const auto& res_set_id : variant_value.csi_ssb_res_set_list) {
            VERIFY_ID_EXISTS([res_set_id](const csi_ssb_resource_set& rhs) { return rhs.res_set_id == res_set_id; },
                             csi_meas_cfg.csi_ssb_res_set_list,
                             "CSI SSB resource set id={} does not exist",
                             res_set_id);
          }
        } else if (variant_holds_alternative<csi_resource_config::csi_im_resource_set_list>(
                       res_cfg.csi_rs_res_set_list)) {
          const auto& variant_value =
              variant_get<csi_resource_config::csi_im_resource_set_list>(res_cfg.csi_rs_res_set_list);
          for (const auto& res_set_id : variant_value) {
            VERIFY_ID_EXISTS([res_set_id](const csi_im_resource_set& rhs) { return rhs.res_set_id == res_set_id; },
                             csi_meas_cfg.csi_im_res_set_list,
                             "CSI IM resource set id={} does not exist",
                             res_set_id);
          }
        }
        // TODO: BWP-Id and Resource Type validation.
      }
      // CSI-ReportConfig.
      for (const auto& rep_cfg : csi_meas_cfg.csi_report_cfg_list) {
        const auto res_for_channel_meas = rep_cfg.res_for_channel_meas;
        VERIFY_ID_EXISTS(
            [res_for_channel_meas](const csi_resource_config& rhs) { return rhs.res_cfg_id == res_for_channel_meas; },
            csi_meas_cfg.csi_res_cfg_list,
            "CSI Recourse Config id={} does not exist",
            res_for_channel_meas);

        if (rep_cfg.csi_im_res_for_interference.has_value()) {
          const auto csi_im_res_for_interference = rep_cfg.csi_im_res_for_interference.value();
          VERIFY_ID_EXISTS(
              [csi_im_res_for_interference](const csi_resource_config& rhs) {
                return rhs.res_cfg_id == csi_im_res_for_interference;
              },
              csi_meas_cfg.csi_res_cfg_list,
              "CSI Recourse Config id={} does not exist",
              csi_im_res_for_interference);
        }

        if (rep_cfg.nzp_csi_rs_res_for_interference.has_value()) {
          const auto nzp_csi_rs_res_for_interference = rep_cfg.nzp_csi_rs_res_for_interference.value();
          VERIFY_ID_EXISTS(
              [nzp_csi_rs_res_for_interference](const csi_resource_config& rhs) {
                return rhs.res_cfg_id == nzp_csi_rs_res_for_interference;
              },
              csi_meas_cfg.csi_res_cfg_list,
              "CSI Recourse Config id={} does not exist",
              nzp_csi_rs_res_for_interference);
        }

        if (variant_holds_alternative<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                rep_cfg.report_cfg_type)) {
          const auto& pucch_csi =
              variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(rep_cfg.report_cfg_type);
          VERIFY(cell.serv_cell_cfg.ul_config.has_value(), "Cell={} does not define a UL Config", cell.serv_cell_idx);
          VERIFY(cell.serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.has_value(),
                 "Cell={} does not define a PUCCH Config",
                 cell.serv_cell_idx);
          const auto& pucch_resources =
              cell.serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.value().pucch_res_list;
          for (const auto& pucch_res : pucch_csi.pucch_csi_res_list) {
            const auto& pucch_res_id = pucch_res.pucch_res_id;
            VERIFY_ID_EXISTS([pucch_res_id](const pucch_resource& rhs) { return rhs.res_id == pucch_res_id; },
                             pucch_resources,
                             "PUCCH resource id={} does not exist",
                             pucch_res_id);
          }
        }
      }
    }
  }
  // TODO: Validate other parameters.
  return {};
}

error_type<std::string>
srsran::config_validators::validate_sched_ue_creation_request_message(const sched_ue_creation_request_message& msg)
{
  // Verify the list of ServingCellConfig contains spCellConfig.
  VERIFY(not msg.cfg.cells.empty(), "Empty list of ServingCellConfig");

  HANDLE_CODE(validate_pdsch_cfg(msg));

  HANDLE_CODE(validate_pucch_cfg(msg));

  HANDLE_CODE(validate_csi_meas_cfg(msg));

  // TODO: Validate other parameters.
  return {};
}
