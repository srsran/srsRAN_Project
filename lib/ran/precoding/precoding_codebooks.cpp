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
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "srsran/adt/interval.h"
#include "srsran/support/math_utils.h"

using namespace srsran;

precoding_weight_matrix srsran::make_single_port()
{
  return make_one_layer_one_port(1, 0);
}

precoding_weight_matrix srsran::make_one_layer_one_port(unsigned nof_ports, unsigned selected_i_port)
{
  interval<unsigned, false> selected_i_port_range(0, nof_ports);
  srsran_assert(selected_i_port_range.contains(selected_i_port),
                "The given port identifier (i.e., {}) is out of the valid range {}",
                selected_i_port,
                selected_i_port_range);

  precoding_weight_matrix result(1, nof_ports);

  // Set weights per port.
  for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
    cf_t port_weight = (i_port == selected_i_port) ? 1.0F : 0.0F;
    result.set_coefficient(port_weight, 0, i_port);
  }

  return result;
}

precoding_weight_matrix srsran::make_one_layer_all_ports(unsigned nof_ports)
{
  interval<unsigned, true> nof_ports_range(1, precoding_constants::MAX_NOF_PORTS);
  srsran_assert(nof_ports_range.contains(nof_ports),
                "The number of ports (i.e., {}) is out of the valid range {}.",
                nof_ports,
                nof_ports_range);

  precoding_weight_matrix result(1, nof_ports);

  // Set normalized weights per port.
  cf_t weight = {1.0F / std::sqrt(static_cast<float>(nof_ports)), 0.0F};
  for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
    result.set_coefficient(weight, 0, i_port);
  }

  return result;
}

precoding_weight_matrix srsran::make_identity(unsigned nof_streams)
{
  static constexpr interval<unsigned, true> nof_streams_range(1, precoding_constants::MAX_NOF_LAYERS);

  srsran_assert(nof_streams_range.contains(nof_streams),
                "The number of streams (i.e., {}) is out of the valid range {}.",
                nof_streams,
                nof_streams_range);

  precoding_weight_matrix result(nof_streams, nof_streams);

  cf_t normalised_weight = 1.0F / std::sqrt(static_cast<float>(nof_streams));

  // Set weights per port.
  for (unsigned i_layer = 0; i_layer != nof_streams; ++i_layer) {
    for (unsigned i_port = 0; i_port != nof_streams; ++i_port) {
      cf_t weight = (i_layer == i_port) ? normalised_weight : 0.0F;
      result.set_coefficient(weight, i_layer, i_port);
    }
  }
  return result;
}

precoding_weight_matrix srsran::make_one_layer_two_ports(unsigned i_codebook)
{
  static constexpr interval<unsigned, true>           i_codebook_range(0, 3);
  static constexpr cf_t                               sqrt_1_2         = {M_SQRT1_2, 0};
  static constexpr cf_t                               j_sqrt_1_2       = {0, M_SQRT1_2};
  static constexpr cf_t                               minus_sqrt_1_2   = {-M_SQRT1_2, 0};
  static constexpr cf_t                               minus_j_sqrt_1_2 = {0, -M_SQRT1_2};
  static constexpr std::array<std::array<cf_t, 2>, 4> codebooks        = {
             {{sqrt_1_2, sqrt_1_2}, {sqrt_1_2, j_sqrt_1_2}, {sqrt_1_2, minus_sqrt_1_2}, {sqrt_1_2, minus_j_sqrt_1_2}}};

  srsran_assert(i_codebook_range.contains(i_codebook),
                "The given codebook identifier (i.e., {}) is out of the range {}",
                i_codebook,
                i_codebook_range);

  precoding_weight_matrix result(1, 2);

  // Select codebook.
  span<const cf_t> codebook = codebooks[i_codebook];

  // Set weights per port.
  for (unsigned i_port = 0; i_port != 2; ++i_port) {
    result.set_coefficient(codebook[i_port], 0, i_port);
  }

  return result;
}

