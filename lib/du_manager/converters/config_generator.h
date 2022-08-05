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
