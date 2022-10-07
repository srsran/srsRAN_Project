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
/// \brief PUCCH detector definition for Formats 0 and 1.

#include "pucch_detector_impl.h"

using namespace srsgnb;

namespace {
// Container for time-domain spreading sequences of length N.
template <unsigned N>
struct time_spreading_sequences {
  using type = cf_t;
  explicit time_spreading_sequences(const std::array<std::array<float, N>, N>& seed)
  {
    for (unsigned i_seq = 0; i_seq != N; ++i_seq) {
      for (unsigned i_term = 0; i_term != N; ++i_term) {
        sequences[i_seq][i_term] = std::exp(-COMPLEX_J * TWOPI * seed[i_seq][i_term] / static_cast<float>(N));
      }
    }
  };

  std::array<std::array<type, N>, N> sequences;
};

template <unsigned N>
span<const cf_t> get_w_star(unsigned length, unsigned i);

// Build and provide access to the time-domain spreading sequences of length 1.
template <>
span<const cf_t> get_w_star<1>(unsigned /*length*/, unsigned i)
{
  static constexpr std::array<cf_t, 1> w = {{{1.0F, 0.0F}}};
  srsgnb_assert(i == 0, "Invalid sequence index {} for sequences of length {}.", i, 1);
  return {w};
}

// Build and provide access to the time-domain spreading sequences of length 2.
template <>
span<const cf_t> get_w_star<2>(unsigned length, unsigned i)
{
  static constexpr unsigned                N = 2;
  static const time_spreading_sequences<N> w({{{0, 0}, {0, 1}}});
  if (length != N) {
    return get_w_star<N - 1>(length, i);
  }
  srsgnb_assert(i < N, "Invalid sequence index {} for sequences of length {}.", i, N);
  return {w.sequences[i]};
}

// Build and provide access to the time-domain spreading sequences of length 3.
template <>
span<const cf_t> get_w_star<3>(unsigned length, unsigned i)
{
  static constexpr unsigned                N = 3;
  static const time_spreading_sequences<N> w({{{0, 0, 0}, {0, 1, 2}, {0, 2, 1}}});
  if (length != N) {
    return get_w_star<N - 1>(length, i);
  }
  srsgnb_assert(i < N, "Invalid sequence index {} for sequences of length {}.", i, N);
  return {w.sequences[i]};
}

// Build and provide access to the time-domain spreading sequences of length 4.
template <>
span<const cf_t> get_w_star<4>(unsigned length, unsigned i)
{
  static constexpr unsigned                N = 4;
  static const time_spreading_sequences<N> w({{{0, 0, 0, 0}, {0, 2, 0, 2}, {0, 0, 2, 2}, {0, 2, 2, 0}}});
  if (length != N) {
    return get_w_star<N - 1>(length, i);
  }
  srsgnb_assert(i < N, "Invalid sequence index {} for sequences of length {}.", i, N);
  return {w.sequences[i]};
}

// Build and provide access to the time-domain spreading sequences of length 5.
template <>
span<const cf_t> get_w_star<5>(unsigned length, unsigned i)
{
  static constexpr unsigned                N = 5;
  static const time_spreading_sequences<N> w(
      {{{0, 0, 0, 0, 0}, {0, 1, 2, 3, 4}, {0, 2, 4, 1, 3}, {0, 3, 1, 4, 2}, {0, 4, 3, 2, 1}}});
  if (length != N) {
    return get_w_star<N - 1>(length, i);
  }
  srsgnb_assert(i < N, "Invalid sequence index {} for sequences of length {}.", i, N);
  return {w.sequences[i]};
}

// Build and provide access to the time-domain spreading sequences of length 6.
template <>
span<const cf_t> get_w_star<6>(unsigned length, unsigned i)
{
  static constexpr unsigned                N = 6;
  static const time_spreading_sequences<N> w({{{0, 0, 0, 0, 0, 0},
                                               {0, 1, 2, 3, 4, 5},
                                               {0, 2, 4, 0, 2, 4},
                                               {0, 3, 0, 3, 0, 3},
                                               {0, 4, 2, 0, 4, 2},
                                               {0, 5, 4, 3, 2, 1}}});
  if (length != N) {
    return get_w_star<N - 1>(length, i);
  }
  srsgnb_assert(i < N, "Invalid sequence index {} for sequences of length {}.", i, N);
  return {w.sequences[i]};
}

// Build and provide access to the time-domain spreading sequences of length 7.
template <>
span<const cf_t> get_w_star<7>(unsigned length, unsigned i)
{
  static constexpr unsigned                N = 7;
  static const time_spreading_sequences<N> w({{{0, 0, 0, 0, 0, 0, 0},
                                               {0, 1, 2, 3, 4, 5, 6},
                                               {0, 2, 4, 6, 1, 3, 5},
                                               {0, 3, 6, 2, 5, 1, 4},
                                               {0, 4, 1, 5, 2, 6, 3},
                                               {0, 5, 3, 1, 6, 4, 2},
                                               {0, 6, 5, 4, 3, 2, 1}}});
  if (length != N) {
    return get_w_star<N - 1>(length, i);
  }
  srsgnb_assert(i < N, "Invalid sequence index {} for sequences of length {}.", i, N);
  return {w.sequences[i]};
}
} // namespace

