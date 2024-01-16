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

#pragma once

#include "csi_report_config.h"
#include "csi_resource_config.h"
#include "csi_rs_id.h"
#include "csi_rs_pattern.h"
#include "csi_rs_types.h"
#include "frequency_allocation_type.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/static_vector.h"
#include "srsran/adt/variant.h"
#include "srsran/ran/tci.h"

namespace srsran {

/// CSI-RS-ResourceMapping is used to configure the resource element mapping of a CSI-RS resource in time- and frequency
/// domain
/// \remark See TS 38.331, \c CSI-RS-ResourceMapping.
struct csi_rs_resource_mapping {
  /// \brief Frequency domain allocation within a physical resource block in accordance with TS 38.211,
  /// clause 7.4.1.5.3, and \c frequencyDomainAllocation in \c CSI-RS-ResourceMapping in TS38.331. Possible sizes:
  /// {row1: 4, row2: 12, row4: 3, row_other: 6}
  csi_rs::freq_allocation_mask_type fd_alloc;
  /// Number of ports. Values {1, 2, 4, 8, 12, 16, 24, 32}.
  unsigned nof_ports;
  /// The time domain location reference \f$l_0\f$ in TS 38.211, clause 7.4.1.5.3. Values {0,...,13}.
  unsigned first_ofdm_symbol_in_td;
  /// The time domain location reference \f$l_1\f$ in TS 38.211, clause 7.4.1.5.3. Values {2,...,12}.
  optional<unsigned> first_ofdm_symbol_in_td2;
  /// CDM configuration.
  csi_rs_cdm_type cdm;
  /// Frequency density configuration.
  csi_rs_freq_density_type freq_density;
  /// \brief Frequency band of the CSI-RS resource in CRBs (in relation to CRB#0). Only multiples of 4 are allowed
  /// for the start and length. The smallest configurable length is equal to MIN(24, BWP size in RBs). If the
  /// configured value is larger than the width of the corresponding BWP, the UE shall assume that the actual CSI-RS
  /// bandwidth is equal to the width of the BWP.
  crb_interval freq_band_rbs;

  bool operator==(const csi_rs_resource_mapping& rhs) const
  {
    return fd_alloc == rhs.fd_alloc && nof_ports == rhs.nof_ports &&
           first_ofdm_symbol_in_td == rhs.first_ofdm_symbol_in_td &&
           first_ofdm_symbol_in_td2 == rhs.first_ofdm_symbol_in_td2 && cdm == rhs.cdm &&
           freq_density == rhs.freq_density && freq_band_rbs == rhs.freq_band_rbs;
  }
  bool operator!=(const csi_rs_resource_mapping& rhs) const { return !(rhs == *this); }
};

/// Used to configure a periodicity for periodic and semi-persistent CSI resources, and for
/// periodic and semi-persistent reporting on PUCCH.
/// \remark See TS 38.331, \c CSI-ResourcePeriodicityAndOffset.
enum class csi_resource_periodicity {
  slots4   = 4,
  slots5   = 5,
  slots8   = 8,
  slots10  = 10,
  slots16  = 16,
  slots20  = 20,
  slots32  = 32,
  slots40  = 40,
  slots64  = 64,
  slots80  = 80,
  slots160 = 160,
  slots320 = 320,
  slots640 = 640
};

inline unsigned csi_resource_periodicity_to_uint(csi_resource_periodicity val)
{
  return static_cast<unsigned>(val);
}

inline const span<const csi_resource_periodicity> csi_resource_periodicity_options()
{
  const static std::array<csi_resource_periodicity, 13> list{csi_resource_periodicity::slots4,
                                                             csi_resource_periodicity::slots5,
                                                             csi_resource_periodicity::slots8,
                                                             csi_resource_periodicity::slots10,
                                                             csi_resource_periodicity::slots16,
                                                             csi_resource_periodicity::slots20,
                                                             csi_resource_periodicity::slots32,
                                                             csi_resource_periodicity::slots40,
                                                             csi_resource_periodicity::slots64,
                                                             csi_resource_periodicity::slots80,
                                                             csi_resource_periodicity::slots160,
                                                             csi_resource_periodicity::slots320,
                                                             csi_resource_periodicity::slots640};
  return span<const csi_resource_periodicity>{list};
}

/// \brief NZP-CSI-RS-Resource is used to configure Non-Zero-Power (NZP) CSI-RS transmitted in the cell.
/// \remark See TS 38.331, \c NZP-CSI-RS-Resource and TS 38.214, clause 5.2.2.3.1.
struct nzp_csi_rs_resource {
  nzp_csi_rs_res_id_t res_id;
  /// OFDM symbol location(s) in a slot and subcarrier occupancy in a PRB of the CSI-RS resource.
  csi_rs_resource_mapping res_mapping;
  /// Power offset of PDSCH RE to NZP CSI-RS RE. Value in dB. Values {-8,...,15}. See TS 38.214, clause 5.2.2.3.1.
  int8_t pwr_ctrl_offset;
  /// Power offset of NZP CSI-RS RE to SSS RE. Value in dB. Values {-3, 0, 3, 6}. See TS 38.214, clause 5.2.2.3.1.
  optional<int8_t> pwr_ctrl_offset_ss_db;
  /// Scrambling ID. See TS 38.214, clause 5.2.2.3.1.
  unsigned scrambling_id;
  /// Present only for periodic and semi-persistent NZP-CSI-RS-Resources.
  optional<csi_resource_periodicity> csi_res_period;
  /// Present only for periodic and semi-persistent NZP-CSI-RS-Resources. Values {0,...,(periodicity_in_slots - 1)}.
  optional<unsigned> csi_res_offset;
  /// Present only for periodic NZP-CSI-RS-Resources (as indicated in CSI-ResourceConfig).
  optional<tci_state_id_t> qcl_info_periodic_csi_rs;

