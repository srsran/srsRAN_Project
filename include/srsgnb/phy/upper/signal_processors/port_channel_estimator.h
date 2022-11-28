/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Channel estimator declaration.

#pragma once

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/phy/support/resource_grid.h"
#include "srsgnb/phy/upper/channel_estimation.h"
#include "srsgnb/phy/upper/dmrs_mapping.h"
#include "srsgnb/phy/upper/re_measurement.h"
#include "srsgnb/ran/subcarrier_spacing.h"

namespace srsgnb {

/// Container for DM-RS symbols.
using dmrs_symbol_list = static_re_measurement<cf_t,
                                               pusch_constants::MAX_NOF_DMRS_SUBC,
                                               pusch_constants::MAX_NOF_DMRS_SYMBOLS,
                                               pusch_constants::MAX_NOF_LAYERS>;

/// DM-RS-based channel estimator for one receive antenna port.
class port_channel_estimator
{
public:
  /// \brief DM-RS pattern to specify the position of DM-RS symbols in the resource grid.
  ///
  /// DM-RS symbols are transmitted with a specific subcarrier pattern that repeats at specific OFDM symbols within the
  /// slot. Therefore, the position of the DM-RS symbols can be univocally characterized by a time mask and a frequency
  /// mask: Resource element \f$(k, l)\f$ contains a DM-RS symbol only if element \f$k\f$ of the frequency mask and
  /// element \f$l\f$ of the time mask are set to true.
  struct layer_dmrs_pattern {
    /// Boolean mask to specify the OFDM symbols carrying DM-RS.
    std::array<bool, MAX_NSYMB_PER_SLOT> symbols = {};
    /// Boolean mask to specify the resource blocks carrying DM-RS.
    bounded_bitset<MAX_RB> rb_mask;
    /// Boolean mask to specify the resource blocks carrying DM-RS after the frequency hop.
    bounded_bitset<MAX_RB> rb_mask2;
    /// Symbol index within the slot in which the first hop occurs if it has a value.
    optional<unsigned> hopping_symbol_index;
    /// Boolean mask to specify the resource elements within the resource blocks carrying DM-RS symbols.
    std::array<bool, NRE> re_pattern = {};
  };

  /// Estimator configuration parameters.
  struct configuration {
    /// Subcarrier spacing of the estimated channel.
    subcarrier_spacing scs = subcarrier_spacing::kHz15;
    /// Cyclic prefix.
    cyclic_prefix cp;
    /// First OFDM symbol within the slot for which the channel should be estimated.
    unsigned first_symbol;
    /// Number of OFDM symbols for which the channel should be estimated.
    unsigned nof_symbols;
    /// DM-RS pattern for each layer. The number of elements, indicates the number of layers.
    static_vector<layer_dmrs_pattern, pusch_constants::MAX_NOF_LAYERS> dmrs_pattern;
    /// List of receive ports.
    static_vector<uint8_t, DMRS_MAX_NPORTS> rx_ports;
    /// \brief Ratio of data EPRE to DM-RS EPRE in dB.
    ///
    /// Should be equal to zero for PUCCH and equal to parameter \f$\beta_{\textup{DMRS}}\f$ (see TS38.214
    /// Section 6.2.2) for PUSCH.
    float beta_dmrs = 0.0F;
  };

  /// Default destructor.
  virtual ~port_channel_estimator() = default;

  /// \brief Computes the channel estimate.
  ///
  /// Estimates the channel to the current receive antenna port for all transmit layers.
  /// \param[out] estimate The estimated channel coefficients for all resource elements, all OFDM symbols, all receive
  ///                      ports and all transmit layers.
  /// \param[in]  grid     The frequency&ndash;time resource grid.
  /// \param[in]  port     Receive antenna port the estimation refers to.
  /// \param[in]  pilots   DM-RS symbols (a.k.a. pilots). For each layer, symbols are listed by RE first and then by
  ///                      OFDM symbol.
  /// \param[in]  pattern  Pattern with the positions of the DM-RS symbols.
  /// \param[in]  cfg      Estimator configuration.
  virtual void compute(channel_estimate&           estimate,
                       const resource_grid_reader& grid,
                       unsigned                    port,
                       const dmrs_symbol_list&     pilots,
                       const configuration&        cfg) = 0;
};

} // namespace srsgnb