precoding_weight_matrix srsran::make_two_layer_two_ports(unsigned i_codebook)
{
  static constexpr interval<unsigned, true>           i_codebook_range(0, 1);
  static constexpr cf_t                               dot_five         = {0.5F, 0.0F};
  static constexpr cf_t                               minus_dot_five   = {-0.5F, 0.0F};
  static constexpr cf_t                               j_dot_five       = {0.0F, 0.5F};
  static constexpr cf_t                               minus_j_dot_five = {0.0F, -0.5F};
  static constexpr std::array<std::array<cf_t, 2>, 2> codebook0 = {{{dot_five, dot_five}, {dot_five, minus_dot_five}}};
  static constexpr std::array<std::array<cf_t, 2>, 2> codebook1 = {
      {{dot_five, dot_five}, {j_dot_five, minus_j_dot_five}}};

  srsran_assert(i_codebook_range.contains(i_codebook),
                "The given codebook identifier (i.e., {}) is out of the range {}",
                i_codebook,
                i_codebook_range);

  precoding_weight_matrix result(2, 2);

  // Select codebook.
  const std::array<std::array<cf_t, 2>, 2>& codebook = (i_codebook == 0) ? codebook0 : codebook1;

  // Set weights per port.
  for (unsigned i_layer = 0; i_layer != 2; ++i_layer) {
    span<const cf_t> codebook_layer = codebook[i_layer];
    for (unsigned i_port = 0; i_port != 2; ++i_port) {
      result.set_coefficient(codebook_layer[i_port], i_layer, i_port);
    }
  }

  return result;
}

// Creates horizontal beam coefficients, given a phase increment between adjacent elements, an initial phase, and a
// scaling factor.
static void create_horizontal_beam(span<cf_t> beam, float phase_increment_rad, float initial_phase_rad, float scaling)
{
  // Number of beam coefficients.
  unsigned nof_elements = beam.size();

  // Phasor to increment the phase of each beam coefficient.
  cf_t phase_increment = std::polar(1.0F, phase_increment_rad);

  // Initial beam coefficient.
  cf_t beam_coefficient = std::polar(scaling, initial_phase_rad);

  for (unsigned i_element = 0; i_element != nof_elements; ++i_element) {
    beam[i_element] = beam_coefficient;

    // Increment phase;
    beam_coefficient *= phase_increment;
  }
}

precoding_weight_matrix srsran::make_one_layer_four_ports_type1_sp_mode1(unsigned beam_azimuth_id,
                                                                         unsigned pol_shift_id)
{
  // Beam oversampling factor in the horizontal plane, from TS38.214 Section 5.2.2.2.1.
  static constexpr unsigned O_1 = 4;
  // Number of cross-polarized antenna elements, from TS38.214 Section 5.2.2.2.1.
  static constexpr unsigned N_1 = 2;
  // Number of possible horizontal beams to choose from.
  static constexpr unsigned nof_beams = O_1 * N_1;
  // Number of possible polarization phase shifts to choose from.
  static constexpr unsigned nof_pol_shifts = 4;

  static constexpr interval<unsigned, false> beam_azimuth_range(0, nof_beams);
  static constexpr interval<unsigned, false> pol_phase_shift_range(0, nof_pol_shifts);

  srsran_assert(beam_azimuth_range.contains(beam_azimuth_id),
                "The given beam azimuth identifier i1_1 (i.e., {}) is out of the range {}",
                beam_azimuth_id,
                beam_azimuth_range);

  srsran_assert(pol_phase_shift_range.contains(pol_shift_id),
                "The given polarization phase shift i2 (i.e., {}) is out of the range {}",
                pol_shift_id,
                pol_phase_shift_range);

  // Precoding weight matrix for one layer mapped into four antenna ports.
  precoding_weight_matrix result(1, 4);

  // Power normalization factor. It is equal to the square root of the number of antenna ports.
  static constexpr float scaling = 0.5F;

  // Phase increment for each beam coefficient. This defines the direction of the beam in the horizontal plane.
  float phase_increment_rad = 2.0F * M_PI * static_cast<float>(beam_azimuth_id) / static_cast<float>(nof_beams);
  // Polarization phase shift. This defines the relative phase between the cross-polarized antenna elements.
  float pol_phase_shift_rad = M_PI_2 * static_cast<float>(pol_shift_id);

  std::array<cf_t, N_1> beam;

  // Beam for the first polarization.
  create_horizontal_beam(beam, phase_increment_rad, 0.0F, scaling);

  // Set weights for the first polarization.
  result.set_coefficient(beam[0], 0, 0);
  result.set_coefficient(beam[1], 0, 1);

  // Beam for the second polarization.
  create_horizontal_beam(beam, phase_increment_rad, pol_phase_shift_rad, scaling);

  // Set weights for the second polarization.
  result.set_coefficient(beam[0], 0, 2);
  result.set_coefficient(beam[1], 0, 3);

  return result;
}