  bool operator==(const nzp_csi_rs_resource& rhs) const
  {
    return res_id == rhs.res_id && res_mapping == rhs.res_mapping && pwr_ctrl_offset == rhs.pwr_ctrl_offset &&
           pwr_ctrl_offset_ss_db == rhs.pwr_ctrl_offset_ss_db && scrambling_id == rhs.scrambling_id &&
           csi_res_period == rhs.csi_res_period && csi_res_offset == rhs.csi_res_offset &&
           qcl_info_periodic_csi_rs == rhs.qcl_info_periodic_csi_rs;
  }
  bool operator!=(const nzp_csi_rs_resource& rhs) const { return !(rhs == *this); }
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
  optional<bool> is_repetition_on;
  /// Offset X between the slot containing the DCI that triggers a set of aperiodic NZP CSI-RS resources and the slot in
  /// which the CSI-RS resource set is transmitted. Values {0,...,6}.
  optional<unsigned> aperiodic_trigger_offset;
  /// Indicates that the antenna port for all NZP-CSI-RS resources in the CSI-RS resource set is same.
  bool is_trs_info_present{false};

  bool operator==(const nzp_csi_rs_resource_set& rhs) const
  {
    return res_set_id == rhs.res_set_id && nzp_csi_rs_res == rhs.nzp_csi_rs_res &&
           is_repetition_on == rhs.is_repetition_on && aperiodic_trigger_offset == rhs.aperiodic_trigger_offset &&
           is_trs_info_present == rhs.is_trs_info_present;
  }
  bool operator!=(const nzp_csi_rs_resource_set& rhs) const { return !(rhs == *this); }
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

    bool operator==(const csi_im_resource_element_pattern& rhs) const
    {
      return pattern_type == rhs.pattern_type && subcarrier_location == rhs.subcarrier_location &&
             symbol_location == rhs.symbol_location;
    }
    bool operator!=(const csi_im_resource_element_pattern& rhs) const { return !(rhs == *this); }
  };

  csi_im_res_id_t                           res_id;
  optional<csi_im_resource_element_pattern> csi_im_res_element_pattern;
  /// RBs that the CSI resource spans, related to the CRB 0.
  crb_interval freq_band_rbs;
  /// Present only for periodic and semi-persistent NZP-CSI-RS-Resources.
  optional<csi_resource_periodicity> csi_res_period;
  /// Present only for periodic and semi-persistent NZP-CSI-RS-Resources. Values {0,...,(periodicity_in_slots - 1)}.
  optional<unsigned> csi_res_offset;

  bool operator==(const csi_im_resource& rhs) const
  {
    return res_id == rhs.res_id && csi_im_res_element_pattern == rhs.csi_im_res_element_pattern &&
           freq_band_rbs == rhs.freq_band_rbs && csi_res_period == rhs.csi_res_period &&
           csi_res_offset == rhs.csi_res_offset;
  }
  bool operator!=(const csi_im_resource& rhs) const { return !(rhs == *this); }
};