static void validate_config(const pucch_detector::format1_configuration& config)
{
  srsgnb_assert(config.start_symbol_index <= 10,
                "Setting {} as the first PUCCH symbol index, but only values between 0 and 10 are valid.",
                config.start_symbol_index);
  srsgnb_assert((config.nof_symbols >= 4) && (config.nof_symbols <= 14),
                "Requiring {} OFDM symbols for PUCCH, but only values between 4 and 14 are valid.",
                config.nof_symbols);
  srsgnb_assert(
      config.nof_symbols + config.start_symbol_index <= 14,
      "The sum of the starting OFDM symbol (i.e., {}) and the number of allocated symbols (i.e., {}) cannot exceed 14.",
      config.start_symbol_index,
      config.nof_symbols);
  srsgnb_assert(config.starting_prb <= 274,
                "Setting {} as the PRB allocated to PUCCH, but only values between 0 and 274 are valid.",
                config.starting_prb);
  if (config.second_hop_prb.has_value()) {
    srsgnb_assert(config.second_hop_prb.value() <= 274,
                  "Setting {} as the PRB allocated to PUCCH after frequency hopping, but only values between 0 and 274 "
                  "are valid.",
                  config.second_hop_prb.value());
  }
  srsgnb_assert(config.time_domain_occ <= 6,
                "Setting {} as the time-domain OCC index, but only values between 0 and 6 are valid.",
                config.time_domain_occ);
  srsgnb_assert(
      config.group_index <= 29, "The group index is a number between 0 and 29 - provided {}", config.group_index);
  srsgnb_assert(config.sequence_number <= 1,
                "The sequence number can only be either 0 or 1 - provided {}",
                config.sequence_number);
  srsgnb_assert((config.initial_cyclic_shift <= 9) && (config.initial_cyclic_shift % 3 == 0),
                "The initial cyclic shift takes values in {{0, 3, 6, 9}} - provided {}.",
                config.initial_cyclic_shift);
  srsgnb_assert(config.n_id <= 1023,
                "Initializing the pseudorandom generator with {}, but only values between 0 and 1023 are valid.",
                config.n_id);
  srsgnb_assert(config.nof_sr <= 1, "At most one SR bit - requested {}.", config.nof_sr);
  srsgnb_assert(config.nof_harq_ack <= 2, "At most two ACK bits - requested {}.", config.nof_harq_ack);
  srsgnb_assert(!((config.nof_harq_ack > 0) && (config.nof_sr > 0)),
                "Requiring {} ACK bits and one SR bit, but no SR bits are transmitted when ACK bits are.",
                config.nof_harq_ack);
}

// TEMPORARY: need to refactor equalizer interface.
static void equalize(span<cf_t>       symbols_out,
                     span<float>      noise_out,
                     span<const cf_t> symbols_in,
                     span<const cf_t> channels,
                     float            noise_var,
                     float            beta)
{
  std::transform(symbols_in.begin(), symbols_in.end(), channels.begin(), symbols_out.begin(), [beta](cf_t a, cf_t b) {
    return a / b / beta;
  });
  float beta2 = beta * beta;
  std::transform(channels.begin(), channels.end(), noise_out.begin(), [beta2, noise_var](cf_t a) {
    float a2 = std::abs(a);
    a2 *= a2;
    return noise_var / a2 / beta2;
  });
}