precoding_weight_matrix srsran::make_two_layer_four_ports_type1_sp_mode1(unsigned beam_azimuth_id,
                                                                         unsigned beam_offset_id,
                                                                         unsigned pol_shift_id)
{
  // Beam oversampling factor in the horizontal plane, from TS38.214 Section 5.2.2.2.1.
  static constexpr unsigned O_1 = 4;
  // Number of cross-polarized antenna elements, from TS38.214 Section 5.2.2.2.1.
  static constexpr unsigned N_1 = 2;
  // Number of possible horizontal beams to choose from.
  static constexpr unsigned nof_beams = O_1 * N_1;
  // Number of possible polarization phase shifts to choose from.
  static constexpr unsigned nof_pol_shifts = 2;

  static constexpr interval<unsigned, false> beam_azimuth_range(0, nof_beams);
  static constexpr interval<unsigned, false> beam_offset_range(0, 2);
  static constexpr interval<unsigned, false> pol_phase_shift_range(0, nof_pol_shifts);

  srsran_assert(beam_azimuth_range.contains(beam_azimuth_id),
                "The given beam azimuth identifier i1_1 (i.e., {}) is out of the range {}",
                beam_azimuth_id,
                beam_azimuth_range);

  srsran_assert(beam_offset_range.contains(beam_offset_id),
                "The given beam offset identifier i1_3 (i.e., {}) is out of the range {}",
                beam_offset_id,
                beam_offset_range);

  srsran_assert(pol_phase_shift_range.contains(pol_shift_id),
                "The given polarization phase shift i2 (i.e., {}) is out of the range {}",
                pol_shift_id,
                pol_phase_shift_range);

  // Precoding weight matrix for two layers mapped into four antenna ports.
  precoding_weight_matrix result(2, 4);

  // Power normalization factor. It is equal to the square root of the number of ports times the number of layers.
  static constexpr float scaling = 0.5F * M_SQRT1_2;

  // Select k1 as per TS38.214 Section 5.2.2.2.1-3.
  unsigned k1 = (beam_offset_id == 0) ? 0 : O_1;

  // Phase increment for each beam coefficient. This defines the direction of the beam in the horizontal plane.
  float phase_increment_rad = static_cast<float>(beam_azimuth_id) * 2.0F * M_PI / static_cast<float>(nof_beams);
  // Polarization phase shift. This defines the relative phase between the cross-polarized antenna elements.
  float pol_phase_shift_rad = M_PI_2 * static_cast<float>(pol_shift_id);

  std::array<cf_t, N_1> beam;

  // Layer 0.

  // Beam for the first polarization.
  create_horizontal_beam(beam, phase_increment_rad, 0.0F, scaling);

  // Set weights for the first polarization.
  result.set_coefficient(beam[0], 0, 0);
  result.set_coefficient(beam[1], 0, 1);

  // Beam for the second polarization.
  create_horizontal_beam(beam, phase_increment_rad, pol_phase_shift_rad, scaling);

  // Set weights for the second polarization.
  result.set_coefficient(beam[0], 0, 2);
  result.set_coefficient(beam[1], 0, 3);

  // Layer 1. Recalculate phase increment for adjacent beam antenna elements.
  phase_increment_rad = static_cast<float>(beam_azimuth_id + k1) * 2.0F * M_PI / static_cast<float>(nof_beams);

  // Beam for the first polarization.
  create_horizontal_beam(beam, phase_increment_rad, 0.0F, scaling);

  // Set weights for the first polarization.
  result.set_coefficient(beam[0], 1, 0);
  result.set_coefficient(beam[1], 1, 1);

  // Beam for the second polarization.
  create_horizontal_beam(beam, phase_increment_rad, pol_phase_shift_rad + M_PI, scaling);

  // Set weights for the second polarization.
  result.set_coefficient(beam[0], 1, 2);
  result.set_coefficient(beam[1], 1, 3);

  return result;
}

