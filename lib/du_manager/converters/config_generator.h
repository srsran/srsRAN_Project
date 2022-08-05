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

  std::vector<unsigned> searchspace0_idx_list;
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
