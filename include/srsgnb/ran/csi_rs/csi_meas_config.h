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

#include "csi_report_config.h"
#include "csi_resource_config.h"
#include "csi_rs_id.h"
#include "csi_rs_pattern.h"
#include "csi_rs_types.h"
#include "srsgnb/adt/variant.h"
#include "srsgnb/ran/tci.h"

namespace srsgnb {

/// CSI-RS-ResourceMapping is used to configure the resource element mapping of a CSI-RS resource in time- and frequency
/// domain
/// \remark See TS 38.331, \c CSI-RS-ResourceMapping.
using csi_rs_resource_mapping = csi_rs_pattern_configuration;

/// Used to configure a periodicity for periodic and semi-persistent CSI resources, and for
/// periodic and semi-persistent reporting on PUCCH.
/// \remark See TS 38.331, \c CSI-ResourcePeriodicityAndOffset.
enum class csi_resource_periodicity {
  slots4,
  slots5,
  slots8,
  slots10,
  slots16,
  slots20,
  slots32,
  slots40,
  slots64,
  slots80,
  slots160,
  slots320,
  slots640
};

/// \brief NZP-CSI-RS-Resource is used to configure Non-Zero-Power (NZP) CSI-RS transmitted in the cell.
/// \remark See TS 38.331, \c NZP-CSI-RS-Resource and TS 38.214, clause 5.2.2.3.1.
struct nzp_csi_rs_resource {
  nzp_csi_rs_res_id_t res_id;
  /// OFDM symbol location(s) in a slot and subcarrier occupancy in a PRB of the CSI-RS resource.
  csi_rs_resource_mapping res_mapping;
  /// Power offset of PDSCH RE to NZP CSI-RS RE. Value in dB. Values {-8,...,15}. See TS 38.214, clause 5.2.2.3.1.
  int pwr_ctrl_offset;
  /// Power offset of NZP CSI-RS RE to SSS RE. Value in dB. Values {-3, 0, 3, 6}. See TS 38.214, clause 5.2.2.3.1.
  optional<int> pwr_ctrl_offset_ss_db;
  /// Scrambling ID. See TS 38.214, clause 5.2.2.3.1.
  unsigned scrambling_id;
  /// Present only for periodic and semi-persistent NZP-CSI-RS-Resources.
  optional<csi_resource_periodicity> csi_res_period;
  /// Present only for periodic and semi-persistent NZP-CSI-RS-Resources. Values {0,...,(periodicity_in_slots - 1)}.
  optional<unsigned> csi_res_offset;
  /// Present only for periodic NZP-CSI-RS-Resources (as indicated in CSI-ResourceConfig).
  optional<tci_state_id_t> tci_state_id;
};

/// NZP-CSI-RS-ResourceSet is a set of Non-Zero-Power (NZP) CSI-RS resources (their IDs) and set-specific
/// parameters.
/// \remark See TS 38.331, \c NZP-CSI-RS-ResourceSet.
struct nzp_csi_rs_resource_set {
  nzp_csi_rs_res_set_id_t res_set_id;
  /// NZP-CSI-RS-Resources associated with this NZP-CSI-RS resource set (see TS 38.214 [19], clause 5.2). For CSI, there
  /// are at most 8 NZP CSI RS resources per resource set.
  static_vector<nzp_csi_rs_res_id_t, nzp_csi_rs_res_set_id_t::MAX_NOF_NZP_CSI_RS_RESOURCES_PER_SET> nzp_csi_rs_res;
  /// Indicates whether repetition is on/off. If false, UE may not assume that the NZP-CSI-RS resources within the
  /// resource set are transmitted with the same downlink spatial domain transmission filter.
  bool is_repetition_on{false};
  /// Offset X between the slot containing the DCI that triggers a set of aperiodic NZP CSI-RS resources and the slot in
  /// which the CSI-RS resource set is transmitted. Values {0,...,6}.
  unsigned aperiodic_trigger_offset;
  /// Indicates that the antenna port for all NZP-CSI-RS resources in the CSI-RS resource set is same.
  bool is_trs_info_present{false};
};

/// \brief CSI-IM-Resource is used to configure one CSI Interference Management (IM) resource.
/// \remark See TS 38.331, \c CSI-IM-Resource.
struct csi_im_resource {
  /// \brief The resource element pattern type.
  /// \remark TS 38.331, \c csi-IM-ResourceElementPattern.
  enum class csi_im_resource_element_pattern_type { pattern0, pattern1 };
  /// \brief The resource element pattern (Pattern0 (2,2) or Pattern1 (4,1)) with corresponding parameters
  /// \remark See TS 38.214, clause 5.2.2.4 and TS 38.331, \c csi-IM-ResourceElementPattern.
  struct csi_im_resource_element_pattern {
    csi_im_resource_element_pattern_type pattern_type;
    /// OFDM subcarrier occupancy of the CSI-IM resource for Pattern0 or Pattern1.
    /// For Pattern0, Values {0, 2, 4, 6, 8, 10} and for Pattern1, Values {0, 4, 8}.
    unsigned subcarrier_location;
    /// OFDM subcarrier occupancy of the CSI-IM resource for Pattern0 or Pattern1.
    /// For Pattern0, Values {0,...,12} and for Pattern1, Values {0,..,13}.
    unsigned symbol_location;
  };