precoding_weight_matrix srsran::make_three_layer_four_ports_type1_sp(unsigned beam_azimuth_id, unsigned pol_shift_id)
{
  // Beam oversampling factor in the horizontal plane, from TS38.214 Section 5.2.2.2.1.
  static constexpr unsigned O_1 = 4;
  // Number of cross-polarized antenna elements, from TS38.214 Section 5.2.2.2.1.
  static constexpr unsigned N_1 = 2;
  // Number of possible horizontal beams to choose from.
  static constexpr unsigned nof_beams = O_1 * N_1;
  // Number of possible polarization phase shifts to choose from.
  static constexpr unsigned nof_pol_shifts = 2;

  static constexpr interval<unsigned, false> beam_azimuth_range(0, nof_beams);
  static constexpr interval<unsigned, false> pol_phase_shift_range(0, nof_pol_shifts);

  srsran_assert(beam_azimuth_range.contains(beam_azimuth_id),
                "The given beam azimuth identifier i1_1 (i.e., {}) is out of the range {}",
                beam_azimuth_id,
                beam_azimuth_range);

  srsran_assert(pol_phase_shift_range.contains(pol_shift_id),
                "The given polarization phase shift i2 (i.e., {}) is out of the range {}",
                pol_shift_id,
                pol_phase_shift_range);

  // Precoding weight matrix for three layers mapped into four antenna ports.
  precoding_weight_matrix result(3, 4);

  // Power normalization factor. It is equal to the square root of the number of ports times the number of layers.
  float scaling = 1.0F / std::sqrt(12.0F);

  // Select k1 as per TS38.214 Section 5.2.2.2.1-4.
  static constexpr unsigned k1 = O_1;

  // Phase increment for each beam coefficient. This defines the direction of the beam in the horizontal plane.
  float phase_increment_rad = static_cast<float>(beam_azimuth_id) * 2.0F * M_PI / static_cast<float>(nof_beams);
  // Polarization phase shift. This defines the relative phase between the cross-polarized antenna elements.
  float pol_phase_shift_rad = M_PI_2 * static_cast<float>(pol_shift_id);

  std::array<cf_t, N_1> beam;

  // Layer 0.

  // Beam for the first polarization.
  create_horizontal_beam(beam, phase_increment_rad, 0.0F, scaling);

  // Set weights for the first polarization.
  result.set_coefficient(beam[0], 0, 0);
  result.set_coefficient(beam[1], 0, 1);

  // Beam for the second polarization.
  create_horizontal_beam(beam, phase_increment_rad, pol_phase_shift_rad, scaling);

  // Set weights for the second polarization.
  result.set_coefficient(beam[0], 0, 2);
  result.set_coefficient(beam[1], 0, 3);

  // Layer 2. The phase increment for adjacent beam antenna elements is the same as layer 0.

  // Beam for the first polarization.
  create_horizontal_beam(beam, phase_increment_rad, 0.0F, scaling);

  // Set weights for the first polarization.
  result.set_coefficient(beam[0], 2, 0);
  result.set_coefficient(beam[1], 2, 1);

  // Beam for the second polarization.
  create_horizontal_beam(beam, phase_increment_rad, pol_phase_shift_rad + M_PI, scaling);

  // Set weights for the second polarization.
  result.set_coefficient(beam[0], 2, 2);
  result.set_coefficient(beam[1], 2, 3);

  // Layer 1. Recalculate phase increment for adjacent beam antenna elements.
  phase_increment_rad = static_cast<float>(beam_azimuth_id + k1) * 2.0F * M_PI / static_cast<float>(nof_beams);

  // Beam for the first polarization.
  create_horizontal_beam(beam, phase_increment_rad, 0.0F, scaling);

  // Set weights for the first polarization.
  result.set_coefficient(beam[0], 1, 0);
  result.set_coefficient(beam[1], 1, 1);

  // Beam for the second polarization.
  create_horizontal_beam(beam, phase_increment_rad, pol_phase_shift_rad, scaling);

  // Set weights for the second polarization.
  result.set_coefficient(beam[0], 1, 2);
  result.set_coefficient(beam[1], 1, 3);

  return result;
}

