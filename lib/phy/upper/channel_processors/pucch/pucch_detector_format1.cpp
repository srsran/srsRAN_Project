/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

static void validate_config(const pucch_detector::format1_configuration& config)
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
  srsran_assert(config.time_domain_occ <= 6,
                "Setting {} as the time-domain OCC index, but only values between 0 and 6 are valid.",
                config.time_domain_occ);
  if (config.second_hop_prb.has_value()) {
    srsran_assert(config.second_hop_prb.value() <= 274,
                  "Setting {} as the PRB allocated to PUCCH after frequency hopping, but only values between 0 and 274 "
                  "are valid.",
                  config.second_hop_prb.value());
    srsran_assert(config.time_domain_occ < config.nof_symbols / 4U,
                  "Cannot have OCCI {} with {} allocated OFDM symbols and frequency hopping enabled.",
                  config.time_domain_occ,
                  config.nof_symbols);
  } else {
    srsran_assert(config.time_domain_occ < config.nof_symbols / 2U,
                  "Cannot have OCCI {} with {} allocated OFDM symbols.",
                  config.time_domain_occ,
                  config.nof_symbols);
  }
  srsran_assert(config.n_id <= 1023,
                "Initializing the pseudorandom generator with {}, but only values between 0 and 1023 are valid.",
                config.n_id);
  srsran_assert(config.nof_harq_ack <= 2, "At most two ACK bits - requested {}.", config.nof_harq_ack);
}

// Given the detected symbol and the corresponding equivalent noise variance, demodulates the symbol into nof_bits bits.
// It also returns the detection metric used to decide whether the PUCCH was transmitted or not by threshold comparison.
static float detect_bits(span<uint8_t> out_bits, cf_t detected_symbol, float eq_noise_var)
{
  unsigned nof_bits          = out_bits.size();
  float    detection_metric  = std::real(detected_symbol) + std::imag(detected_symbol);
  unsigned bits              = (detection_metric > 0) ? 0U : 3U;
  detection_metric           = std::abs(detection_metric);
  float    detection_metric2 = std::real(detected_symbol) - std::imag(detected_symbol);
  unsigned bits2             = (detection_metric2 > 0) ? 2U : 1U;
  detection_metric2          = std::abs(detection_metric2);
  if ((nof_bits > 1) && (detection_metric2 > detection_metric)) {
    out_bits[0] = (bits2 & 1U);
    out_bits[1] = ((bits2 >> 1U) & 1U);
    return (std::norm(detected_symbol) / eq_noise_var);
  }
  out_bits[0] = (bits & 1U);
  if (nof_bits > 1) {
    out_bits[1] = ((bits >> 1U) & 1U);
  }
  return (std::norm(detected_symbol) / eq_noise_var);
}