  csi_im_res_id_t                           res_id;
  optional<csi_im_resource_element_pattern> csi_im_res_element_pattern;
  /// PRB where the CSI resource starts, related to the CRB 0.
  optional<unsigned> start_rb;
  /// Number of PRBs that the CSI spans.
  optional<unsigned> nof_rb;
  /// Present only for periodic and semi-persistent NZP-CSI-RS-Resources.
  optional<csi_resource_periodicity> csi_res_period;
  /// Present only for periodic and semi-persistent NZP-CSI-RS-Resources. Values {0,...,(periodicity_in_slots - 1)}.
  optional<unsigned> csi_res_offset;
};

/// CSI-IM-ResourceSet is used to configure a set of one or more CSI Interference Management (IM) resources (their IDs)
/// and set-specific parameters
/// \remark See TS 38.331, \c CSI-IM-ResourceSet.
struct csi_im_resource_set {
  csi_im_res_set_id_t res_set_id;
  /// CSI-IM-Resources associated with this CSI-IM-ResourceSet. See TS 38.214, clause 5.2.
  static_vector<csi_im_res_id_t, csi_im_res_set_id_t::MAX_NOF_CSI_IM_RESOURCES_PER_SET> csi_ims_resources;
};

/// CSI-SSB-ResourceSet is used to configure one SS/PBCH block resource set which refers to SS/PBCH as indicated in
/// ServingCellConfigCommon.
/// \remark See TS 38.331, \c CSI-SSB-ResourceSet.
struct csi_ssb_resource_set {
  csi_ssb_res_set_id_t                                                             res_set_id;
  static_vector<ssb_id_t, csi_ssb_res_set_id_t::MAX_NOF_CSI_SSB_RESOURCES_PER_SET> csi_ssb_res_list;
};

/// See TS 38.331, \c CSI-AssociatedReportConfigInfo.
struct csi_associated_report_config_info {
  /// CSI-SSB-ResourceSet for channel measurements. Values
  /// {1,...,MAX_NOF_CSI_SSB_RESOURCE_SETS_PER_CSI_RESOURCE_CONFIG}.
  /// \remark See TS 38.331,\c CSI-SSB-ResourceSet.
  using csi_ssb_resource_set = unsigned;

  struct nzp_csi_rs {
    /// NZP-CSI-RS-ResourceSet for channel measurements. Values
    /// {1,...,MAX_NOF_NZP_CSI_RS_RESOURCE_SETS_PER_CSI_RESOURCE_CONFIG}.
    unsigned resource_set;
    /// Only present if the NZP-CSI-RS-Resources in the associated resourceSet have the resourceType aperiodic.
    static_vector<tci_state_id_t, MAX_NOF_AP_CSI_RS_RESOURCES_PER_SET> qcl_info;
  };

