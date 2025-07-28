/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

/// \file
/// \brief PUCCH detector definition for Format 1.

#include "pucch_detector_format1.h"
#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/tensor.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_detector.h"
#include "srsran/phy/upper/channel_processors/uci/uci_status.h"
#include "srsran/phy/upper/pucch_orthogonal_sequence.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/srsvec/add.h"
#include "srsran/srsvec/compare.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/dot_prod.h"
#include "srsran/srsvec/mean.h"
#include "srsran/srsvec/modulus_square.h"
#include "srsran/srsvec/prod.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/srsvec/subtract.h"
#include "srsran/support/math/math_utils.h"
#include "srsran/support/srsran_assert.h"
#include "fmt/base.h"
#include <array>
#include <cstdint>

using namespace srsran;

// Pre-generated orthogonal cover code.
static const pucch_orthogonal_sequence_format1 occ;

// The number of available initial cyclic shifts is the same as the number of RE per PRB, i.e., 12.
static constexpr unsigned NSHIFTS = pucch_constants::format1_initial_cyclic_shift_range.stop();

static void validate_config(const pucch_detector::format1_configuration& config,
                            const pucch_format1_map<unsigned>&           mux_nof_harq_ack)
{
  srsran_assert(config.start_symbol_index <= 10,
                "Setting {} as the first PUCCH symbol index, but only values between 0 and 10 are valid.",
                config.start_symbol_index);
  srsran_assert((config.nof_symbols >= 4) && (config.nof_symbols <= 14),
                "Requiring {} OFDM symbols for PUCCH, but only values between 4 and 14 are valid.",
                config.nof_symbols);
  srsran_assert(
      config.nof_symbols + config.start_symbol_index <= 14,
      "The sum of the starting OFDM symbol (i.e., {}) and the number of allocated symbols (i.e., {}) cannot exceed 14.",
      config.start_symbol_index,
      config.nof_symbols);
  srsran_assert(config.starting_prb <= 274,
                "Setting {} as the PRB allocated to PUCCH, but only values between 0 and 274 are valid.",
                config.starting_prb);
  srsran_assert(config.n_id <= 1023,
                "Initializing the pseudorandom generator with {}, but only values between 0 and 1023 are valid.",
                config.n_id);
  if (config.second_hop_prb.has_value()) {
    srsran_assert(*config.second_hop_prb <= 274,
                  "Setting {} as the PRB allocated to PUCCH after frequency hopping, but only values between 0 and 274 "
                  "are valid.",
                  *config.second_hop_prb);
  }
  unsigned symbol_occ_ratio = (config.second_hop_prb.has_value() ? 4U : 2U);
  for (const auto& this_pucch : mux_nof_harq_ack) {
    srsran_assert(this_pucch.initial_cyclic_shift <= 11,
                  "Setting {} as the initial cyclic shift, but only values between 0 and 11 are valid.",
                  this_pucch.initial_cyclic_shift);
    srsran_assert(this_pucch.time_domain_occ <= 6,
                  "Setting {} as the time-domain OCC index, but only values between 0 and 6 are valid.",
                  this_pucch.time_domain_occ);
    srsran_assert(config.time_domain_occ < config.nof_symbols / symbol_occ_ratio,
                  "Cannot have OCCI {} with {} allocated OFDM symbols and frequency hopping {}.",
                  config.time_domain_occ,
                  config.nof_symbols,
                  (config.second_hop_prb.has_value() ? "enabled" : "disabled"));
    srsran_assert(this_pucch.value <= 2, "At most two ACK bits - requested {}.", this_pucch.value);
  }
}