pucch_detector::pucch_detection_result
pucch_detector_format1::detect(const resource_grid_reader&                  grid,
                               const channel_estimate&                      estimates,
                               const pucch_detector::format1_configuration& config)
{
  validate_config(config);

  // Total number of REs used for PUCCH data (recall that positive integer division implies taking the floor).
  unsigned nof_res   = (config.nof_symbols / 2) * NRE;
  unsigned nof_ports = config.ports.size();
  time_spread_sequence.resize(nof_ports, nof_res);
  ch_estimates.resize(nof_res, nof_ports, 1);
  eq_time_spread_sequence.resize(nof_res);
  eq_time_spread_noise_var.resize(nof_res);

  // Compute the number of data symbols before frequency hop.
  nof_data_symbols         = config.nof_symbols / 2;
  nof_data_symbols_pre_hop = nof_data_symbols;
  if (config.second_hop_prb.has_value()) {
    nof_data_symbols_pre_hop = config.nof_symbols / 4;
  }

  alpha_indices = span<unsigned>(alpha_buffer).first(nof_data_symbols);

  extract_data_and_estimates(
      grid, estimates, config.start_symbol_index, config.starting_prb, config.second_hop_prb, config.ports);

  span<float> noise_var_all_ports = span<float>(noise_var_buffer).first(nof_ports);
  for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
    noise_var_all_ports[i_port] = estimates.get_noise_variance(config.ports[i_port]);
  }
  equalizer->equalize(eq_time_spread_sequence,
                      eq_time_spread_noise_var,
                      time_spread_sequence,
                      ch_estimates,
                      noise_var_all_ports,
                      config.beta_pucch);

  marginalize_w_and_r_out(config);

  // Prepare UCI message output.
  // We don't set the SR bit here - this task is delegated to a higher-level function, based on the uci_status returned
  // by this detector and on the used PUCCH resource.
  // This format doesn't support CSI reports.
  pucch_uci_message::configuration pucch_uci_message_config;
  pucch_uci_message_config.nof_sr               = 0;
  pucch_uci_message_config.nof_harq_ack         = config.nof_harq_ack;
  pucch_uci_message_config.nof_csi_part1        = 0;
  pucch_uci_message_config.nof_csi_part2        = 0;
  pucch_detector::pucch_detection_result output = {pucch_uci_message(pucch_uci_message_config), 0};

  // Select view of the payload.
  span<uint8_t> bits = output.uci_message.get_harq_ack_bits();

  // Recall that, when nof_harq_ack == 0, we still need to look for the positive SR indicator (i.e., a single, 0-valued
  // transmitted bit).
  static_vector<uint8_t, 1> temp_bits(1);
  if (config.nof_harq_ack == 0) {
    bits = temp_bits;
  }

  float detection_metric = detect_bits(bits, detected_symbol, eq_noise_var);

  // Detection threshold.
  //
  // The detection metric, as computed by \c detect_bits, is assumed to be normally distributed with variance 1 and mean
  // either 0 (when there is no PUCCH) or larger than 0 (when there is a PUCCH). Therefore, one can target a constant
  // probability of false alarm of 1% by setting the detection threshold T such that Q(T) = 0.01, where the Q-function
  // is the tail distribution function of the standard normal distribution.
  constexpr float THRESHOLD = 4.0F;
  bool            is_msg_ok = (detection_metric > THRESHOLD);
  output.detection_metric   = detection_metric / THRESHOLD;

  if (!is_msg_ok) {
    output.uci_message.set_status(uci_status::invalid);
    return output;
  }

  if (config.nof_harq_ack > 0) {
    output.uci_message.set_status(uci_status::valid);
    return output;
  }

  // If we are here, there should only be a positive SR bit and it should be 0, since nothing is sent for negative
  // SR and no ACK.
  if (bits[0] == 0U) {
    output.uci_message.set_status(uci_status::valid);
    return output;
  }

  output.uci_message.set_status(uci_status::unknown);
  return output;
}

void pucch_detector_format1::extract_data_and_estimates(const resource_grid_reader&              grid,
                                                        const channel_estimate&                  estimates,
                                                        unsigned                                 first_symbol,
                                                        unsigned                                 first_prb,
                                                        std::optional<unsigned>                  second_prb,
                                                        const static_vector<uint8_t, MAX_PORTS>& antenna_ports)
{
  for (uint8_t port : antenna_ports) {
    unsigned      i_symbol       = 0;
    unsigned      skip           = 0;
    unsigned      symbol_index   = first_symbol + 1;
    span<cbf16_t> sequence_slice = time_spread_sequence.get_slice(port);
    span<cbf16_t> estimate_slice = ch_estimates.get_channel(port, 0);
    for (; i_symbol != nof_data_symbols_pre_hop; ++i_symbol, skip += NRE, symbol_index += 2) {
      // Index of the first subcarrier assigned to PUCCH, before hopping.
      unsigned      k_init         = NRE * first_prb;
      span<cbf16_t> sequence_chunk = sequence_slice.subspan(skip, NRE);
      grid.get(sequence_chunk, port, symbol_index, k_init);

      span<const cbf16_t> tmp = estimates.get_symbol_ch_estimate(symbol_index, port);
      srsvec::copy(estimate_slice.subspan(skip, NRE), tmp.subspan(k_init, NRE));
    }

    for (; i_symbol != nof_data_symbols; ++i_symbol, skip += NRE, symbol_index += 2) {
      // Index of the first subcarrier assigned to PUCCH, after hopping. Note that we only enter this loop if
      // second_prb.has_value().
      unsigned      k_init         = NRE * second_prb.value();
      span<cbf16_t> sequence_chunk = sequence_slice.subspan(skip, NRE);
      grid.get(sequence_chunk, port, symbol_index, k_init);

      span<const cbf16_t> tmp_in  = estimates.get_symbol_ch_estimate(symbol_index, port).subspan(k_init, NRE);
      span<cbf16_t>       tmp_out = estimate_slice.subspan(skip, NRE);
      srsvec::copy(tmp_out, tmp_in);
    }
  }
}