  csi_report_config_id_t                    report_cfg_id;
  variant<nzp_csi_rs, csi_ssb_resource_set> res_for_channel;
  /// CSI-IM-ResourceSet for interference measurement. Values
  /// {1,...,MAX_NOF_CSI_IM_RESOURCE_SETS_PER_CSI_RESOURCE_CONFIG}.
  /// Field is present if the CSI-ReportConfig identified by reportConfigId is configured with
  /// csi-IM-ResourcesForInterference.
  optional<unsigned> csi_im_resources_for_interference;
  /// NZP-CSI-RS-ResourceSet for interference measurement. Values
  /// {1,...,MAX_NOF_NZP_CSI_RS_RESOURCE_SETS_PER_CSI_RESOURCE_CONFIG}.
  /// Field is present if the CSI-ReportConfig identified by reportConfigId is configured with
  /// nzp-CSI-RS-ResourcesForInterference.
  optional<unsigned> nzp_csi_rs_resources_for_interference;
};

/// See TS 38.331, \c CSI-AperiodicTriggerState.
using csi_aperiodic_trigger_state =
    static_vector<csi_associated_report_config_info, MAX_NOF_REPORT_CONFIG_PER_APERIODIC_TRIGGER>;

/// Used to configure the UE with a list of aperiodic trigger states. Each codepoint of the DCI field "CSI request" is
/// associated with one trigger state.
/// \remark TS 38.331, \c CSI-AperiodicTriggerStateList.
using csi_aperiodic_trigger_state_list = static_vector<csi_aperiodic_trigger_state, MAX_NOF_CSI_APERIODIC_TRIGGERS>;

/// Used to configure the UE with a list of trigger states for semi-persistent reporting of channel state information on
/// L1.
/// \remark TS 38.331, \c CSI-SemiPersistentOnPUSCH-TriggerStateList.
using csi_semi_persistent_on_pusch_trigger_state_list =
    static_vector<csi_report_config_id_t, MAX_NOF_SEMI_PERSISTENT_PUSCH_TRIGGERS>;

/// \brief CSI-MeasConfig is used to configure CSI-RS belonging to the serving cell in which CSI-MeasConfig is included.
/// \remark See TS 38.331, \c CSI-MeasConfig.
struct csi_meas_config {
  /// Pool of \c NZP-CSI-RS-Resource which can be referred to from \c NZP-CSI-RS-ResourceSet.
  static_vector<nzp_csi_rs_resource, nzp_csi_rs_res_id_t::MAX_NOF_NZP_CSI_RS_RESOURCES> nzp_csi_rs_res_list;
  /// Pool of NZP-CSI-RS-ResourceSet which can be referred to from CSI-ResourceConfig or from MAC CEs.
  static_vector<nzp_csi_rs_resource_set, nzp_csi_rs_res_set_id_t::MAX_NOF_NZP_CSI_RS_RESOURCE_SETS>
      nzp_csi_rs_res_set_list;
  /// Pool of CSI-IM-Resource which can be referred to from CSI-IM-ResourceSet.
  static_vector<csi_im_resource, csi_im_res_id_t::MAX_NOF_CSI_IM_RESOURCES> csi_im_res_list;
  /// Pool of CSI-IM-ResourceSet which can be referred to from CSI-ResourceConfig or from MAC CEs.
  static_vector<csi_im_resource_set, csi_im_res_set_id_t::MAX_NOF_CSI_IM_RESOURCE_SETS> csi_im_res_set_list;
  /// Pool of CSI-SSB-ResourceSet which can be referred to from CSI-ResourceConfig.
  static_vector<csi_ssb_resource_set, csi_ssb_res_set_id_t::MAX_NOF_CSI_SSB_RESOURCE_SETS> csi_ssb_res_set_list;
  /// Configured CSI resource settings as specified in TS 38.214 clause 5.2.1.2.
  static_vector<csi_resource_config, csi_res_config_id_t::MAX_NOF_CSI_RESOURCE_CONFIGS> csi_res_cfg_list;
  /// Configured CSI report settings as specified in TS 38.214 clause 5.2.1.1.
  static_vector<csi_report_config, csi_report_config_id_t::MAX_NOF_CSI_REPORT_CONFIGS> csi_report_cfg_list;
  /// Size of CSI request field in DCI (bits). See TS 38.214, clause 5.2.1.5.1.
  optional<unsigned>                                        report_trigger_size;
  optional<csi_aperiodic_trigger_state_list>                aperiodic_trigger_state_list;
  optional<csi_semi_persistent_on_pusch_trigger_state_list> semi_persistent_on_pusch_trigger_state_list;
};

} // namespace srsgnb