/// \brief Detects the transmitted symbol.
///
/// This function looks for the symbol (BPSK or QPSK) that maximizes the real part of the cross term between DM-RS and
/// signal components.
/// \param[out] bits        The bits corresponding to the detected symbol.
/// \param[in]  cross_term  The cross term between DM-RS and signal components of the PUCCH transmission.
/// \returns The real part of the cross term between DM-RS and signal components assuming the detected symbol.
/// \remark The order of the modulation is inferred from the length of \c bits.
static float detect_symbol(span<uint8_t> bits, cf_t cross_term)
{
  unsigned nof_bits = bits.size();
  srsran_assert(nof_bits <= 2, "PUCCH Format 1 supports max 2 bits, requested {}.", nof_bits);

  // Case BPSK.
  if (nof_bits == 1) {
    cf_t  rx_symbol    = cf_t(M_SQRT1_2, M_SQRT1_2);
    float metric_cross = std::real(rx_symbol * cross_term);
    if (metric_cross > 0) {
      bits[0] = 0;
    } else {
      rx_symbol    = -rx_symbol;
      metric_cross = -metric_cross;
      bits[0]      = 1;
    }
    return metric_cross;
  }

  // Case QPSK.
  cf_t  d1 = cf_t(M_SQRT1_2, M_SQRT1_2);
  float m1 = std::real(d1 * cross_term);
  cf_t  d2 = cf_t(M_SQRT1_2, -M_SQRT1_2);
  float m2 = std::real(d2 * cross_term);

  // First decide whether the most likely symbol is in quadrants 1-3 or 2-4.
  cf_t  rx_symbol    = d1;
  float metric_cross = m1;
  bits[0]            = 0;
  bits[1]            = 0;
  if (std::abs(m2) > std::abs(m1)) {
    rx_symbol    = d2;
    metric_cross = m2;
    bits[1]      = 1;
  }

  // Finally, pick the symbol that returns a positive real part.
  if (metric_cross < 0) {
    rx_symbol    = -rx_symbol;
    metric_cross = -metric_cross;
    for (auto& this_bit : bits) {
      this_bit = 1 - this_bit;
    }
  }

  return metric_cross;
}