// Computest the indices of the cyclic shifts for all symbols.
static void compute_alpha_indices(span<unsigned>       indices,
                                  unsigned             start_symbol,
                                  unsigned             nof_symbols,
                                  const slot_point&    slot,
                                  const cyclic_prefix& cp,
                                  unsigned             n_id,
                                  unsigned             m0,
                                  pucch_helper&        helper)
{
  srsran_assert(indices.size() == nof_symbols / 2,
                "The number of alpha indices {} does not match with the number of allocated symbols {}.",
                indices.size(),
                nof_symbols);

  // Only every other symbol, starting from the second one, contains data.
  for (unsigned i_symbol = 1, i_alpha = 0; i_symbol < nof_symbols; i_symbol += 2, ++i_alpha) {
    // Compute alpha index.
    indices[i_alpha] = helper.get_alpha_index(slot, cp, n_id, start_symbol + i_symbol, m0, 0);
  }
}

void pucch_detector_format1::marginalize_w_and_r_out(const pucch_detector::format1_configuration& config)
{
  unsigned time_domain_occ = config.time_domain_occ;

  srsran_assert(config.group_hopping == pucch_group_hopping::NEITHER,
                "At the moment, only group the hopping type 'neither' is implemented, requesting {}",
                ((config.group_hopping == pucch_group_hopping::ENABLE) ? "enable" : "disable"));
  unsigned group_index     = config.n_id % 30;
  unsigned sequence_number = 0;

  span<const cf_t> w_star = occ.get_sequence_conj(nof_data_symbols_pre_hop, time_domain_occ);

  compute_alpha_indices(alpha_indices,
                        config.start_symbol_index,
                        config.nof_symbols,
                        config.slot,
                        config.cp,
                        config.n_id,
                        config.initial_cyclic_shift,
                        helper);

  detected_symbol = 0;
  unsigned offset = 0;
  for (unsigned i_symbol = 0; i_symbol != nof_data_symbols_pre_hop; ++i_symbol, offset += NRE) {
    span<const cf_t> seq_r = low_papr->get(group_index, sequence_number, alpha_indices[i_symbol]);
    for (unsigned i_elem = 0; i_elem != NRE; ++i_elem) {
      detected_symbol += eq_time_spread_sequence[offset + i_elem] * w_star[i_symbol] * std::conj(seq_r[i_elem]);
    }
  }

  unsigned nof_data_symbols_post_hop = nof_data_symbols - nof_data_symbols_pre_hop;
  if (nof_data_symbols_post_hop > 0) {
    w_star = occ.get_sequence_conj(nof_data_symbols_post_hop, time_domain_occ);

    for (unsigned i_symbol = 0; i_symbol != nof_data_symbols_post_hop; ++i_symbol, offset += NRE) {
      span<const cf_t> seq_r =
          low_papr->get(group_index, sequence_number, alpha_indices[i_symbol + nof_data_symbols_pre_hop]);
      for (unsigned i_elem = 0; i_elem != NRE; ++i_elem) {
        detected_symbol += eq_time_spread_sequence[offset + i_elem] * w_star[i_symbol] * std::conj(seq_r[i_elem]);
      }
    }
  }

  auto n_repetitions = static_cast<float>(eq_time_spread_sequence.size());
  detected_symbol /= n_repetitions;
  // For the noise variance, we have to compute the sum of all variances and divide by the square of their number: same
  // as computing the mean and dividing again buy their number.
  eq_noise_var = srsvec::mean(eq_time_spread_noise_var) / n_repetitions;
}

// The number of available initial cyclic shifts is the same as the number of RE per PRB, i.e., 12.
static constexpr unsigned NSHIFTS = pucch_constants::format1_initial_cyclic_shift_range.stop();

/// \brief Detects the transmitted symbol.
///
/// This function looks for the symbol (BPSK or QPSK) that maximizes the real part of the cross term between DM-RS and
/// signal components.
/// \param[out] bits        The bits corresponding to the detected symbol.
/// \param[in]  cross_term  The cross term between DM-RS and signal components of the PUCCH transmission.
/// \returns The real part of the cross term between DM-RS and signal components assuming the detected symbol, and the
/// detected symbol.
/// \remark The order of the modulation is inferred from the length of \c bits.
static std::pair<float, cf_t> detect_symbol(span<uint8_t> bits, cf_t cross_term)
{
  unsigned nof_bits = bits.size();
  srsran_assert(nof_bits <= 2, "PUCCH Format 1 supports max 2 bits, requested {}.", nof_bits);

  // Case BPSK.
  if (nof_bits == 1) {
    cf_t  rx_symbol    = cf_t(M_SQRT1_2f, M_SQRT1_2f);
    float metric_cross = std::real(rx_symbol * cross_term);
    if (metric_cross > 0) {
      bits[0] = 0;
    } else {
      rx_symbol    = -rx_symbol;
      metric_cross = -metric_cross;
      bits[0]      = 1;
    }
    return {metric_cross, rx_symbol};
  }

  // Case QPSK.
  cf_t  d1 = cf_t(M_SQRT1_2f, M_SQRT1_2f);
  float m1 = std::real(d1 * cross_term);
  cf_t  d2 = cf_t(M_SQRT1_2f, -M_SQRT1_2f);
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

  return {metric_cross, rx_symbol};
}