/// CSI-IM-ResourceSet is used to configure a set of one or more CSI Interference Management (IM) resources (their IDs)
/// and set-specific parameters
/// \remark See TS 38.331, \c CSI-IM-ResourceSet.
struct csi_im_resource_set {
  csi_im_res_set_id_t res_set_id;
  /// CSI-IM-Resources associated with this CSI-IM-ResourceSet. See TS 38.214, clause 5.2.
  static_vector<csi_im_res_id_t, csi_im_res_set_id_t::MAX_NOF_CSI_IM_RESOURCES_PER_SET> csi_ims_resources;

  bool operator==(const csi_im_resource_set& rhs) const
  {
    return res_set_id == rhs.res_set_id && csi_ims_resources == rhs.csi_ims_resources;
  }
  bool operator!=(const csi_im_resource_set& rhs) const { return !(rhs == *this); }
};

/// CSI-SSB-ResourceSet is used to configure one SS/PBCH block resource set which refers to SS/PBCH as indicated in
/// ServingCellConfigCommon.
/// \remark See TS 38.331, \c CSI-SSB-ResourceSet.
struct csi_ssb_resource_set {
  csi_ssb_res_set_id_t                                                             res_set_id;
  static_vector<ssb_id_t, csi_ssb_res_set_id_t::MAX_NOF_CSI_SSB_RESOURCES_PER_SET> csi_ssb_res_list;

  bool operator==(const csi_ssb_resource_set& rhs) const
  {
    return res_set_id == rhs.res_set_id && csi_ssb_res_list == rhs.csi_ssb_res_list;
  }
  bool operator!=(const csi_ssb_resource_set& rhs) const { return !(rhs == *this); }
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

    bool operator==(const nzp_csi_rs& rhs) const
    {
      return resource_set == rhs.resource_set && qcl_info == rhs.qcl_info;
    }
    bool operator!=(const nzp_csi_rs& rhs) const { return !(rhs == *this); }
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

  bool operator==(const csi_associated_report_config_info& rhs) const
  {
    return report_cfg_id == rhs.report_cfg_id && res_for_channel == rhs.res_for_channel &&
           csi_im_resources_for_interference == rhs.csi_im_resources_for_interference &&
           nzp_csi_rs_resources_for_interference == rhs.nzp_csi_rs_resources_for_interference;
  }
  bool operator!=(const csi_associated_report_config_info& rhs) const { return !(rhs == *this); }
};

/// See TS 38.331, \c CSI-AperiodicTriggerState.
struct csi_aperiodic_trigger_state {
  static_vector<csi_associated_report_config_info, MAX_NOF_REPORT_CONFIG_PER_APERIODIC_TRIGGER>
      associated_report_cfg_info_list;

  bool operator==(const csi_aperiodic_trigger_state& rhs) const
  {
    return associated_report_cfg_info_list == rhs.associated_report_cfg_info_list;
  }
  bool operator!=(const csi_aperiodic_trigger_state& rhs) const { return !(rhs == *this); }
};

/// Used to configure the UE with a list of aperiodic trigger states. Each codepoint of the DCI field "CSI request" is
/// associated with one trigger state.
/// \remark TS 38.331, \c CSI-AperiodicTriggerStateList.
using csi_aperiodic_trigger_state_list = static_vector<csi_aperiodic_trigger_state, MAX_NOF_CSI_APERIODIC_TRIGGERS>;

/// See TS 38.331, \c CSI-SemiPersistentOnPUSCH-TriggerState.
struct csi_semi_persistent_on_pusch_trigger_state {
  csi_report_config_id_t associated_report_cfg_info;

  bool operator==(const csi_semi_persistent_on_pusch_trigger_state& rhs) const
  {
    return associated_report_cfg_info == rhs.associated_report_cfg_info;
  }
  bool operator!=(const csi_semi_persistent_on_pusch_trigger_state& rhs) const { return !(rhs == *this); }
};

/// Used to configure the UE with a list of trigger states for semi-persistent reporting of channel state information on
/// L1.
/// \remark TS 38.331, \c CSI-SemiPersistentOnPUSCH-TriggerStateList.
using csi_semi_persistent_on_pusch_trigger_state_list =
    static_vector<csi_semi_persistent_on_pusch_trigger_state, MAX_NOF_SEMI_PERSISTENT_PUSCH_TRIGGERS>;

