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

#include "ssb_configuration.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/du_types.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include <cstdint>

namespace srsran {

/// \brief Associates one or two DL reference signals with a corresponding quasi-colocation (QCL) type.
/// \remark See TS 38.331, TCI-State.
struct ptrs_uplink_config {
  /// \brief Configuration of UL PTRS without transform precoder (with CP-OFDM).
  struct transform_precoder_disabled {
    static constexpr uint8_t max_nof_ul_ptrs_freq_density = 2;
    static constexpr uint8_t max_nof_ul_ptrs_time_density = 3;

    /// \brief The maximum number of UL PTRS ports for CP-OFDM. See TS 38.214, clause 6.2.3.1.
    enum class max_nof_ports { n1, n2 };
    /// \brief Indicates the subcarrier offset for UL PTRS for CP-OFDM. See TS 38.211, clause 6.4.1.2.2.
    enum class resource_element_offset { offset01, offset10, offset11, not_set };
    /// \brief UL PTRS power boosting factor per PTRS port. See TS 38.214, clause 6.1, table 6.2.3.1.3.
    enum class ptrs_power { p00, p01, p10, p11 };

    /// Presence and Frequency density of UL PT-RS for CP-OFDM waveform as a function of scheduled BW. If the field is
    /// absent, the UE uses K_PT-RS = 2. See TS 38.214, clause 6.1. Values {1..276}.
    static_vector<uint16_t, max_nof_ul_ptrs_freq_density> f_density;
    /// Presence and Time density of UL PT-RS for CP-OFDM waveform as a function of MCS. If the field is absent, the UE
    /// uses L_PT-RS = 1. See TS 38.214, clause 6.1. Values {0..29}.
    static_vector<uint8_t, max_nof_ul_ptrs_time_density> t_density;
    max_nof_ports                                        max_ports;
    /// If the field is absent, the UE applies the value offset00.
    resource_element_offset res_elem_offset{resource_element_offset::not_set};
    ptrs_power              power;

    bool operator==(const transform_precoder_disabled& rhs) const
    {
      return f_density == rhs.f_density && t_density == rhs.t_density && max_ports == rhs.max_ports &&
             res_elem_offset == rhs.res_elem_offset && power == rhs.power;
    }
    bool operator!=(const transform_precoder_disabled& rhs) const { return !(rhs == *this); }
  };

  /// \brief Configuration of UL PTRS with transform precoder (DFT-S-OFDM).
  struct transform_precoder_enabled {
    static constexpr uint8_t max_nof_ptrs_dft_ofdm_sample_density = 5;

    /// Sample density of PT-RS for DFT-s-OFDM, pre-DFT, indicating a set of thresholds T={NRBn, n=0,1,2,3,4}, that
    /// indicates dependency between presence of PT-RS and scheduled BW and the values of X and K the UE should use
    /// depending on the scheduled BW. See TS 38.214, clause 6.1, table 6.2.3.2-1. Value {1..276}.
    static_vector<uint16_t, max_nof_ptrs_dft_ofdm_sample_density> sampl_density;
    /// Time density (OFDM symbol level) of PT-RS for DFT-s-OFDM.
    /// Mapping to NR RRC: If true, the field is present in NR RRC indicating value d2 to UE. If false, the field is
    /// absent in NR RRC and the UE applies value d1.
    bool is_t_density_trans_precoding_d2{false};

    bool operator==(const transform_precoder_enabled& rhs) const
    {
      return sampl_density == rhs.sampl_density &&
             is_t_density_trans_precoding_d2 == rhs.is_t_density_trans_precoding_d2;
    }
    bool operator!=(const transform_precoder_enabled& rhs) const { return !(rhs == *this); }
  };

  optional<transform_precoder_disabled> trans_precoder_disabled;
  optional<transform_precoder_enabled>  trans_precoder_enabled;

  bool operator==(const ptrs_uplink_config& rhs) const
  {
    return trans_precoder_disabled == rhs.trans_precoder_disabled &&
           trans_precoder_enabled == rhs.trans_precoder_enabled;
  }
  bool operator!=(const ptrs_uplink_config& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
