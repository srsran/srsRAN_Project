/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pucch_resource_generator.h"

using namespace srsran;
using namespace srs_du;

struct pucch_grant {
  pucch_format           format;
  ofdm_symbol_range      symbols;
  prb_interval           prbs;
  optional<prb_interval> freq_hop_grant;
  optional<unsigned>     occ_cs_idx;

  bool is_empty() const { return symbols.empty() and prbs.empty(); }
};

static unsigned format1_symb_to_spreading_factor(bounded_integer<unsigned, 4, 14> f1_symbols)
{
  return f1_symbols.to_uint() / 2;
};

static std::vector<pucch_grant> compute_f1_res(pucch_params_option2 params, unsigned bwp_size_rbs, unsigned nof_occ_css)
{
  std::vector<pucch_grant> res_list;
  if (params.f1_intraslot_freq_hopping) {
    for (unsigned rb_idx = 0; rb_idx < bwp_size_rbs / 2 - 1; ++rb_idx) {
      const prb_interval prbs{rb_idx, rb_idx + 1};
      const prb_interval freq_hop_prbs{bwp_size_rbs - 1 - rb_idx, bwp_size_rbs - rb_idx};

      for (unsigned sym_idx = 0; sym_idx + params.nof_f1_symbols.to_uint() <= 14;
           sym_idx += params.nof_f1_symbols.to_uint()) {
        const ofdm_symbol_range symbols{sym_idx, sym_idx + params.nof_f1_symbols.to_uint()};

        // Allocate all OCC and CSS resources for first hop.
        for (unsigned occ_cs_idx = 0; occ_cs_idx < nof_occ_css; ++occ_cs_idx) {
          res_list.push_back(pucch_grant{
              .format = srsran::pucch_format::FORMAT_1, .symbols = symbols, .prbs = prbs, .occ_cs_idx = occ_cs_idx});
          res_list.back().freq_hop_grant.emplace(freq_hop_prbs);
        }
        if (res_list.size() == params.nof_res_f1) {
          break;
        }

        // Allocate all OCC and CSS resources for second hop.
        for (unsigned occ_cs_idx = 0; occ_cs_idx < nof_occ_css; ++occ_cs_idx) {
          res_list.push_back(
              pucch_grant{.format = srsran::pucch_format::FORMAT_1, .symbols = symbols, .prbs = freq_hop_prbs});
          res_list.back().freq_hop_grant.emplace(prbs);
          res_list.back().occ_cs_idx.emplace(occ_cs_idx);
        }
        if (res_list.size() == params.nof_res_f1) {
          break;
        }
      }
      if (res_list.size() == params.nof_res_f1) {
        break;
      }
    }
  } else {
    for (unsigned rb_idx = 0; rb_idx < bwp_size_rbs / 2 - 1; ++rb_idx) {
      const prb_interval prbs_low_spectrum{rb_idx, rb_idx + 1};
      const prb_interval prbs_hi_spectrum{bwp_size_rbs - 1 - rb_idx, bwp_size_rbs - rb_idx};

      for (unsigned sym_idx = 0; sym_idx + params.nof_f1_symbols.to_uint() <= 14;
           sym_idx += params.nof_f1_symbols.to_uint()) {
        const ofdm_symbol_range symbols{sym_idx, sym_idx + params.nof_f1_symbols.to_uint()};

        // Allocate all OCC and CSS resources for first hop.
        for (unsigned occ_cs_idx = 0; occ_cs_idx < nof_occ_css; ++occ_cs_idx) {
          res_list.push_back(
              pucch_grant{.format = srsran::pucch_format::FORMAT_1, .symbols = symbols, .prbs = prbs_low_spectrum});
          res_list.back().occ_cs_idx.emplace(occ_cs_idx);
          if (res_list.size() == params.nof_res_f1) {
            break;
          }
        }
        if (res_list.size() == params.nof_res_f1) {
          break;
        }
      }
      if (res_list.size() == params.nof_res_f1) {
        break;
      }

      for (unsigned sym_idx = 0; sym_idx + params.nof_f1_symbols.to_uint() <= 14;
           sym_idx += params.nof_f1_symbols.to_uint()) {
        const ofdm_symbol_range symbols{sym_idx, sym_idx + params.nof_f1_symbols.to_uint()};

        // Allocate all OCC and CSS resources for first hop.
        for (unsigned occ_cs_idx = 0; occ_cs_idx < nof_occ_css; ++occ_cs_idx) {
          res_list.push_back(
              pucch_grant{.format = srsran::pucch_format::FORMAT_1, .symbols = symbols, .prbs = prbs_hi_spectrum});
          res_list.back().occ_cs_idx.emplace(occ_cs_idx);
          if (res_list.size() == params.nof_res_f1) {
            break;
          }
        }
        if (res_list.size() == params.nof_res_f1) {
          break;
        }
      }

      if (res_list.size() == params.nof_res_f1) {
        break;
      }
    }
  }

  return res_list;
}