const pucch_format1_map<pucch_detector::pucch_detection_result>&
pucch_detector_format1::detect(const resource_grid_reader&                  grid,
                               const pucch_detector::format1_configuration& config,
                               const pucch_format1_map<unsigned>&           mux_nof_harq_ack)
{
  // Validate the configuration of all multiplexed transmissions.
  pucch_detector::format1_configuration conf_tmp = config;

  // for (auto this_pucch_nof_harq_ack = mux_nof_harq_ack.begin(), end_pucch_nof_harq_ack = mux_nof_harq_ack.end();
  //      this_pucch_nof_harq_ack != end_pucch_nof_harq_ack;
  //      ++this_pucch_nof_harq_ack) {
  for (const auto& this_pucch_nof_harq_ack : mux_nof_harq_ack) {
    conf_tmp.initial_cyclic_shift = this_pucch_nof_harq_ack.initial_cyclic_shift;
    conf_tmp.time_domain_occ      = this_pucch_nof_harq_ack.time_domain_occ;
    conf_tmp.nof_harq_ack         = this_pucch_nof_harq_ack.value;
    validate_config(conf_tmp);
  }

  // Set the boolean mask of OFDM symbols carrying DM-RS (every other allocated symbol starting from the first).
  dmrs_mask.reset();
  for (unsigned i_symbol = config.start_symbol_index, end_symbol = config.start_symbol_index + config.nof_symbols;
       i_symbol < end_symbol;
       i_symbol += 2) {
    dmrs_mask.set(i_symbol);
  }

  static_vector<metric_contributions, NSHIFTS * pucch_constants::format1_time_domain_occ_range.stop()> metrics_hop0(0);
  static_vector<metric_contributions, NSHIFTS * pucch_constants::format1_time_domain_occ_range.stop()> metrics_hop1(0);
  auto [noise_var, noise_samples] = process_hop(metrics_hop0, grid, config, mux_nof_harq_ack, /*i_hop=*/0);
  if (config.second_hop_prb.has_value()) {
    auto [noise_var1, noise_samples1] = process_hop(metrics_hop1, grid, config, mux_nof_harq_ack, /*i_hop =*/1);
    noise_var += noise_var1;
    noise_samples += noise_samples1;
  }

  noise_var /= noise_samples;

  // Different thresholds are needed depending on the number of antenna ports and on whether or not the intra-slot
  // frequency hopping is active.
  unsigned nof_contributions   = grid.get_nof_ports() * (config.second_hop_prb.has_value() ? 2 : 1);
  float    detection_threshold = 0.0F;

  switch (nof_contributions) {
    case 1:
      detection_threshold = 6.25F;
      break;
    case 2:
      detection_threshold = 14.65F;
      break;
    case 4:
      detection_threshold = 20.3F;
      break;
    case 8:
      detection_threshold = 29.8F;
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
    if (config.second_hop_prb.has_value()) {
      main_contribution += this_metric_hop1->main_contribution;
      cross_contribution += this_metric_hop1->cross_contribution;
      ++this_metric_hop1;
    }

    unsigned nof_harq_ack = (*this_pucch_nof_harq_ack).value;
    mux_results.insert(ics,
                       occi,
                       {.uci_message = pucch_uci_message(
                            {.nof_sr = 0, .nof_harq_ack = nof_harq_ack, .nof_csi_part1 = 0, .nof_csi_part2 = 0}),
                        .detection_metric = std::numeric_limits<float>::quiet_NaN()});
    pucch_detector::pucch_detection_result& this_result = mux_results.get(ics, occi);
    span<uint8_t>                           bits        = this_result.uci_message.get_harq_ack_bits();

    // If no HARQ ACK bits were transmitted, we still need to look at the SR "bit."
    std::array<uint8_t, 1> bit_sr;
    if (nof_harq_ack == 0) {
      bits = span<uint8_t>(bit_sr);
    }

    auto [detected_contribution, rx_symbol] = detect_symbol(bits, cross_contribution);
    float detection_metric                  = (main_contribution + 2 * detected_contribution) / noise_var;

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
  for (unsigned i_port = 0; i_port != n_ports; ++i_port) {
    srsvec::sc_prod(in.get_view({0, i_port}), w[0], out.get_view({i_port}));
    for (unsigned i_symbol = 1; i_symbol != n_symbols; ++i_symbol) {
      srsvec::sc_prod(in.get_view({i_symbol, i_port}), w[i_symbol], scaled);
      srsvec::add(scaled, out.get_view({i_port}), out.get_view({i_port}));
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
/// \param[in]  in                     DM-RS samples after despreading in time and frequency domains (for one single OCC
/// index).
/// \param[in[  nof_dmrs_symbols_hop   Number of DM-RS symbols in the current hop.
static void estimate_channels(static_tensor<2, cf_t, NSHIFTS * MAX_PORTS>&       ch,
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
  srsvec::sc_prod(in.get_data(), 1.0F / static_cast<float>(nof_dmrs_symbols_hop * NSHIFTS), ch.get_data());

  // Estimate the total channel energy (across ports) for all ICSs.
  std::array<float, NSHIFTS> total_ch_energy;
  srsvec::modulus_square(total_ch_energy, ch.get_view({0}));
  for (unsigned i_port = 1; i_port != n_ports; ++i_port) {
    srsvec::modulus_square_and_add(total_ch_energy, ch.get_view({i_port}), total_ch_energy);
  }

  // Find the strongest ICS and neglect all ICSs that are 10+ dB below that one.
  auto [pos, value] = srsvec::max_element(total_ch_energy);
  float th          = value / 10;

  for (unsigned i_port = 0; i_port != n_ports; ++i_port) {
    span<cf_t> path = ch.get_view({i_port});
    for (unsigned i_element = 0; i_element != NSHIFTS; ++i_element) {
      path[i_element] = (total_ch_energy[i_element] > th) ? path[i_element] : 0;
    }
  }
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

    static constexpr float nre_inv = 1.0F / static_cast<float>(NSHIFTS);
    for (unsigned i_symbol = 0; i_symbol != n_symbols; ++i_symbol) {
      // Now copy (or combine) the result to each output symbol after applying the OCC (nre_inv is needed since our iDFT
      // doesn't normalize the output).
      if (is_first) {
        srsvec::sc_prod(
            idft_output, std::conj(w_star_dmrs[i_symbol]) * nre_inv, dmrs_reconstructed.get_view({i_symbol, i_port}));
      } else {
        srsvec::sc_prod(idft_output, std::conj(w_star_dmrs[i_symbol]) * nre_inv, reconstructed);
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
  srsvec::prod_conj(dmrs.get_view({0}), data.get_view({0}), cross_contributions);
  std::array<cf_t, NSHIFTS> support_cross;

  // Accumulate the contributions from all ports.
  for (unsigned i_port = 1; i_port != n_ports; ++i_port) {
    srsvec::modulus_square_and_add(main_contributions, data.get_view({i_port}), main_contributions);
    srsvec::modulus_square_and_add(main_contributions, dmrs.get_view({i_port}), main_contributions);

    srsvec::prod_conj(dmrs.get_view({i_port}), data.get_view({i_port}), support_cross);
    srsvec::add(cross_contributions, support_cross, cross_contributions);
  }
}

std::pair<float, unsigned> pucch_detector_format1::process_hop(
    static_vector<metric_contributions, nof_initial_cyclic_shifts * nof_time_domain_occs>& hop_metrics,
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
      first_subcarrier = NRE * config.second_hop_prb.value();

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
        srsvec::prod_conj(dmrs_help, seq_r, dmrs_help);

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
        srsvec::prod_conj(data_help, seq_r, data_help);

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

    srsvec::sc_prod(main_contributions, normalizer, main_contributions);
    srsvec::sc_prod(cross_contributions, normalizer, cross_contributions);

    // Store the metrics together with the PUCCH identifiers (ICS and OCCI) to be retrieved later.
    for (; (this_pucch_nof_harq_ack != end_pucch_nof_harq_ack) && ((*this_pucch_nof_harq_ack).time_domain_occ == occi);
         ++this_pucch_nof_harq_ack) {
      unsigned ics = (*this_pucch_nof_harq_ack).initial_cyclic_shift;
      hop_metrics.push_back(
          {.main_contribution = main_contributions[ics], .cross_contribution = cross_contributions[ics]});
    }

    // Estimate the channels from the DM-RS symbols.
    static_tensor<2, cf_t, MAX_PORTS * NSHIFTS> estimated_channels({NSHIFTS, nof_ports});
    estimate_channels(estimated_channels, dmrs_single, nof_dmrs_symbols_hop);

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

  return {noise_var, dmrs_lse.get_data().size()};
}
