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

#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/alpha.h"

namespace srsgnb {

/// \brief Used to configure the UE specific PUSCH parameters applicable to a particular BWP.
/// \remark See TS 38.311, PUSCH-Config.
struct pusch_config {
  /// Type of transmission schemes for PUSCH.
  enum class tx_config { codebook, non_codebook };

  struct pusch_power_control {
    enum class tpc_accumulation { disabled };

    enum p0_pusch_alphaset_id : uint8_t {
      MIN_P0_PUSCH_ALPHASET_ID   = 0,
      MAX_P0_PUSCH_ALPHASET_ID   = 29,
      MAX_NOF_P0_PUSCH_ALPHASETS = 30
    };

    struct p0_pusch_alphaset {
      p0_pusch_alphaset_id id;
      /// P0 value for PUSCH with grant (except msg3) in steps of 1dB. When the field is absent the UE applies the value
      /// 0. See TS 38.213, clause 7.1. Values {-16..15}.
      optional<int> p0;
      /// alpha value for PUSCH with grant (except msg3). When the field is absent the UE applies the value 1. See
      /// TS 38.213, clause 7.1.
      optional<alpha> p0_pusch_alpha;
    };

    /// If enabled, UE applies TPC commands via accumulation. If not enabled, UE applies the TPC command without
    /// accumulation. If the field is absent, TPC accumulation is enabled.
    optional<tpc_accumulation> tpc_accum;
    /// Dedicated alpha value for msg3 PUSCH. When the field is absent the UE applies the value 1.
    optional<alpha> msg3_alpha;
    /// P0 value for UL grant-free/SPS based PUSCH. Value in dBm. Only even values (step size 2) allowed. See TS 38.213,
    /// clause 7.1. Values {-202..24}.
    optional<int> p0_nominal_without_grant;
    /// Configuration {p0-pusch, alpha} sets for PUSCH (except msg3), i.e. { {p0,alpha,index1}, {p0,alpha,index2},...}.
    /// When no set is configured, the UE uses the P0-nominal for msg3 PUSCH, P0-UE is set to 0 and alpha is set
    /// according to msg3-Alpha configured for msg3 PUSCH.
    static_vector<p0_pusch_alphaset, p0_pusch_alphaset_id::MAX_NOF_P0_PUSCH_ALPHASETS> p0_alphasets;
  };

  /// Identifier used to initalite data scrambling (c_init) for PUSCH. If the field is absent, the UE applies the
  /// physical cell ID. See TS 38.211, clause 6.3.1.1.
  optional<uint16_t> data_scrambling_id_pusch;
  /// UE uses codebook based or non-codebook based transmission (see TS 38.214, clause 6.1.1). If the field is
  /// absent, the UE transmits PUSCH on one antenna port.
  optional<tx_config> tx_cfg;
  /// DMRS configuration for PUSCH transmissions using PUSCH (chosen dynamically via
  /// PUSCH-TimeDomainResourceAllocation). Only the fields dmrs-Type, dmrs-AdditionalPosition and maxLength may be set
  /// differently for mapping type A and B. The field dmrs-UplinkForPUSCH-MappingTypeA applies to DCI format 0_1.
  optional<dmrs_uplink_config> pusch_mapping_type_a_dmrs;
  optional<dmrs_uplink_config> pusch_mapping_type_b_dmrs;
};

} // namespace srsgnb