static std::vector<pucch_grant> compute_f2_res(pucch_params_option2 params, unsigned bwp_size_rbs)
{
  // Compute the number of symbols and PRBs for F2.
  std::vector<pucch_grant> res_list;
  unsigned                 f2_max_rbs = 1;
  unsigned                 nof_f2_symbols =
      std::ceil(params.max_f2_payload_bits / (8 * 2 * f2_max_rbs * to_max_code_rate_float(params.max_f2_code_rate)));
  while (nof_f2_symbols > 2) {
    f2_max_rbs = f2_max_rbs + 1;
    nof_f2_symbols =
        std::ceil(params.max_f2_payload_bits / (8 * 2 * f2_max_rbs * to_max_code_rate_float(params.max_f2_code_rate)));
  }

  if (params.f2_intraslot_freq_hopping) {
    for (unsigned rb_idx = 0; rb_idx < bwp_size_rbs / 2 - f2_max_rbs; rb_idx += f2_max_rbs) {
      const prb_interval prbs{rb_idx, rb_idx + f2_max_rbs};
      const prb_interval freq_hop_prbs{bwp_size_rbs - f2_max_rbs - rb_idx, bwp_size_rbs - rb_idx};

      for (unsigned sym_idx = 0; sym_idx + nof_f2_symbols <= 14; sym_idx += nof_f2_symbols) {
        const ofdm_symbol_range symbols{sym_idx, sym_idx + nof_f2_symbols};

        // Allocate resources for first hop.
        res_list.push_back(pucch_grant{.format = srsran::pucch_format::FORMAT_2, .symbols = symbols, .prbs = prbs});
        res_list.back().freq_hop_grant.emplace(freq_hop_prbs);
        if (res_list.size() == params.nof_res_f2) {
          break;
        }

        // Allocate resources for second hop.
        res_list.push_back(
            pucch_grant{.format = srsran::pucch_format::FORMAT_2, .symbols = symbols, .prbs = freq_hop_prbs});
        res_list.back().freq_hop_grant.emplace(prbs);
        if (res_list.size() == params.nof_res_f2) {
          break;
        }
      }
      if (res_list.size() == params.nof_res_f2) {
        break;
      }
    }
  } else {
    for (unsigned rb_idx = 0; rb_idx < bwp_size_rbs / 2 - f2_max_rbs; rb_idx += f2_max_rbs) {
      const prb_interval prbs_low_spectrum{rb_idx, rb_idx + f2_max_rbs};
      const prb_interval prbs_hi_spectrum{bwp_size_rbs - f2_max_rbs - rb_idx, bwp_size_rbs - rb_idx};

      for (unsigned sym_idx = 0; sym_idx + nof_f2_symbols <= 14; sym_idx += nof_f2_symbols) {
        const ofdm_symbol_range symbols{sym_idx, sym_idx + nof_f2_symbols};

        // Allocate all OCC and CSS resources for first hop.
        res_list.push_back(
            pucch_grant{.format = srsran::pucch_format::FORMAT_2, .symbols = symbols, .prbs = prbs_low_spectrum});
        if (res_list.size() == params.nof_res_f2) {
          break;
        }
      }
      if (res_list.size() == params.nof_res_f2) {
        break;
      }

      for (unsigned sym_idx = 0; sym_idx + nof_f2_symbols <= 14; sym_idx += nof_f2_symbols) {
        const ofdm_symbol_range symbols{sym_idx, sym_idx + nof_f2_symbols};

        // Allocate all OCC and CSS resources for first hop.
        res_list.push_back(
            pucch_grant{.format = srsran::pucch_format::FORMAT_2, .symbols = symbols, .prbs = prbs_hi_spectrum});
        if (res_list.size() == params.nof_res_f2) {
          break;
        }
      }

      if (res_list.size() == params.nof_res_f2) {
        break;
      }
    }
  }

  return res_list;
}