// Given the detected symbol and the corresponding equivalent noise variance, demodulates the symbol into nof_bits bits.
// It also returns the detection metric used to decide whether the PUCCH was transmitted or not by threshold comparison.
static float detect_bits(span<uint8_t> out_bits, cf_t detected_symbol, float eq_noise_var, unsigned nof_bits)
{
  float    detection_metric  = std::real(detected_symbol) + std::imag(detected_symbol);
  unsigned bits              = (detection_metric > 0) ? 0U : 3U;
  detection_metric           = std::abs(detection_metric);
  float    detection_metric2 = std::real(detected_symbol) - std::imag(detected_symbol);
  unsigned bits2             = (detection_metric2 > 0) ? 1U : 2U;
  detection_metric2          = std::abs(detection_metric2);
  if ((nof_bits > 1) && (detection_metric2 > detection_metric)) {
    out_bits[0] = (bits2 & 1U);
    out_bits[1] = ((bits2 >> 1U) & 1U);
    return (detection_metric2 / std::sqrt(eq_noise_var));
  }
  out_bits[0] = (bits & 1U);
  out_bits[1] = ((bits >> 1U) & 1U);
  return detection_metric / std::sqrt(eq_noise_var);
}

pucch_uci_message pucch_detector_impl::detect(const resource_grid_reader&  grid,
                                              const channel_estimate&      estimates,
                                              const format1_configuration& config)
{
  validate_config(config);

  // Total number of REs used for PUCCH data.
  unsigned nof_res = config.nof_symbols * NRE / 2;
  time_spread_sequence.resize(nof_res);
  ch_estimates.resize(nof_res);
  eq_time_spread_sequence.resize(nof_res);
  eq_time_spread_noise_var.resize(nof_res);
  repeated_symbol.resize(nof_res);

  // Compute the number of data symbols before frequency hop.
  nof_data_symbols         = config.nof_symbols / 2;
  nof_data_symbols_pre_hop = nof_data_symbols;
  if (config.second_hop_prb.has_value()) {
    nof_data_symbols_pre_hop = config.nof_symbols / 4;
  }

  extract_data_and_estimates(grid, estimates, config.starting_prb, config.second_hop_prb, config.port);

  equalize(eq_time_spread_sequence,
           eq_time_spread_noise_var,
           time_spread_sequence,
           ch_estimates,
           estimates.get_noise_variance(config.port),
           config.beta_pucch);

  marginalize_w_and_r_out(config);

  unsigned               nof_bits         = std::max(config.nof_harq_ack, config.nof_sr);
  std::array<uint8_t, 2> msg_bits         = {};
  float                  detection_metric = detect_bits(msg_bits, detected_symbol, eq_noise_var, nof_bits);

  // Check whether the computed detection metric is above the threshold, which is set to guarantee a 1% FA probability.
  bool is_msg_ok = (detection_metric > 2.33);

  pucch_uci_message output;

  if (!is_msg_ok) {
    if ((config.nof_harq_ack == 0) && (config.nof_sr == 1)) {
      // Nothing is sent if the UCI message only contains one 0-valued SR bit.
      output.sr[0] = 0;
      // todo: not sure whether we should set this to valid or not.
      output.status = uci_status::valid;
    }
    output.status = uci_status::invalid;
    return output;
  }

  if (config.nof_harq_ack > 0) {
    for (unsigned i = 0; i != config.nof_harq_ack; ++i) {
      output.harq_ack[i] = msg_bits[i];
    }
    // Recall that no SR bits are sent if there's at least one ACK bit.
    output.status = uci_status::valid;
    return output;
  }

  // If we are here, there should only be an SR bit and it should be 1, since nothing is sent for SR = 0 and no ACK.
  output.status = uci_status::invalid;
  if (msg_bits[0] == 1U) {
    output.sr[0]  = 1U;
    output.status = uci_status::valid;
  }
  return output;
}

void pucch_detector_impl::extract_data_and_estimates(const resource_grid_reader& grid,
                                                     const channel_estimate&     estimates,
                                                     unsigned                    first_prb,
                                                     const optional<unsigned>&   second_prb,
                                                     unsigned                    port)
{
  unsigned i_symbol = 0;
  unsigned skip     = 0;
  for (; i_symbol != nof_data_symbols_pre_hop; ++i_symbol, skip += NRE) {
    // Index of the first subcarrier assigned to PUCCH, before hopping.
    unsigned   k_init         = NRE * first_prb;
    span<cf_t> sequence_chunk = span<cf_t>(time_spread_sequence).subspan(skip, NRE);
    grid.get(sequence_chunk, port, 2 * i_symbol + 1, k_init);

    span<const cf_t> tmp = estimates.get_symbol_ch_estimate(2 * i_symbol + 1, port);
    std::copy_n(tmp.begin() + k_init, NRE, ch_estimates.begin() + skip);
  }

  for (; i_symbol != nof_data_symbols; ++i_symbol, skip += NRE) {
    // Index of the first subcarrier assigned to PUCCH, after hopping. Note that we only enter this loop if
    // second_prb.has_value().
    unsigned   k_init         = NRE * second_prb.value();
    span<cf_t> sequence_chunk = span<cf_t>(time_spread_sequence).subspan(skip, NRE);
    grid.get(sequence_chunk, port, 2 * i_symbol + 1, k_init);

    span<const cf_t> tmp = estimates.get_symbol_ch_estimate(2 * i_symbol + 1, port);
    std::copy_n(tmp.begin() + k_init, NRE, ch_estimates.begin() + skip);
  }
}