const pucch_format1_map<pucch_detector::pucch_detection_result_csi>&
pucch_detector_format1::detect(const resource_grid_reader&                  grid,
                               const pucch_detector::format1_configuration& config,
                               const pucch_format1_map<unsigned>&           mux_nof_harq_ack)
{
  // Validate the configuration of all multiplexed transmissions.
  validate_config(config, mux_nof_harq_ack);

  // Set the boolean mask of OFDM symbols carrying DM-RS (every other allocated symbol starting from the first).
  dmrs_mask.reset();
  for (unsigned i_symbol = config.start_symbol_index, end_symbol = config.start_symbol_index + config.nof_symbols;
       i_symbol < end_symbol;
       i_symbol += 2) {
    dmrs_mask.set(i_symbol);
  }

  static_vector<hop_contribution_mux, NSHIFTS * pucch_constants::format1_time_domain_occ_range.stop()> metrics_hop0(0);
  static_vector<hop_contribution_mux, NSHIFTS * pucch_constants::format1_time_domain_occ_range.stop()> metrics_hop1(0);
  hop_contribution_common hop0           = process_hop(metrics_hop0, grid, config, mux_nof_harq_ack, /*i_hop=*/0);
  float                   epre           = hop0.epre_contribution;
  unsigned                epre_samples0  = hop0.nof_epre_samples;
  unsigned                epre_samples1  = 0;
  float                   noise_var      = hop0.noise_var_contribution;
  unsigned                noise_samples0 = hop0.nof_noise_samples;
  unsigned                noise_samples1 = 0;
  if (config.second_hop_prb.has_value()) {
    hop_contribution_common hop1 = process_hop(metrics_hop1, grid, config, mux_nof_harq_ack, /*i_hop =*/1);
    epre += hop1.epre_contribution;
    epre_samples1 = hop1.nof_epre_samples;
    noise_var += hop1.noise_var_contribution;
    noise_samples1 = hop1.nof_noise_samples;
  }

  epre /= (epre_samples0 + epre_samples1);
  noise_var /= (noise_samples0 + noise_samples1);

  // Different thresholds are needed depending on the number of antenna ports and on whether or not the intra-slot
  // frequency hopping is active.
  unsigned nof_contributions   = grid.get_nof_ports() * (config.second_hop_prb.has_value() ? 2 : 1);
  float    detection_threshold = 0.0F;

  switch (nof_contributions) {
    case 1:
      detection_threshold = 0.9F;
      break;
    case 2:
      detection_threshold = 3.0F;
      break;
    case 4:
      detection_threshold = 4.45F;
      break;
    case 8:
      detection_threshold = 6.95F;
      break;
    default:
      srsran_terminate("The PUCCH detector does not support more than 4 ports, configured {}.", grid.get_nof_ports());
  }

  // Clear previous results.
  mux_results.clear();

  // Loop through all the multiplexed transmissions - recall we need to read from metrics_hop0, metrics_hop1 and
  // mux_nof_harq_ack, which should all have active elements at the same (ics, occi) coordinates.
  // Results are written in mux_results.
  const auto* this_metric_hop1        = metrics_hop1.begin();
  auto        this_pucch_nof_harq_ack = mux_nof_harq_ack.begin();
  for (const auto& this_metric_hop0 : metrics_hop0) {
    unsigned ics                = (*this_pucch_nof_harq_ack).initial_cyclic_shift;
    unsigned occi               = (*this_pucch_nof_harq_ack).time_domain_occ;
    float    main_contribution  = this_metric_hop0.main_contribution;
    cf_t     cross_contribution = this_metric_hop0.cross_contribution;
    float    rsrp               = this_metric_hop0.rsrp_contribution;
    if (config.second_hop_prb.has_value()) {
      main_contribution += this_metric_hop1->main_contribution;
      cross_contribution += this_metric_hop1->cross_contribution;
      rsrp = (rsrp * noise_samples0 + this_metric_hop1->rsrp_contribution * noise_samples1) /
             (noise_samples0 + noise_samples1);
      ++this_metric_hop1;
    }

    channel_state_information csi;
    csi.set_epre(convert_power_to_dB(epre));
    csi.set_rsrp(convert_power_to_dB(rsrp));
    float sinr = std::isnormal(noise_var) ? rsrp / noise_var : std::numeric_limits<float>::quiet_NaN();
    csi.set_sinr_dB(channel_state_information::sinr_type::channel_estimator, convert_power_to_dB(sinr));
    csi.reset_time_alignment();

    unsigned nof_harq_ack = (*this_pucch_nof_harq_ack).value;
    mux_results.insert(ics,
                       occi,
                       {{.uci_message = pucch_uci_message(
                             {.nof_sr = 0U, .nof_harq_ack = nof_harq_ack, .nof_csi_part1 = 0U, .nof_csi_part2 = 0U}),
                         .detection_metric = std::numeric_limits<float>::quiet_NaN()},
                        csi});
    pucch_detector::pucch_detection_result& this_result = mux_results.get(ics, occi).detection_result;
    span<uint8_t>                           bits        = this_result.uci_message.get_harq_ack_bits();

    // If no HARQ ACK bits were transmitted, we still need to look at the SR "bit."
    std::array<uint8_t, 1> bit_sr;
    if (nof_harq_ack == 0) {
      bits = span<uint8_t>(bit_sr);
    }

    float detected_contribution = detect_symbol(bits, cross_contribution);
    float detection_metric      = (main_contribution + 2 * detected_contribution) / noise_var;

    bool is_detected = (detection_metric > detection_threshold);

    this_result.detection_metric = detection_metric / detection_threshold;
    this_result.uci_message.set_status(is_detected ? uci_status::valid : uci_status::invalid);

    // When the transmission is SR-only, the detected bit should be 0. If not, set the status to invalid.
    if (is_detected && (nof_harq_ack == 0)) {
      this_result.uci_message.set_status((bits[0] == 0) ? uci_status::valid : uci_status::invalid);
    }

    ++this_pucch_nof_harq_ack;
  }
  srsran_assert(this_metric_hop1 == metrics_hop1.end(), "Not all hop1 metrics have been processed.");
  srsran_assert(this_pucch_nof_harq_ack == mux_nof_harq_ack.end(),
                "Not all PUCCH in the input list have been processed.");

  return mux_results;
}