static std::vector<cell_pucch_resource> compute_resources(pucch_params_option2 params, unsigned bwp_size_rbs)
{
  // Compute the resources until max number of RBs or Number of requested resource is reached.
  const unsigned nof_occ_codes = params.f1_occ_supported ? format1_symb_to_spreading_factor(params.nof_f1_symbols) : 1;
  const unsigned nof_cs        = format1_cp_step_to_uint(params.format1_cp);

  const std::vector<pucch_grant> pucch_f1_resource_list = compute_f1_res(params, bwp_size_rbs, nof_cs * nof_occ_codes);
  const std::vector<pucch_grant> pucch_f2_resource_list = compute_f2_res(params, bwp_size_rbs);

  // Focus on Format 1 first.
  std::vector<cell_pucch_resource> resource_list;

  //
  unsigned pucch_f2_prb_ref_low_spectrum = 0;
  unsigned pucch_f2_prb_ref_hi_spectrum  = 0;
  for (const auto& res_f1 : pucch_f1_resource_list) {
    const unsigned      cell_res_id = static_cast<unsigned>(resource_list.size());
    cell_pucch_resource res{.cell_res_id = cell_res_id, .starting_prb = res_f1.prbs.start()};
    if (res_f1.freq_hop_grant.has_value()) {
      res.second_hop_prb.emplace(res_f1.freq_hop_grant.value().start());
    }
    pucch_format_1_cfg format1{.starting_sym_idx = res_f1.symbols.start()};
    if (res_f1.prbs.start() < bwp_size_rbs / 2 - 2) {
      pucch_f2_prb_ref_low_spectrum = std::max(pucch_f2_prb_ref_low_spectrum, res_f1.prbs.start() + 1);
    } else if (res_f1.prbs.start() > bwp_size_rbs / 2 + 1) {
      pucch_f2_prb_ref_hi_spectrum = std::max(pucch_f2_prb_ref_hi_spectrum, bwp_size_rbs - res_f1.prbs.start());
    }

    format1.nof_symbols       = res_f1.symbols.length();
    const unsigned max_nof_cs = format1_cp_step_to_uint(nof_cyclic_shifts::twelve);
    const unsigned cs_step    = max_nof_cs / nof_cs;
    srsran_assert(res_f1.occ_cs_idx.has_value(),
                  "The index needed to compute OCC code and cyclic shift have not been found");
    const unsigned initial_cyclic_shift = (res_f1.occ_cs_idx.value() * cs_step) % max_nof_cs;
    format1.initial_cyclic_shift        = initial_cyclic_shift;
    format1.time_domain_occ             = res_f1.occ_cs_idx.value() / nof_cs;
    res.format_params.emplace<pucch_format_1_cfg>(format1);
    res.format = pucch_format::FORMAT_1;
    resource_list.push_back(res);
  }

  for (const auto& res_f2 : pucch_f2_resource_list) {
    const unsigned      cell_res_id = static_cast<unsigned>(resource_list.size());
    cell_pucch_resource res{.cell_res_id = cell_res_id};
    if (res_f2.prbs.start() < bwp_size_rbs / 2 - res_f2.prbs.length() - 1) {
      res.starting_prb = res_f2.prbs.start() + pucch_f2_prb_ref_low_spectrum;
      if (res_f2.freq_hop_grant.has_value()) {
        res.second_hop_prb.emplace(res_f2.freq_hop_grant.value().start() - pucch_f2_prb_ref_hi_spectrum);
      }
    } else if (res_f2.prbs.start() > bwp_size_rbs / 2 + 1) {
      res.starting_prb = res_f2.prbs.start() - pucch_f2_prb_ref_hi_spectrum;
      if (res_f2.freq_hop_grant.has_value()) {
        res.second_hop_prb.emplace(res_f2.freq_hop_grant.value().start() + pucch_f2_prb_ref_low_spectrum);
      }
    } else {
      continue;
    }

    pucch_format_2_3_cfg format2{.starting_sym_idx = res_f2.symbols.start()};
    format2.nof_symbols = res_f2.symbols.length();
    format2.nof_prbs    = res_f2.prbs.length();
    res.format_params.emplace<pucch_format_2_3_cfg>(format2);
    res.format = pucch_format::FORMAT_2;
    resource_list.push_back(res);
  }

  return resource_list;
}

