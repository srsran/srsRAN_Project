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

#include "e2sm_kpm_utils.h"

namespace srsran {

std::string e2sm_kpm_label_2_str(e2sm_kpm_label_enum label)
{
  switch (label) {
    case NO_LABEL:
      return "NO_LABEL";
    case PLMN_ID_LABEL:
      return "PLMN_ID_LABEL";
    case SLICE_ID_LABEL:
      return "SLICE_ID_LABEL";
    case FIVE_QI_LABEL:
      return "FIVE_QI_LABEL";
    case QFI_LABEL:
      return "QFI_LABEL";
    case QCI_LABEL:
      return "QCI_LABEL";
    case QCI_MAX_LABEL:
      return "QCI_MAX_LABEL";
    case QCI_MIN_LABEL:
      return "QCI_MIN_LABEL";
    case ARP_MAX_LABEL:
      return "ARP_MAX_LABEL";
    case ARP_MIN_LABEL:
      return "ARP_MIN_LABEL";
    case BITRATE_RANGE_LABEL:
      return "BITRATE_RANGE_LABEL";
    case LAYER_MU_MIMO_LABEL:
      return "LAYER_MU_MIMO_LABEL";
    case SUM_LABEL:
      return "SUM_LABEL";
    case DIST_BIN_X_LABEL:
      return "DIST_BIN_X_LABEL";
    case DIST_BIN_Y_LABEL:
      return "DIST_BIN_Y_LABEL";
    case DIST_BIN_Z_LABEL:
      return "DIST_BIN_Z_LABEL";
    case PRE_LABEL_OVERRIDE_LABEL:
      return "PRE_LABEL_OVERRIDE_LABEL";
    case START_END_IND_LABEL:
      return "START_END_IND_LABEL";
    case MIN_LABEL:
      return "MIN_LABEL";
    case MAX_LABEL:
      return "MAX_LABEL";
    case AVG_LABEL:
      return "AVG_LABEL";
    case SSB_IDX_LABEL:
      return "SSB_IDX_LABEL";
    case NON_GO_B_BFMODE_IDX_LABEL:
      return "NON_GO_B_BFMODE_IDX_LABEL";
    case MIMO_MODE_IDX_LABEL:
      return "MIMO_MODE_IDX_LABEL";
    case ALL_VALUE_TYPE_LABELS:
      return "ALL_VALUE_TYPE_LABELS";
    case ALL_SUBCOUNTER_LABELS:
      return "ALL_SUBCOUNTER_LABELS";
    case ALL_LABELS:
      return "ALL_LABELS";
    default:
      return "UNKNOWN_LABEL";
  }
}

std::string e2sm_kpm_scope_2_str(e2sm_kpm_metric_level_enum level)
{
  switch (level) {
    case E2_NODE_LEVEL:
      return "E2_NODE_LEVEL";
    case UE_LEVEL:
      return "UE_LEVEL";
    case QOS_FLOW_LEVEL:
      return "QOS_FLOW_LEVEL";
    case ALL_LEVELS:
      return "ALL_LEVELS";
    default:
      return "UNKNOWN_LEVEL";
  }
}

e2sm_kpm_label_enum asn1_label_2_enum(const asn1::e2sm_kpm::meas_label_s& meas_label)
{
  if (meas_label.no_label_present) {
    return NO_LABEL;
  }
  if (meas_label.sum_present) {
    return SUM_LABEL;
  }
  if (meas_label.min_present) {
    return MIN_LABEL;
  }
  if (meas_label.max_present) {
    return MAX_LABEL;
  }
  if (meas_label.avg_present) {
    return AVG_LABEL;
  }
  if (meas_label.plmn_id_present) {
    return PLMN_ID_LABEL;
  }
  if (meas_label.slice_id_present) {
    return SLICE_ID_LABEL;
  }
  if (meas_label.five_qi_present) {
    return FIVE_QI_LABEL;
  }
  if (meas_label.qfi_present) {
    return QFI_LABEL;
  }
  if (meas_label.qci_present) {
    return QCI_LABEL;
  }
  if (meas_label.qcimax_present) {
    return QCI_MAX_LABEL;
  }
  if (meas_label.qcimin_present) {
    return QCI_MIN_LABEL;
  }
  if (meas_label.arpmax_present) {
    return ARP_MAX_LABEL;
  }
  if (meas_label.arpmin_present) {
    return ARP_MIN_LABEL;
  }
  if (meas_label.bitrate_range_present) {
    return BITRATE_RANGE_LABEL;
  }
  if (meas_label.layer_mu_mimo_present) {
    return LAYER_MU_MIMO_LABEL;
  }
  if (meas_label.dist_bin_x_present) {
    return DIST_BIN_X_LABEL;
  }
  if (meas_label.dist_bin_y_present) {
    return DIST_BIN_Y_LABEL;
  }
  if (meas_label.dist_bin_z_present) {
    return DIST_BIN_Z_LABEL;
  }
  if (meas_label.pre_label_override_present) {
    return PRE_LABEL_OVERRIDE_LABEL;
  }
  if (meas_label.start_end_ind_present) {
    return START_END_IND_LABEL;
  }
  if (meas_label.ssb_idx_present) {
    return SSB_IDX_LABEL;
  }
  if (meas_label.non_go_b_bfmode_idx_present) {
    return NON_GO_B_BFMODE_IDX_LABEL;
  }
  if (meas_label.mimo_mode_idx_present) {
    return MIMO_MODE_IDX_LABEL;
  }
  return UNKNOWN_LABEL;
}

bool operator==(asn1::e2sm_kpm::ueid_c const& lhs, asn1::e2sm_kpm::ueid_c const& rhs)
{
  if (lhs.type() != rhs.type()) {
    return false;
  }

  // TODO: add support for all types
  switch (lhs.type()) {
    case asn1::e2sm_kpm::ueid_c::types_opts::gnb_du_ueid:
      if (lhs.gnb_du_ueid() == rhs.gnb_du_ueid()) {
        return true;
      }
    default:
      return false;
  }
}

bool operator!=(asn1::e2sm_kpm::ueid_c const& lhs, asn1::e2sm_kpm::ueid_c const& rhs)
{
  return !(lhs == rhs);
}

bool operator<(asn1::e2sm_kpm::ueid_c const& lhs, asn1::e2sm_kpm::ueid_c const& rhs)
{
  srsran_assert(lhs.type() != rhs.type(), "Cannot compare UEIDs of different types.");
  switch (lhs.type()) {
    case asn1::e2sm_kpm::ueid_c::types_opts::gnb_du_ueid:
      if (lhs.gnb_du_ueid() < rhs.gnb_du_ueid()) {
        return true;
      }
    default:
      return false;
  }
}

bool operator==(asn1::e2sm_kpm::ueid_gnb_du_s const& lhs, asn1::e2sm_kpm::ueid_gnb_du_s const& rhs)
{
  return lhs.gnb_cu_ue_f1_ap_id == rhs.gnb_cu_ue_f1_ap_id;
}

bool operator<(asn1::e2sm_kpm::ueid_gnb_du_s const& lhs, asn1::e2sm_kpm::ueid_gnb_du_s const& rhs)
{
  return lhs.gnb_cu_ue_f1_ap_id < rhs.gnb_cu_ue_f1_ap_id;
}

} // namespace srsran