/// \brief Combines all the symbols of the same type (data or DM-RS) matching the OCC.
///
/// The combination is carried out for each port separately.
/// \param[out] out Resulting combined signal (12 entries per port).
/// \param[in]  in  PUCCH signal from the resource grid (either data or DM-RS), for all ports.
/// \param[in]  w   Orthogonal cover code.
static void
combine_symbols(static_tensor<2, cf_t, NSHIFTS * MAX_PORTS>&                                               out,
                const static_tensor<3, cf_t, MAX_PORTS * pucch_detector_format1::MAX_ALLOCATED_RE_F1 / 2>& in,
                span<const cf_t>                                                                           w)
{
  const auto& out_dims = out.get_dimensions_size();
  const auto& in_dims  = in.get_dimensions_size();

  srsran_assert(out_dims[0] == NSHIFTS,
                "The number of entries {} per port in the output tensor is not {}.",
                out_dims[0],
                NSHIFTS);
  srsran_assert(
      in_dims[0] == NSHIFTS, "The number of entries {} per port in the input tensor is not {}.", in_dims[0], NSHIFTS);
  unsigned n_ports = out_dims[1];
  srsran_assert(in_dims[2] == n_ports,
                "The number of antenna ports {} in the input tensor does not match that of the output tensor {}.",
                in_dims[2],
                n_ports);
  unsigned n_symbols = in_dims[1];
  srsran_assert(n_symbols == w.size(),
                "The number of OFDM symbols in the input tensor {} does not match the length of the w sequence {}.",
                n_symbols,
                w.size());

  std::array<cf_t, NSHIFTS> scaled;
  float                     normalizer = 1.0F / std::sqrt(static_cast<float>(n_symbols));
  for (unsigned i_port = 0; i_port != n_ports; ++i_port) {
    srsvec::sc_prod(out.get_view({i_port}), in.get_view({0, i_port}), w[0] * normalizer);
    for (unsigned i_symbol = 1; i_symbol != n_symbols; ++i_symbol) {
      srsvec::sc_prod(scaled, in.get_view({i_symbol, i_port}), w[i_symbol] * normalizer);
      srsvec::add(out.get_view({i_port}), scaled, out.get_view({i_port}));
    }
  }
}

/// \brief Estimates the channels for each port.
///
/// The input tensor should be the result of despreading the DM-RS both in time and in frequency domain. Therefore, and
/// for each port, each entry is an estimate of the channel for the corresponding ICS (this function must be called for
/// each OCC). The function computes the total channel energy across ports and discards those that are 10+ dB below the
/// strongest one.
/// \param[out] ch                     Estimated channels.
/// \param[out] rsrp                   Estimated RSRP.
/// \param[in]  in                     DM-RS samples after despreading in time and frequency domains (for one single OCC
/// index).
/// \param[in[  nof_dmrs_symbols_hop   Number of DM-RS symbols in the current hop.
static void estimate_channels(static_tensor<2, cf_t, NSHIFTS * MAX_PORTS>&       ch,
                              span<float>                                        rsrp,
                              const static_tensor<2, cf_t, NSHIFTS * MAX_PORTS>& in,
                              unsigned                                           nof_dmrs_symbols_hop)
{
  const auto& ch_dims = ch.get_dimensions_size();
  const auto& in_dims = in.get_dimensions_size();

  srsran_assert(
      ch_dims[0] == NSHIFTS, "The number of entries {} per port in the output tensor is not {}.", ch_dims[0], NSHIFTS);
  srsran_assert(
      in_dims[0] == NSHIFTS, "The number of entries {} per port in the input tensor is not {}.", in_dims[0], NSHIFTS);
  unsigned n_ports = ch_dims[1];
  srsran_assert(in_dims[1] == n_ports,
                "The number of antenna ports {} in the input tensor does not match that of the output tensor {}.",
                in_dims[1],
                n_ports);

  // Normalize the input to take into account the spreading factor.
  srsvec::sc_prod(ch.get_data(), in.get_data(), 1.0F / static_cast<float>(std::sqrt(nof_dmrs_symbols_hop) * NSHIFTS));

  // Estimate the total channel energy (across ports) for all ICSs.
  srsvec::modulus_square(rsrp, ch.get_view({0}));
  for (unsigned i_port = 1; i_port != n_ports; ++i_port) {
    srsvec::modulus_square_and_add(rsrp, ch.get_view({i_port}), rsrp);
  }

  // Find the strongest ICS and neglect all ICSs that are 10+ dB below that one.
  auto [pos, value] = srsvec::max_element(rsrp);
  float th          = value / 10;

  for (unsigned i_port = 0; i_port != n_ports; ++i_port) {
    span<cf_t> path = ch.get_view({i_port});
    for (unsigned i_element = 0; i_element != NSHIFTS; ++i_element) {
      path[i_element] = (rsrp[i_element] > th) ? path[i_element] : 0;
    }
  }

  srsvec::sc_prod(rsrp, rsrp, 1.0F / n_ports);
}