static std::tuple<unsigned, unsigned> compute_nof_f1_f2_resources(pucch_params_option1 params)
{
  // Compute the resources until max number of RBs or Number of requested resource is reached.
  const unsigned nof_occ_codes = params.f1_occ_supported ? format1_symb_to_spreading_factor(params.nof_f1_symbols) : 1;
  const unsigned nof_cs        = format1_cp_step_to_uint(params.format1_cp);
  const unsigned nof_symbols_per_slot = 14;

  unsigned f2_max_rbs = 1;
  unsigned nof_f2_symbols =
      std::ceil(params.max_f2_payload_bits / (8 * 2 * f2_max_rbs * to_max_code_rate_float(params.max_f2_code_rate)));
  while (nof_f2_symbols > 2) {
    f2_max_rbs = f2_max_rbs + 1;
    nof_f2_symbols =
        std::ceil(params.max_f2_payload_bits / (8 * 2 * f2_max_rbs * to_max_code_rate_float(params.max_f2_code_rate)));
  }

  const unsigned required_f1_rbs = 1;
  const unsigned rbs_per_f1_res  = params.f1_intraslot_freq_hopping ? 2 * required_f1_rbs : required_f1_rbs;
  const unsigned nof_f1_res      = nof_occ_codes * nof_cs * (nof_symbols_per_slot / params.nof_f1_symbols.to_uint()) *
                              (params.max_pucch_f1_rbs / rbs_per_f1_res);

  const unsigned rbs_per_f2_res = params.f2_intraslot_freq_hopping ? 2 * f2_max_rbs : f2_max_rbs;
  const unsigned nof_f2_res     = (nof_symbols_per_slot / nof_f2_symbols) * (params.max_pucch_f2_rbs / rbs_per_f2_res);

  return std::tuple<unsigned, unsigned>{nof_f1_res, nof_f2_res};
}

std::vector<cell_pucch_resource>
srsran::srs_du::generate_pucch_resource_list(variant<pucch_params_option1, pucch_params_option2> params,
                                             unsigned                                            bwp_size)
{
  // Check what parameters have been given and decide what needs to be computed.

  if (variant_holds_alternative<pucch_params_option1>(params)) {
    // Compute the resources until max number of RBs or Number of requested resource is reached.
    const pucch_params_option1 params_1 = variant_get<pucch_params_option1>(params);
    pucch_params_option2       params_2{.format1_cp                = params_1.format1_cp,
                                        .f1_occ_supported          = params_1.f1_occ_supported,
                                        .nof_f1_symbols            = params_1.nof_f1_symbols,
                                        .f1_intraslot_freq_hopping = params_1.f1_intraslot_freq_hopping,
                                        .max_f2_payload_bits       = params_1.max_f2_payload_bits,
                                        .max_f2_code_rate          = params_1.max_f2_code_rate,
                                        .f2_intraslot_freq_hopping = params_1.f2_intraslot_freq_hopping};

    const std::tuple<unsigned, unsigned> nof_pucch_res = compute_nof_f1_f2_resources(params_1);
    params_2.nof_res_f1                                = std::get<0>(nof_pucch_res);
    params_2.nof_res_f2                                = std::get<1>(nof_pucch_res);

    return compute_resources(params_2, bwp_size);
  } else if (variant_holds_alternative<pucch_params_option2>(params)) {
    return compute_resources(variant_get<pucch_params_option2>(params), bwp_size);
  }

  return {};
}
