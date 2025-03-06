/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_uplane_fragment_size_calculator.h"
#include "srsran/ran/resource_block.h"

using namespace srsran;
using namespace ofh;

/// Builds and returns an index of the PRB data size lookup table with the given bitwidth.
static constexpr unsigned build_prb_data_size_lut_index(unsigned bitwidth)
{
  return units::bits(bitwidth * NOF_SUBCARRIERS_PER_RB * 2).round_up_to_bytes().value();
}

/// Returns the PRB data size in bytes for the given compression parameters.
static units::bytes get_prb_data_size(const ru_compression_params& compr_params)
{
  static constexpr std::array<unsigned, 17> prb_data_size = {build_prb_data_size_lut_index(0),
                                                             build_prb_data_size_lut_index(1),
                                                             build_prb_data_size_lut_index(2),
                                                             build_prb_data_size_lut_index(3),
                                                             build_prb_data_size_lut_index(4),
                                                             build_prb_data_size_lut_index(5),
                                                             build_prb_data_size_lut_index(6),
                                                             build_prb_data_size_lut_index(7),
                                                             build_prb_data_size_lut_index(8),
                                                             build_prb_data_size_lut_index(9),
                                                             build_prb_data_size_lut_index(10),
                                                             build_prb_data_size_lut_index(11),
                                                             build_prb_data_size_lut_index(12),
                                                             build_prb_data_size_lut_index(13),
                                                             build_prb_data_size_lut_index(14),
                                                             build_prb_data_size_lut_index(15),
                                                             build_prb_data_size_lut_index(16)};

  srsran_assert(compr_params.data_width < prb_data_size.size(), "Invalid data width");

  return units::bytes((compr_params.type == compression_type::none || compr_params.type == compression_type::modulation)
                          ? prb_data_size[compr_params.data_width]
                          : prb_data_size[compr_params.data_width] + 1U);
}

ofh_uplane_fragment_size_calculator::ofh_uplane_fragment_size_calculator(unsigned                     start_prb_,
                                                                         unsigned                     nof_prbs_,
                                                                         const ru_compression_params& compr_params) :
  start_prb(start_prb_),
  nof_prbs(nof_prbs_),
  prb_size(get_prb_data_size(compr_params)),
  next_fragment_start_prb_index(start_prb_)
{
}

bool ofh_uplane_fragment_size_calculator::calculate_fragment_size(unsigned& fragment_start_prb,
                                                                  unsigned& fragment_nof_prbs,
                                                                  unsigned  frame_size)
{
  units::bytes frame_sz(frame_size);

  if (frame_sz.value() < prb_size.value()) {
    fragment_start_prb = next_fragment_start_prb_index;
    fragment_nof_prbs  = 0;

    return false;
  }

  unsigned nof_prbs_fit_in_frame = frame_sz.value() / prb_size.value();

  fragment_start_prb      = next_fragment_start_prb_index;
  unsigned remaining_prbs = nof_prbs - (next_fragment_start_prb_index - start_prb);

  // Last fragment.
  if (nof_prbs_fit_in_frame >= remaining_prbs) {
    fragment_nof_prbs = remaining_prbs;
    next_fragment_start_prb_index += remaining_prbs;

    return true;
  }

  fragment_nof_prbs = nof_prbs_fit_in_frame;
  next_fragment_start_prb_index += nof_prbs_fit_in_frame;

  return false;
}

unsigned ofh_uplane_fragment_size_calculator::calculate_nof_segments(units::bytes                 frame_size,
                                                                     unsigned                     nof_prbs,
                                                                     const ru_compression_params& compr_params,
                                                                     units::bytes                 headers_size)
{
  units::bytes prb_size              = get_prb_data_size(compr_params);
  units::bytes frame_size_data       = frame_size - headers_size;
  unsigned     nof_prbs_fit_in_frame = frame_size_data.value() / prb_size.value();

  return std::ceil(static_cast<float>(nof_prbs) / nof_prbs_fit_in_frame);
}