// Computest the index of the cyclic shift.
static unsigned compute_i_alpha(unsigned                 m0,
                                unsigned                 l,
                                unsigned                 n_id,
                                unsigned                 n_sf,
                                unsigned                 nof_symbols_per_slot,
                                pseudo_random_generator& prg)
{
  // Initialize the pseduorandom generator.
  prg.init(n_id);
  // Create the required PR numbers.
  prg.advance(8 * (nof_symbols_per_slot * n_sf + l));
  std::array<float, 8> c_values = {};
  prg.generate(c_values, 1.0F);

  // Compute n_cs
  unsigned n_cs = std::accumulate(c_values.begin(), c_values.end(), 0.0F, [n = 0U](unsigned a, unsigned b) mutable {
    return (a + static_cast<unsigned>(b) * (1U << (n++)));
  });

  // Compute alpha index - we negate the expression in TS38.211 Section 6.3.2.2.2 to get the conjugated sequence.
  return ((NRE - m0 - n_cs) % NRE);
}

void pucch_detector_impl::marginalize_w_and_r_out(const format1_configuration& config)
{
  unsigned time_domain_occ = config.time_domain_occ;
  unsigned group_index     = config.group_index;
  unsigned sequence_number = config.sequence_number;

  unsigned nof_symbols_per_slot = get_nsymb_per_slot(config.cp);
  // Get slot number in radio frame.
  unsigned n_sf = config.slot.slot_index();

  span<const cf_t> w_star = get_w_star<MAX_N_DATA_SYMBOLS>(nof_data_symbols_pre_hop, time_domain_occ);

  unsigned offset = 0;
  for (unsigned i_symbol = 0; i_symbol != nof_data_symbols_pre_hop; ++i_symbol, offset += NRE) {
    unsigned         i_alpha = compute_i_alpha(config.initial_cyclic_shift,
                                       config.start_symbol_index + (2 * i_symbol + 1),
                                       config.n_id,
                                       n_sf,
                                       nof_symbols_per_slot,
                                       *pseudo_random);
    span<const cf_t> r_star  = low_papr->get(group_index, sequence_number, i_alpha);
    for (unsigned i_elem = 0; i_elem != NRE; ++i_elem) {
      repeated_symbol[offset + i_elem] = eq_time_spread_sequence[offset + i_elem] * w_star[i_symbol] * r_star[i_elem];
    }
  }

  unsigned nof_data_symbols_post_hop = nof_data_symbols - nof_data_symbols_pre_hop;
  w_star                             = get_w_star<MAX_N_DATA_SYMBOLS>(nof_data_symbols_post_hop, time_domain_occ);

  for (unsigned i_symbol = 0; i_symbol != nof_data_symbols_post_hop; ++i_symbol, offset += NRE) {
    unsigned         i_alpha = compute_i_alpha(config.initial_cyclic_shift,
                                       config.start_symbol_index + (2 * (i_symbol + nof_data_symbols_pre_hop) + 1),
                                       config.n_id,
                                       n_sf,
                                       nof_symbols_per_slot,
                                       *pseudo_random);
    span<const cf_t> r_star  = low_papr->get(group_index, sequence_number, i_alpha);
    for (unsigned i_elem = 0; i_elem != NRE; ++i_elem) {
      repeated_symbol[offset + i_elem] = eq_time_spread_sequence[offset + i_elem] * w_star[i_symbol] * r_star[i_elem];
    }
  }

  auto n_repetitions = static_cast<float>(repeated_symbol.size());
  detected_symbol = std::accumulate(repeated_symbol.begin(), repeated_symbol.end(), 0.0F * COMPLEX_J) / n_repetitions;
  eq_noise_var    = std::accumulate(eq_time_spread_noise_var.begin(), eq_time_spread_noise_var.end(), 0.0F) /
                 n_repetitions / n_repetitions;
}