/// \brief CSI-MeasConfig is used to configure CSI-RS belonging to the serving cell in which CSI-MeasConfig is included.
/// \remark See TS 38.331, \c CSI-MeasConfig.
struct csi_meas_config {
  /// Pool of \c NZP-CSI-RS-Resource which can be referred to from \c NZP-CSI-RS-ResourceSet. Maximum size: 192.
  std::vector<nzp_csi_rs_resource> nzp_csi_rs_res_list;
  /// Pool of NZP-CSI-RS-ResourceSet which can be referred to from CSI-ResourceConfig or from MAC CEs. Maximum size: 64.
  std::vector<nzp_csi_rs_resource_set> nzp_csi_rs_res_set_list;
  /// Pool of CSI-IM-Resource which can be referred to from CSI-IM-ResourceSet. Maximum size: 32.
  std::vector<csi_im_resource> csi_im_res_list;
  /// Pool of CSI-IM-ResourceSet which can be referred to from CSI-ResourceConfig or from MAC CEs. Maximum size: 64.
  std::vector<csi_im_resource_set> csi_im_res_set_list;
  /// Pool of CSI-SSB-ResourceSet which can be referred to from CSI-ResourceConfig. Maximum size: 64.
  std::vector<csi_ssb_resource_set> csi_ssb_res_set_list;
  /// Configured CSI resource settings as specified in TS 38.214 clause 5.2.1.2. Maximum size: 112.
  std::vector<csi_resource_config> csi_res_cfg_list;
  /// Configured CSI report settings as specified in TS 38.214 clause 5.2.1.1. Maximum size: 48.
  std::vector<csi_report_config> csi_report_cfg_list;
  /// Size of CSI request field in DCI (bits). See TS 38.214, clause 5.2.1.5.1.
  optional<unsigned>                                        report_trigger_size;
  optional<csi_aperiodic_trigger_state_list>                aperiodic_trigger_state_list;
  optional<csi_semi_persistent_on_pusch_trigger_state_list> semi_persistent_on_pusch_trigger_state_list;

  bool operator==(const csi_meas_config& rhs) const
  {
    return nzp_csi_rs_res_list == rhs.nzp_csi_rs_res_list && nzp_csi_rs_res_set_list == rhs.nzp_csi_rs_res_set_list &&
           csi_im_res_list == rhs.csi_im_res_list && csi_im_res_set_list == rhs.csi_im_res_set_list &&
           csi_ssb_res_set_list == rhs.csi_ssb_res_set_list && csi_res_cfg_list == rhs.csi_res_cfg_list &&
           csi_report_cfg_list == rhs.csi_report_cfg_list && report_trigger_size == rhs.report_trigger_size &&
           aperiodic_trigger_state_list == rhs.aperiodic_trigger_state_list &&
           semi_persistent_on_pusch_trigger_state_list == rhs.semi_persistent_on_pusch_trigger_state_list;
  }
  bool operator!=(const csi_meas_config& rhs) const { return !(rhs == *this); }
};

/// \brief Configuration of ZP-CSI-RS-Resource as per TS38.331.
struct zp_csi_rs_resource {
  /// ZP CSI-RS resource configuration ID.
  zp_csi_rs_res_id_t id;
  /// \brief OFDM symbol and subcarrier occupancy of the ZP-CSI-RS resource within a slot.
  csi_rs_resource_mapping res_mapping;
  /// Periodicity and slot offset for periodic/semi-persistent ZP-CSI-RS.
  optional<csi_resource_periodicity> period;
  optional<unsigned>                 offset;

  bool operator==(const zp_csi_rs_resource& other) const
  {
    return id == other.id && res_mapping == other.res_mapping && period == other.period && offset == other.offset;
  }
  bool operator!=(const zp_csi_rs_resource& other) const { return !(*this == other); }
};

/// \brief Configuration of ZP-CSI-RS-ResourceSet as per TS38.331.
struct zp_csi_rs_resource_set {
  static constexpr size_t MAX_NOF_ZP_CSI_RS_RESOURCES_PER_SET = 16;

  zp_csi_rs_res_set_id_t id;
  /// The list of ZP-CSI-RS-ResourceId identifying the ZP-CSI-RS-Resource elements belonging to this set.
  static_vector<zp_csi_rs_res_set_id_t, MAX_NOF_ZP_CSI_RS_RESOURCES_PER_SET> zp_csi_rs_res_list;

  bool operator==(const zp_csi_rs_resource_set& other) const
  {
    return id == other.id && zp_csi_rs_res_list == other.zp_csi_rs_res_list;
  }
  bool operator!=(const zp_csi_rs_resource_set& other) const { return !(*this == other); }
};

} // namespace srsran
