/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2ap/e2sm_kpm.h"

namespace srsran {

/* Scopes supported for a metric */
enum e2sm_kpm_metric_level_enum {
  E2_NODE_LEVEL  = 0x01,
  UE_LEVEL       = 0x02,
  QOS_FLOW_LEVEL = 0x04,
  ALL_LEVELS     = 0x07,
  UNKNOWN_LEVEL  = 0x10
};

/* Labels supported for a metric */
enum e2sm_kpm_label_enum {
  NO_LABEL                  = 0x00000001, // if included, other labels shall not be included (and vice versa)
  PLMN_ID_LABEL             = 0x00000002,
  SLICE_ID_LABEL            = 0x00000004,
  FIVE_QI_LABEL             = 0x00000008,
  QFI_LABEL                 = 0x00000010,
  QCI_LABEL                 = 0x00000020,
  QCI_MAX_LABEL             = 0x00000040,
  QCI_MIN_LABEL             = 0x00000080,
  ARP_MAX_LABEL             = 0x00000100,
  ARP_MIN_LABEL             = 0x00000200,
  BITRATE_RANGE_LABEL       = 0x00000400,
  LAYER_MU_MIMO_LABEL       = 0x00000800,
  SUM_LABEL                 = 0x00001000,
  DIST_BIN_X_LABEL          = 0x00002000,
  DIST_BIN_Y_LABEL          = 0x00004000,
  DIST_BIN_Z_LABEL          = 0x00008000,
  PRE_LABEL_OVERRIDE_LABEL  = 0x00010000,
  START_END_IND_LABEL       = 0x00020000,
  MIN_LABEL                 = 0x00040000,
  MAX_LABEL                 = 0x00080000,
  AVG_LABEL                 = 0x00100000,
  SSB_IDX_LABEL             = 0x00200000,
  NON_GO_B_BFMODE_IDX_LABEL = 0x00400000,
  MIMO_MODE_IDX_LABEL       = 0x00800000,
  ALL_VALUE_TYPE_LABELS     = 0x001C1000, // SUM | MIN | MAX | AVG
  ALL_SUBCOUNTER_LABELS     = 0x00E3EFFE,
  ALL_LABELS                = 0x00FFFFFF,
  UNKNOWN_LABEL             = 0x10000000
};

std::string         e2sm_kpm_label_2_str(e2sm_kpm_label_enum label);
std::string         e2sm_kpm_scope_2_str(e2sm_kpm_metric_level_enum level);
e2sm_kpm_label_enum asn1_label_2_enum(const asn1::e2sm_kpm::meas_label_s& meas_label);

} // namespace srsran
