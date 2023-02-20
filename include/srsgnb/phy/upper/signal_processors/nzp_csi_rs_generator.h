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

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/support/resource_grid.h"
#include "srsgnb/ran/csi_rs/csi_rs_types.h"
#include "srsgnb/ran/cyclic_prefix.h"
#include "srsgnb/ran/slot_point.h"

namespace srsran {

/// Describes a Non-Zero-Power CSI Reference Signal (NZP-CSI-RS) processor interface, in compliance with TS 38.211
/// Section 7.4.1.5.
class nzp_csi_rs_generator
{
public:
  /// Describes the required parameters to generate the NZP-CSI-RS signal, as described in TS 38.211 Section 7.4.1.5.
  struct config_t {
    /// \brief Slot context for sequence initialization.
    /// \remark This indicates the numerology, SFN and slot index within the frame.
    slot_point slot;
    /// Cyclic prefix configuration.
    cyclic_prefix cp;
    /// PRB where the CSI resource starts, related to the CRB 0.
    unsigned start_rb;
    /// Number of PRBs that the CSI spans.
    unsigned nof_rb;
    /// Row number of the CSI-RS location table, as defined in TS 38.211 Table 7.4.1.5.3-1.
    unsigned csi_rs_mapping_table_row;
    /// Frequency domain allocation references \f${k_0, k_1, ..., k_n}\f$.
    static_vector<unsigned, CSI_RS_MAX_NOF_K_INDEXES> freq_allocation_ref_idx;
    /// \brief The time domain location reference \f$l_0\f$.
    ///
    /// This reference is defined relative to the start of the slot, and takes values in the range {0, 1, ..., 13}.
    unsigned symbol_l0;
    /// \brief The time domain location reference \f$l_1\f$.
    ///
    /// This reference is defined relative to the start of the slot, and takes values in the range {2, 3, ..., 12}.
    unsigned symbol_l1;
    /// CDM configuration.
    csi_rs_cdm_type cdm;
    /// Frequency density configuration.
    csi_rs_freq_density_type freq_density;
    /// \brief CSI-RS Scrambling ID, as per TS 38.214, Section 5.2.2.3.1.
    ///
    /// Takes values in the range {0, 1, ..., 1023}.
    unsigned scrambling_id;
    /// Linear amplitude scaling factor.
    float amplitude;
    /// \brief Precoding Matrix Indicator.
    ///
    /// Set to 0 for no precoding.
    unsigned pmi;
    /// \brief Port indexes to map the signal.
    /// \remark This is so that the ports to be used can be arbitrarily ordered by higher layers.
    static_vector<uint8_t, MAX_PORTS> ports;
  };

  /// Default destructor.
  virtual ~nzp_csi_rs_generator() = default;

  /// \brief Generates and maps the NZP-CSI-RS, according to TS 38.211, Section 7.4.1.5.
  /// \param [out] grid Provides the destination resource grid.
  /// \param [in] config Provides the required configuration to generate and map the signal.
  virtual void map(resource_grid_writer& grid, const config_t& config) = 0;
};

/// Describes the NZP-CSI-RS generator configuration validator interface.
class nzp_csi_rs_configuration_validator
{
public:
  /// Default destructor.
  virtual ~nzp_csi_rs_configuration_validator() = default;

  /// \brief Validates NZP-CSI-RS generator configuration parameters.
  /// \return True if the parameters contained in \c config are supported, false otherwise.
  virtual bool is_valid(const nzp_csi_rs_generator::config_t& config) = 0;
};

} // namespace srsran