precoding_weight_matrix srsran::make_four_layer_four_ports_type1_sp(unsigned beam_azimuth_id, unsigned pol_shift_id)
{
  // Beam oversampling factor in the horizontal plane, from TS38.214 Section 5.2.2.2.1.
  static constexpr unsigned O_1 = 4;
  // Number of cross-polarized antenna elements, from TS38.214 Section 5.2.2.2.1.
  static constexpr unsigned N_1 = 2;
  // Number of possible horizontal beams to choose from.
  static constexpr unsigned nof_beams = O_1 * N_1;
  // Number of possible polarization phase shifts to choose from.
  static constexpr unsigned nof_pol_shifts = 2;

  static constexpr interval<unsigned, false> beam_azimuth_range(0, nof_beams);
  static constexpr interval<unsigned, false> pol_phase_shift_range(0, nof_pol_shifts);

  srsran_assert(beam_azimuth_range.contains(beam_azimuth_id),
                "The given beam azimuth identifier i1_1 (i.e., {}) is out of the range {}",
                beam_azimuth_id,
                beam_azimuth_range);

  srsran_assert(pol_phase_shift_range.contains(pol_shift_id),
                "The given polarization phase shift i2 (i.e., {}) is out of the range {}",
                pol_shift_id,
                pol_phase_shift_range);

  // Precoding weight matrix for three layers mapped into four antenna ports.
  precoding_weight_matrix result(4, 4);

  // Power normalization factor. It is equal to the square root of the number of ports times the number of layers.
  static constexpr float scaling = 1.0F / 4.0F;

  // Select k1 as per TS38.214 Section 5.2.2.2.1-4.
  static constexpr unsigned k1 = O_1;

  // Phase increment for each beam coefficient. This defines the direction of the beam in the horizontal plane.
  float phase_increment_rad = static_cast<float>(beam_azimuth_id) * 2.0F * M_PI / static_cast<float>(nof_beams);
  // Polarization phase shift. This defines the relative phase between the cross-polarized antenna elements.
  float pol_phase_shift_rad = M_PI_2 * static_cast<float>(pol_shift_id);

  std::array<cf_t, N_1> beam;

  // Layer 0.

  // Beam for the first polarization.
  create_horizontal_beam(beam, phase_increment_rad, 0.0F, scaling);

  // Set weights for the first polarization.
  result.set_coefficient(beam[0], 0, 0);
  result.set_coefficient(beam[1], 0, 1);

  // Beam for the second polarization.
  create_horizontal_beam(beam, phase_increment_rad, pol_phase_shift_rad, scaling);

  // Set weights for the second polarization.
  result.set_coefficient(beam[0], 0, 2);
  result.set_coefficient(beam[1], 0, 3);

  // Layer 2. The phase increment for adjacent beam antenna elements is the same as layer 0.

  // Beam for the first polarization.
  create_horizontal_beam(beam, phase_increment_rad, 0.0F, scaling);

  // Set weights for the first polarization.
  result.set_coefficient(beam[0], 2, 0);
  result.set_coefficient(beam[1], 2, 1);

  // Beam for the second polarization.
  create_horizontal_beam(beam, phase_increment_rad, pol_phase_shift_rad + M_PI, scaling);

  // Set weights for the second polarization.
  result.set_coefficient(beam[0], 2, 2);
  result.set_coefficient(beam[1], 2, 3);

  // Layer 1. Recalculate phase increment for adjacent beam antenna elements.
  phase_increment_rad = static_cast<float>(beam_azimuth_id + k1) * 2.0F * M_PI / static_cast<float>(nof_beams);

  // Beam for the first polarization.
  create_horizontal_beam(beam, phase_increment_rad, 0.0F, scaling);

  // Set weights for the first polarization.
  result.set_coefficient(beam[0], 1, 0);
  result.set_coefficient(beam[1], 1, 1);

  // Beam for the second polarization.
  create_horizontal_beam(beam, phase_increment_rad, pol_phase_shift_rad, scaling);

  // Set weights for the second polarization.
  result.set_coefficient(beam[0], 1, 2);
  result.set_coefficient(beam[1], 1, 3);

  // Layer 3. The phase increment for adjacent beam antenna elements is the same as layer 1.

  // Beam for the first polarization.
  create_horizontal_beam(beam, phase_increment_rad, 0.0F, scaling);

  // Set weights for the first polarization.
  result.set_coefficient(beam[0], 3, 0);
  result.set_coefficient(beam[1], 3, 1);

  // Beam for the second polarization.
  create_horizontal_beam(beam, phase_increment_rad, pol_phase_shift_rad + M_PI, scaling);

  // Set weights for the second polarization.
  result.set_coefficient(beam[0], 3, 2);
  result.set_coefficient(beam[1], 3, 3);

  return result;
}