/// Estimates the noise by looking at the difference between the received and the reconstructed signal.
static float estimate_noise(
    const static_tensor<3, cf_t, MAX_PORTS * pucch_detector_format1::MAX_ALLOCATED_RE_F1 / 2>& dmrs_lse,
    const static_tensor<3, cf_t, MAX_PORTS * pucch_detector_format1::MAX_ALLOCATED_RE_F1 / 2>& dmrs_reconstructed)
{
  const auto& lse_dims           = dmrs_lse.get_dimensions_size();
  const auto& reconstructed_dims = dmrs_reconstructed.get_dimensions_size();

  srsran_assert(lse_dims[0] == NRE, "The number of REs {} in the LSE tensor is not {}.", lse_dims[0], NRE);
  srsran_assert(reconstructed_dims[0] == NRE,
                "The number of REs {} in the reconstructed tensor is not {}.",
                reconstructed_dims[0],
                NRE);
  unsigned n_ports = lse_dims[2];
  srsran_assert(reconstructed_dims[2] == n_ports,
                "The number of antenna ports {} in the reconstructed tensor does not match that of the LSE tensor {}.",
                reconstructed_dims[2],
                n_ports);
  unsigned n_symbols = lse_dims[1];
  srsran_assert(reconstructed_dims[1] == n_symbols,
                "The number of OFDM symbols {} in the reconstructed tensor does not match that of the LSE tensor {}.",
                reconstructed_dims[1],
                n_symbols);

  static_vector<cf_t, MAX_PORTS*(pucch_detector_format1::MAX_ALLOCATED_RE_F1 / 2 + 1)> all_differences(
      dmrs_lse.get_data().size());
  srsvec::subtract(all_differences, dmrs_lse.get_data(), dmrs_reconstructed.get_data());

  return (srsvec::average_power(all_differences) * all_differences.size());
}

void pucch_detector_format1::combine_reconstructed_contributions(
    static_tensor<3, cf_t, MAX_PORTS * pucch_detector_format1::MAX_ALLOCATED_RE_F1 / 2>& dmrs_reconstructed,
    const static_tensor<2, cf_t, NRE * MAX_PORTS>&                                       ch,
    span<const cf_t>                                                                     w_star_dmrs,
    bool                                                                                 is_first)
{
  const auto& ch_dims   = ch.get_dimensions_size();
  const auto& dmrs_dims = dmrs_reconstructed.get_dimensions_size();

  srsran_assert(ch_dims[0] == NSHIFTS, "The number of REs {} in the input tensor is not {}.", ch_dims[0], NSHIFTS);
  srsran_assert(dmrs_dims[0] == NRE, "The number of REs {} in the output tensor is not {}.", dmrs_dims[0], NRE);
  unsigned n_ports = ch_dims[1];
  srsran_assert(dmrs_dims[2] == n_ports,
                "The number of antenna ports {} in the output tensor does not match that of the input tensor {}.",
                dmrs_dims[2],
                n_ports);
  unsigned n_symbols = w_star_dmrs.size();
  srsran_assert(dmrs_dims[1] == n_symbols,
                "The number of OFDM symbols {} in the output tensor does not match the length of the sequence w {}.",
                dmrs_dims[1],
                n_symbols);

  std::array<cf_t, NRE> reconstructed;

  for (unsigned i_port = 0; i_port != n_ports; ++i_port) {
    // Spreading in frequency domain is done with columns of DFT matrix. Therefore, the superposition of all PUCCH is
    // equivalent to taking the iDFT of the vector of channel coefficients.
    span<cf_t> idft_input = idft->get_input();
    srsvec::copy(idft_input, ch.get_view({i_port}));
    span<const cf_t> idft_output = idft->run();

    for (unsigned i_symbol = 0; i_symbol != n_symbols; ++i_symbol) {
      // Now copy (or combine) the result to each output symbol after applying the OCC.
      if (is_first) {
        srsvec::sc_prod(dmrs_reconstructed.get_view({i_symbol, i_port}), idft_output, std::conj(w_star_dmrs[i_symbol]));
      } else {
        srsvec::sc_prod(reconstructed, idft_output, std::conj(w_star_dmrs[i_symbol]));
        srsvec::add(dmrs_reconstructed.get_view({i_symbol, i_port}),
                    reconstructed,
                    dmrs_reconstructed.get_view({i_symbol, i_port}));
      }
    }
  }
}

