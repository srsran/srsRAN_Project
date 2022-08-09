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

#include "srsgnb/du/du_cell_config.h"

namespace srsgnb {

const unsigned NOF_SSB_SUBCARRIERS        = NOF_SSB_PRBS * NOF_SUBCARRIERS_PER_RB;
const unsigned KHZ_TO_HZ                  = 1000;
const double   HZ_TO_KHZ                  = 1e-3;
const unsigned MIN_ARFCN_3_GHZ_24_5_GHZ   = 600000;
const unsigned MIN_ARFCN_24_5_GHZ_100_GHZ = 2016667;
const double   N_SIZE_SYNC_RASTER_1_HZ    = 1200e3;
const unsigned N_UB_SYNC_RASTER_1         = 2500;
const unsigned N_UB_SYNC_RASTER_2         = 14757;
const double   M_SIZE_SYNC_RASTER_1_HZ    = 50e3;
const double   N_SIZE_SYNC_RASTER_2_HZ    = 1440e3;

// This expressed in Hz.
const double N_REF_OFFSET_3_GHZ_24_5_GHZ = 3e9;

namespace srs_du {

struct du_ssb_sib1_location {
  bool                  is_valid;
  ssb_offset_to_pointA  offset_to_point_A;
  ssb_subcarrier_offset k_ssb;
  unsigned              coreset0_idx;
  unsigned              searchspace0_idx;
};

struct ssb_freq_location {
  bool                  is_valid;
  ssb_offset_to_pointA  offset_to_point_A;
  ssb_subcarrier_offset k_ssb;
};

class du_config_generator
{
public:
  explicit du_config_generator(unsigned           dl_arfcn,
                               unsigned           nr_band,
                               unsigned           n_rbs_,
                               subcarrier_spacing scs_common,
                               subcarrier_spacing scs_ssb);

  ssb_freq_location get_next_ssb();
  unsigned          get_ssb_first_symbol() const { return ssb_first_symbol; };
  unsigned          get_max_ss0_idx() const { return max_ss0_idx; };

private:
  unsigned find_M_raster();
  double   get_ss_ref_hz(unsigned N, unsigned M);

  /// Parameters that are passed to the constructor.
  const unsigned           dl_arfcn;
  const unsigned           nr_band;
  const unsigned           n_rbs;
  const subcarrier_spacing scs_common;
  const subcarrier_spacing scs_ssb;

  /// Parameters that are initialized by the constructor.
  double point_A_hz;
  double f_ref_hz;
  /// Band upper-bound, we consider the last subcarrier of the last CRB as band upper-bound.
  double           bw_ub_hz;
  ssb_pattern_case ssb_case;
  unsigned         ssb_first_symbol;
  uint8_t          max_ss0_idx;

  /// This is the \f$N\f$ parameter in the synchronization raster.
  unsigned N_raster;
  /// This is the \f$M\f$ parameter in the synchronization raster.
  unsigned M_raster;
};

/// \brief Derive packed cell MIB from DU cell configuration.
/// \param[in] du_cfg DU Cell Configuration.
/// \return byte buffer with packed cell MIB.
du_ssb_sib1_location get_ssb_sib1_freq_location(unsigned           dl_arfcn,
                                                unsigned           nr_band,
                                                unsigned           n_rbs,
                                                subcarrier_spacing scs_common,
                                                subcarrier_spacing scs_ssb);

} // namespace srs_du

} // namespace srsgnb
