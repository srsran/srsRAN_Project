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
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/scheduler/config/bwp_configuration.h"
#include "ssb_configuration.h"
#include <cstdint>

namespace srsgnb {

/// \brief Associates one or two DL reference signals with a corresponding quasi-colocation (QCL) type.
/// \remark See TS 38.331, TCI-State.
struct ptrs_uplink_config {
  /// \brief Configuration of UL PTRS without transform precoder (with CP-OFDM).
  struct transform_precoder_disabled {
    /// \brief The maximum number of UL PTRS ports for CP-OFDM. See TS 38.214, clause 6.2.3.1.
    enum class max_nof_ports { n1, n2 };
    /// \brief Indicates the subcarrier offset for UL PTRS for CP-OFDM. See TS 38.211, clause 6.4.1.2.2.
    enum class resource_element_offset { offset01, offset10, offset11 };
    /// \brief UL PTRS power boosting factor per PTRS port. See TS 38.214, clause 6.1, table 6.2.3.1.3.
    enum class ptrs_power { p00, p01, p10, p11 };

    /// Presence and Frequency density of UL PT-RS for CP-OFDM waveform as a function of scheduled BW. If the field is
    /// absent, the UE uses K_PT-RS = 2. See TS 38.214, clause 6.1. Values {1..276}.
    static_vector<uint16_t, 2> f_density;
    /// Presence and Time density of UL PT-RS for CP-OFDM waveform as a function of MCS. If the field is absent, the UE
    /// uses L_PT-RS = 1. See TS 38.214, clause 6.1. Values {0..29}.
    static_vector<uint8_t, 3> t_density;
    max_nof_ports             max_ports;
    /// If the field is absent, the UE applies the value offset00.
    optional<resource_element_offset> res_elem_offset;
    ptrs_power                        power;
  };

  /// \brief Configuration of UL PTRS with transform precoder (DFT-S-OFDM).
  struct transform_precoder_enabled {
    /// \brief Time density (OFDM symbol level) of PT-RS for DFT-s-OFDM.
    enum class time_density_transform_precoding { d2 };

    /// Sample density of PT-RS for DFT-s-OFDM, pre-DFT, indicating a set of thresholds T={NRBn, n=0,1,2,3,4}, that
    /// indicates dependency between presence of PT-RS and scheduled BW and the values of X and K the UE should use
    /// depending on the scheduled BW. See TS 38.214, clause 6.1, table 6.2.3.2-1. Value {1..276}.
    uint16_t sampl_density;
    /// If the field is absent, the UE applies value d1.
    optional<time_density_transform_precoding> t_density_trans_precoding;
  };

  optional<transform_precoder_disabled> trans_precoder_disabled;
  optional<transform_precoder_enabled>  trans_precoder_enabled;
};

} // namespace srsgnb