/// \brief Computes the hop contributions to the detection metric.
///
/// For each hop (and each port) there are two contributions per PUCCH transmission: roughly speaking, one is related to
/// the energy of the data and DM-RS components, the other to the cross combination of the two. The latter is a complex
/// value that has to be later defined when choosing the most likely transmitted modulation symbol.
/// The function should be called once per each OCCI.
///
/// \param[out] main_contributions   Main-term contributions to the detection metric, one entry per ICS.
/// \param[out] cross_contributions  Cross-term contributions to the detection metric, one entry per ICS.
/// \param[in]  data                 Data content of the PUCCH transmission (after despreading): one entry per ICS, for
///                                  each port.
/// \param[in]  dmrs                 DM-RS content of the PUCCH transmission (after despreading): one entry per ICS, for
///                                  each port.
static void compute_contributions(span<float>                                        main_contributions,
                                  span<cf_t>                                         cross_contributions,
                                  const static_tensor<2, cf_t, NSHIFTS * MAX_PORTS>& data,
                                  const static_tensor<2, cf_t, NSHIFTS * MAX_PORTS>& dmrs)
{
  const auto& data_dims = data.get_dimensions_size();
  const auto& dmrs_dims = dmrs.get_dimensions_size();

  srsran_assert(main_contributions.size() == NSHIFTS,
                "The number of elements {} in the main contributions output is not {}.",
                main_contributions.size(),
                NSHIFTS);
  srsran_assert(cross_contributions.size() == NSHIFTS,
                "The number of elements {} in the cross contributions output is not {}.",
                cross_contributions.size(),
                NSHIFTS);
  srsran_assert(data_dims[0] == NSHIFTS,
                "The number of elements {} per port in the data tensor is not {}.",
                data_dims[0],
                NSHIFTS);
  srsran_assert(dmrs_dims[0] == NSHIFTS,
                "The number of elements {} per port in the dmrs tensor is not {}.",
                dmrs_dims[0],
                NSHIFTS);
  unsigned n_ports = data_dims[1];
  srsran_assert(dmrs_dims[1] == n_ports,
                "The number of antenna ports {} in the data tensor does not match that of the dmrs tensor {}.",
                n_ports,
                dmrs_dims[1]);

  // Main contributions are just to total energy of the two components.
  srsvec::modulus_square(main_contributions, data.get_view({0}));
  srsvec::modulus_square_and_add(main_contributions, dmrs.get_view({0}), main_contributions);

  // Cross contributions are the cross conjugated product of the two components.
  srsvec::prod_conj(cross_contributions, dmrs.get_view({0}), data.get_view({0}));
  std::array<cf_t, NSHIFTS> support_cross;

  // Accumulate the contributions from all ports.
  for (unsigned i_port = 1; i_port != n_ports; ++i_port) {
    srsvec::modulus_square_and_add(main_contributions, data.get_view({i_port}), main_contributions);
    srsvec::modulus_square_and_add(main_contributions, dmrs.get_view({i_port}), main_contributions);

    srsvec::prod_conj(support_cross, dmrs.get_view({i_port}), data.get_view({i_port}));
    srsvec::add(cross_contributions, support_cross, cross_contributions);
  }
}

pucch_detector_format1::hop_contribution_common pucch_detector_format1::process_hop(
    static_vector<hop_contribution_mux, nof_initial_cyclic_shifts * nof_time_domain_occs>& hop_metrics,
    const resource_grid_reader&                                                            grid,
    const pucch_detector::format1_configuration&                                           config,
    const pucch_format1_map<unsigned>&                                                     mux_nof_harq_ack,
    unsigned                                                                               i_hop)
{
  unsigned first_subcarrier     = NRE * config.starting_prb;
  unsigned start_symbol         = config.start_symbol_index;
  unsigned last_symbol          = start_symbol + config.nof_symbols;
  unsigned nof_data_symbols_hop = config.nof_symbols / 2U;
  unsigned nof_dmrs_symbols_hop = divide_ceil(config.nof_symbols, 2U);
  unsigned nof_ports            = grid.get_nof_ports();

  if (config.second_hop_prb.has_value()) {
    if (i_hop == 0) {
      last_symbol = start_symbol + config.nof_symbols / 2U;

      unsigned nof_symbols_hop = nof_data_symbols_hop;
      nof_data_symbols_hop     = config.nof_symbols / 4U;
      nof_dmrs_symbols_hop     = nof_symbols_hop - nof_data_symbols_hop;
    } else {
      start_symbol     = start_symbol + config.nof_symbols / 2U;
      first_subcarrier = NRE * *config.second_hop_prb;

      unsigned nof_symbols_hop = nof_dmrs_symbols_hop;
      nof_data_symbols_hop     = divide_ceil(config.nof_symbols - 1U, 4U);
      nof_dmrs_symbols_hop     = nof_symbols_hop - nof_data_symbols_hop;
    }
  }

  static_tensor<3, cf_t, MAX_PORTS * MAX_ALLOCATED_RE_F1 / 2> data_lse({NRE, nof_data_symbols_hop, nof_ports});
  static_tensor<3, cf_t, MAX_PORTS * MAX_ALLOCATED_RE_F1 / 2> dmrs_lse({NRE, nof_dmrs_symbols_hop, nof_ports});
  static_tensor<3, cf_t, MAX_PORTS * MAX_ALLOCATED_RE_F1 / 2> data_dft({NRE, nof_data_symbols_hop, nof_ports});
  static_tensor<3, cf_t, MAX_PORTS * MAX_ALLOCATED_RE_F1 / 2> dmrs_dft({NRE, nof_dmrs_symbols_hop, nof_ports});

  // Compute the group index from the initialization seed. For PUCCH F1, the sequence number is identically zero.
  unsigned group_index     = config.n_id % 30;
  unsigned sequence_number = 0;

  unsigned i_data = 0;
  unsigned i_dmrs = 0;
  float    epre   = 0;
  for (unsigned i_symbol = start_symbol; i_symbol != last_symbol; ++i_symbol) {
    // Get the base (ICS 0) spreading sequence for the current symbol.
    unsigned alpha_index = helper.get_alpha_index(config.slot, config.cp, config.n_id, i_symbol, /*m0=*/0, /*m_cs=*/0);
    span<const cf_t> seq_r = low_papr->get(group_index, sequence_number, alpha_index);
    // Divide DM-RS symbols from data ones. All symbols (data and DM-RS) are matched to the base spreading sequence to
    // obtain dmrs_lse and data_lse. Then a DFT is taken - this is equivalent to despreading the ICS-depending frequency
    // components.
    if (dmrs_mask.test(i_symbol)) {
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        span<cf_t> dmrs_help = dmrs_lse.get_view({i_dmrs, i_port});
        grid.get(dmrs_help, i_port, i_symbol, first_subcarrier);
        epre += srsvec::average_power(dmrs_help) * dmrs_help.size();
        srsvec::prod_conj(dmrs_help, dmrs_help, seq_r);

        span<cf_t> dft_input = dft->get_input();
        srsvec::copy(dft_input, dmrs_help);
        span<const cf_t> dft_output = dft->run();
        srsvec::copy(dmrs_dft.get_view({i_dmrs, i_port}), dft_output);
      }
      ++i_dmrs;
    } else {
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        span<cf_t> data_help = data_lse.get_view({i_data, i_port});
        grid.get(data_help, i_port, i_symbol, first_subcarrier);
        epre += srsvec::average_power(data_help) * data_help.size();
        srsvec::prod_conj(data_help, data_help, seq_r);

        span<cf_t> dft_input = dft->get_input();
        srsvec::copy(dft_input, data_help);
        span<const cf_t> dft_output = dft->run();
        srsvec::copy(data_dft.get_view({i_data, i_port}), dft_output);
      }
      ++i_data;
    }
  }
  srsran_assert(i_data == nof_data_symbols_hop, "Error reading PUCCH F1 data symbols.");
  srsran_assert(i_dmrs == nof_dmrs_symbols_hop, "Error reading PUCCH F1 dmrs symbols.");

  bool                                                        is_first = true;
  static_tensor<3, cf_t, MAX_PORTS * MAX_ALLOCATED_RE_F1 / 2> dmrs_reconstructed(
      {NRE, nof_dmrs_symbols_hop, nof_ports});

  auto this_pucch_nof_harq_ack = mux_nof_harq_ack.begin();
  auto end_pucch_nof_harq_ack  = mux_nof_harq_ack.end();
  for (unsigned occi = 0; (occi != nof_data_symbols_hop) && (this_pucch_nof_harq_ack != end_pucch_nof_harq_ack);
       ++occi) {
    if ((*this_pucch_nof_harq_ack).time_domain_occ != occi) {
      continue;
    }

    span<const cf_t> w_star_data = occ.get_sequence_conj(nof_data_symbols_hop, occi);
    span<const cf_t> w_star_dmrs = occ.get_sequence_conj(nof_dmrs_symbols_hop, occi);

    static_tensor<2, cf_t, MAX_PORTS * NSHIFTS> data_single({NSHIFTS, nof_ports});
    static_tensor<2, cf_t, MAX_PORTS * NSHIFTS> dmrs_single({NSHIFTS, nof_ports});

    // Combine all symbols (for DM-RS and data separately) to despread in the time domain.
    combine_symbols(data_single, data_dft, w_star_data);
    combine_symbols(dmrs_single, dmrs_dft, w_star_dmrs);

    std::array<float, NSHIFTS> main_contributions;
    std::array<cf_t, NSHIFTS>  cross_contributions;

    // Compute the contributions of the current hop to the final detection metrics.
    compute_contributions(main_contributions, cross_contributions, data_single, dmrs_single);

    float normalizer = 1.0F / static_cast<float>(NSHIFTS * (nof_dmrs_symbols_hop + nof_data_symbols_hop));

    srsvec::sc_prod(main_contributions, main_contributions, normalizer);
    srsvec::sc_prod(cross_contributions, cross_contributions, normalizer);

    // Estimate the channels from the DM-RS symbols.
    static_tensor<2, cf_t, MAX_PORTS * NSHIFTS> estimated_channels({NSHIFTS, nof_ports});
    std::array<float, NSHIFTS>                  rsrp;
    estimate_channels(estimated_channels, rsrp, dmrs_single, nof_dmrs_symbols_hop);

    // Store the metrics together with the PUCCH identifiers (ICS and OCCI) to be retrieved later.
    for (; (this_pucch_nof_harq_ack != end_pucch_nof_harq_ack) && ((*this_pucch_nof_harq_ack).time_domain_occ == occi);
         ++this_pucch_nof_harq_ack) {
      unsigned ics = (*this_pucch_nof_harq_ack).initial_cyclic_shift;

      hop_metrics.push_back({.main_contribution  = main_contributions[ics],
                             .cross_contribution = cross_contributions[ics],
                             .rsrp_contribution  = rsrp[ics]});
    }

    // Rebuild the estimated noiseless received DM-RS symbols (implies frequency and time spreading).
    combine_reconstructed_contributions(dmrs_reconstructed, estimated_channels, w_star_dmrs, is_first);
    is_first = false;
  }

  srsran_assert(hop_metrics.size() == mux_nof_harq_ack.size(),
                "The number of computed hop metrics {} does not match the number of multiplexed PUCCHs {}.",
                hop_metrics.size(),
                mux_nof_harq_ack.size());

  // Estimate the noise by comparing received and reconstructed signal.
  float noise_var = estimate_noise(dmrs_lse, dmrs_reconstructed);

  return {.epre_contribution      = epre,
          .nof_epre_samples       = static_cast<unsigned>(dmrs_lse.get_data().size() + data_lse.get_data().size()),
          .noise_var_contribution = noise_var,
          .nof_noise_samples      = static_cast<unsigned>(dmrs_lse.get_data().size())};
}
