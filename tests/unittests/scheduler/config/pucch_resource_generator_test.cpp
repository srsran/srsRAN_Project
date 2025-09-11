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

#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "srsran/scheduler/config/pucch_resource_generator.h"
#include "srsran/support/test_utils.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace sched_pucch_gen {
// Test struct.
struct pucch_gen_params {
  unsigned                         nof_res_f0;
  unsigned                         nof_res_f1;
  unsigned                         nof_res_f2;
  unsigned                         nof_res_f3;
  unsigned                         nof_res_f4;
  bounded_integer<unsigned, 1, 2>  f0_nof_symbols{2};
  bool                             f0_intraslot_freq_hopping{false};
  pucch_nof_cyclic_shifts          nof_cyc_shifts{pucch_nof_cyclic_shifts::no_cyclic_shift};
  bool                             occ_supported{false};
  bounded_integer<unsigned, 4, 14> f1_nof_symbols{14};
  bool                             f1_intraslot_freq_hopping{false};
  bounded_integer<unsigned, 1, 2>  f2_nof_symbols{1};
  unsigned                         max_nof_rbs{1};
  std::optional<unsigned>          max_payload_bits;
  max_pucch_code_rate              max_code_rate{max_pucch_code_rate::dot_25};
  bool                             f2_intraslot_freq_hopping{false};
  bounded_integer<unsigned, 4, 14> f3_nof_symbols{4};
  bool                             f3_intraslot_freq_hopping{false};
  bool                             f3_additional_dmrs{false};
  bool                             f3_pi2_bpsk{false};
  bounded_integer<unsigned, 4, 14> f4_nof_symbols{4};
  bool                             f4_intraslot_freq_hopping{false};
  bool                             f4_additional_dmrs{false};
  bool                             f4_pi2_bpsk{false};
  pucch_f4_occ_len                 f4_occ_length{pucch_f4_occ_len::n2};
};

// Dummy function overload of template <typename T> void testing::internal::PrintTo(const T& value, ::std::ostream* os).
// This prevents valgrind from complaining about uninitialized variables.
std::ostream& operator<<(std::ostream& os, const pucch_gen_params& params)
{
  std::string f0_f1_str;
  if (params.nof_res_f0 != 0) {
    f0_f1_str = fmt::format("F0: nof_res={} nof_symbols={} freq_hop={} nof_cyc_shifts={} occ_supported={}",
                            params.nof_res_f0,
                            params.f0_nof_symbols,
                            params.f0_intraslot_freq_hopping,
                            fmt::underlying(params.nof_cyc_shifts),
                            params.occ_supported);
  } else {
    f0_f1_str = fmt::format("F1: nof_res={} nof_symbols={} freq_hop={}",
                            params.nof_res_f1,
                            params.f1_nof_symbols,
                            params.f1_intraslot_freq_hopping);
  }
  std::string f2_f3_f4_str;
  if (params.nof_res_f2 != 0) {
    f2_f3_f4_str = fmt::format(
        "F2: nof_res={} nof_symbols={} max_nof_rbs={} {} max_code_rate={} freq_hop={}",
        params.nof_res_f2,
        params.f2_nof_symbols,
        params.max_nof_rbs,
        params.max_payload_bits.has_value() ? fmt::format("max_payload_bits={}", params.max_payload_bits.value()) : "",
        to_max_code_rate_float(params.max_code_rate),
        params.f2_intraslot_freq_hopping);
  } else if (params.nof_res_f3 != 0) {
    f2_f3_f4_str = fmt::format(
        "F3: nof_res={} nof_symbols={} max_nof_rbs={} {} max_code_rate={} freq_hop={} add_dmrs={} pi2_bpsk={}",
        params.nof_res_f3,
        params.f3_nof_symbols,
        params.max_nof_rbs,
        params.max_payload_bits.has_value() ? fmt::format("max_payload_bits={}", params.max_payload_bits.value()) : "",
        to_max_code_rate_float(params.max_code_rate),
        params.f3_intraslot_freq_hopping,
        params.f3_additional_dmrs,
        params.f3_pi2_bpsk);
  } else {
    f2_f3_f4_str = fmt::format(
        "F4: nof_res={} nof_symbols={} {} max_code_rate={} freq_hop={} add_dmrs={} pi2_bpsk={} occ_len={}",
        params.nof_res_f4,
        params.f4_nof_symbols,
        params.max_payload_bits.has_value() ? fmt::format("max_payload_bits={}", params.max_payload_bits.value()) : "",
        to_max_code_rate_float(params.max_code_rate),
        params.f4_intraslot_freq_hopping,
        params.f4_additional_dmrs,
        params.f4_pi2_bpsk,
        static_cast<unsigned>(params.f4_occ_length));
  }
  fmt::print(os, "{} {}", f0_f1_str, f2_f3_f4_str);

  return os;
}

struct pucch_cfg_builder_params {
  unsigned nof_res_f0_harq = 0;
  unsigned nof_res_f1_harq = 3;
  unsigned nof_res_f2_harq = 6;
  unsigned nof_res_f3_harq = 6;
  unsigned nof_res_f4_harq = 6;
  unsigned nof_harq_cfg    = 1;
  unsigned nof_res_sr      = 2;
  unsigned nof_res_csi     = 1;
};

std::ostream& operator<<(std::ostream& os, const pucch_cfg_builder_params& params)
{
  fmt::print(os,
             "f0_harq={} f1_harq={} f2_harq={} f3_harq={} f4_harq={} nof_harq_cfg={} nof_sr={} nof_csi={}",
             params.nof_res_f0_harq,
             params.nof_res_f1_harq,
             params.nof_res_f2_harq,
             params.nof_res_f3_harq,
             params.nof_res_f4_harq,
             params.nof_harq_cfg,
             params.nof_res_sr,
             params.nof_res_csi);

  return os;
}

} // namespace sched_pucch_gen

using namespace sched_pucch_gen;

// This class implements a resource grid for PUCCH resources. It allocates the resource on the grid and verify whether a
// resource collides with the previously allocated ones.
// For FORMAT 1, it verifies whether the resource has a different OCC or CS code with respect resource allocated on the
// same PRBs and symbols.
class pucch_grid
{
public:
  pucch_grid(unsigned nof_rbs_, unsigned nof_symbols_) : nof_rbs{nof_rbs_}, nof_symbols{nof_symbols_}
  {
    grid.resize(nof_rbs * nof_symbols);
    for (auto& grid_elem : grid) {
      for (auto& occ_cs : grid_elem.allocated_occ_cs_list) {
        occ_cs = false;
      }
    }
  }

  // Allocate the resource on the grid.
  void add_resource(const pucch_resource& res)
  {
    if (res.format == pucch_format::FORMAT_0) {
      srsran_assert(std::holds_alternative<pucch_format_0_cfg>(res.format_params), "Expected PUCCH Format 0");

      if (res.second_hop_prb.has_value()) {
        srsran_assert(res.nof_symbols == 2U, "Intra-slot freq. hopping for PUCCH Format 0 requires 2 symbols");
        // First hop.
        for (unsigned sym_idx = res.starting_sym_idx, sym_stop = res.starting_sym_idx + res.nof_symbols / 2;
             sym_idx != sym_stop;
             ++sym_idx) {
          auto& grid_elem        = grid[sym_idx + nof_symbols * res.starting_prb];
          grid_elem.format       = pucch_format::FORMAT_0;
          grid_elem.element_used = true;
        }
        // Second hop.
        for (unsigned sym_idx  = res.starting_sym_idx + res.nof_symbols / 2,
                      sym_stop = res.starting_sym_idx + res.nof_symbols;
             sym_idx != sym_stop;
             ++sym_idx) {
          auto& grid_elem        = grid[sym_idx + nof_symbols * res.second_hop_prb.value()];
          grid_elem.format       = pucch_format::FORMAT_0;
          grid_elem.element_used = true;
        }
      } else {
        for (unsigned sym_idx = res.starting_sym_idx, sym_stop = res.starting_sym_idx + res.nof_symbols;
             sym_idx != sym_stop;
             ++sym_idx) {
          auto& grid_elem        = grid[sym_idx + nof_symbols * res.starting_prb];
          grid_elem.format       = pucch_format::FORMAT_0;
          grid_elem.element_used = true;
        }
      }

    } else if (res.format == pucch_format::FORMAT_1) {
      srsran_assert(std::holds_alternative<pucch_format_1_cfg>(res.format_params), "Expected PUCCH Format 1");
      const auto& res_f1 = std::get<pucch_format_1_cfg>(res.format_params);

      if (res.second_hop_prb.has_value()) {
        // First hop.
        for (unsigned sym_idx = res.starting_sym_idx, sym_stop = res.starting_sym_idx + res.nof_symbols / 2;
             sym_idx != sym_stop;
             ++sym_idx) {
          auto& grid_elem           = grid[sym_idx + nof_symbols * res.starting_prb];
          grid_elem.format          = pucch_format::FORMAT_1;
          grid_elem.element_used    = true;
          const unsigned occ_cs_idx = res_f1.initial_cyclic_shift + res_f1.time_domain_occ * max_nof_cs;
          srsran_assert(res_f1.initial_cyclic_shift < max_nof_cs, "Initial Cyclic Shift exceeds maximum value 11");
          srsran_assert(res_f1.time_domain_occ < max_nof_occ, " Time Domain OCC exceeds maximum value 7");
          grid_elem.allocated_occ_cs_list[occ_cs_idx] = true;
        }
        // Second hop.
        for (unsigned sym_idx  = res.starting_sym_idx + res.nof_symbols / 2,
                      sym_stop = res.starting_sym_idx + res.nof_symbols;
             sym_idx != sym_stop;
             ++sym_idx) {
          auto& grid_elem           = grid[sym_idx + nof_symbols * res.second_hop_prb.value()];
          grid_elem.format          = pucch_format::FORMAT_1;
          grid_elem.element_used    = true;
          const unsigned occ_cs_idx = res_f1.initial_cyclic_shift + res_f1.time_domain_occ * max_nof_cs;
          srsran_assert(res_f1.initial_cyclic_shift < max_nof_cs, "Initial Cyclic Shift exceeds maximum value 11");
          srsran_assert(res_f1.time_domain_occ < max_nof_occ, " Time Domain OCC exceeds maximum value 7");
          grid_elem.allocated_occ_cs_list[occ_cs_idx] = true;
        }
      } else {
        for (unsigned sym_idx = res.starting_sym_idx, sym_stop = res.starting_sym_idx + res.nof_symbols;
             sym_idx != sym_stop;
             ++sym_idx) {
          auto& grid_elem           = grid[sym_idx + nof_symbols * res.starting_prb];
          grid_elem.format          = pucch_format::FORMAT_1;
          grid_elem.element_used    = true;
          const unsigned occ_cs_idx = res_f1.initial_cyclic_shift + res_f1.time_domain_occ * max_nof_cs;
          srsran_assert(res_f1.initial_cyclic_shift < max_nof_cs, "Initial Cyclic Shift exceeds maximum value 11");
          srsran_assert(res_f1.time_domain_occ < max_nof_occ, " Time Domain OCC exceeds maximum value 7");
          grid_elem.allocated_occ_cs_list[occ_cs_idx] = true;
        }
      }
    } else if (res.format == pucch_format::FORMAT_2) {
      srsran_assert(std::holds_alternative<pucch_format_2_3_cfg>(res.format_params), "Expected PUCCH Format 2");
      const auto& res_f2 = std::get<pucch_format_2_3_cfg>(res.format_params);

      if (res.second_hop_prb.has_value()) {
        srsran_assert(res.nof_symbols == 2U, "Intra-slot freq. hopping for PUCCH Format 0 requires 2 symbols");
        // First hop.
        for (unsigned rb_idx = res.starting_prb, rb_stop = res.starting_prb + res_f2.nof_prbs; rb_idx != rb_stop;
             ++rb_idx) {
          for (unsigned sym_idx = res.starting_sym_idx, sym_stop = res.starting_sym_idx + res.nof_symbols / 2;
               sym_idx != sym_stop;
               ++sym_idx) {
            auto& grid_elem        = grid[sym_idx + nof_symbols * rb_idx];
            grid_elem.element_used = true;
            grid_elem.format       = pucch_format::FORMAT_2;
          }
        }
        // Second hop.
        for (unsigned rb_idx = res.second_hop_prb.value(), rb_stop = res.second_hop_prb.value() + res_f2.nof_prbs;
             rb_idx != rb_stop;
             ++rb_idx) {
          for (unsigned sym_idx  = res.starting_sym_idx + res.nof_symbols / 2,
                        sym_stop = res.starting_sym_idx + res.nof_symbols;
               sym_idx != sym_stop;
               ++sym_idx) {
            auto& grid_elem        = grid[sym_idx + nof_symbols * rb_idx];
            grid_elem.element_used = true;
            grid_elem.format       = pucch_format::FORMAT_2;
          }
        }
      } else {
        for (unsigned rb_idx = res.starting_prb, rb_stop = res.starting_prb + res_f2.nof_prbs; rb_idx != rb_stop;
             ++rb_idx) {
          for (unsigned sym_idx = res.starting_sym_idx, sym_stop = res.starting_sym_idx + res.nof_symbols;
               sym_idx != sym_stop;
               ++sym_idx) {
            auto& grid_elem        = grid[sym_idx + nof_symbols * rb_idx];
            grid_elem.element_used = true;
            grid_elem.format       = pucch_format::FORMAT_2;
          }
        }
      }
    } else if (res.format == pucch_format::FORMAT_3) {
      srsran_assert(std::holds_alternative<pucch_format_2_3_cfg>(res.format_params), "Expected PUCCH Format 3");
      const auto& res_f3 = std::get<pucch_format_2_3_cfg>(res.format_params);

      if (res.second_hop_prb.has_value()) {
        // First hop.
        for (unsigned rb_idx = res.starting_prb, rb_stop = res.starting_prb + res_f3.nof_prbs; rb_idx != rb_stop;
             ++rb_idx) {
          for (unsigned sym_idx = res.starting_sym_idx, sym_stop = res.starting_sym_idx + res.nof_symbols / 2;
               sym_idx != sym_stop;
               ++sym_idx) {
            auto& grid_elem        = grid[sym_idx + nof_symbols * rb_idx];
            grid_elem.element_used = true;
            grid_elem.format       = pucch_format::FORMAT_3;
          }
        }
        // Second hop.
        for (unsigned rb_idx = res.second_hop_prb.value(), rb_stop = res.second_hop_prb.value() + res_f3.nof_prbs;
             rb_idx != rb_stop;
             ++rb_idx) {
          for (unsigned sym_idx  = res.starting_sym_idx + res.nof_symbols / 2,
                        sym_stop = res.starting_sym_idx + res.nof_symbols;
               sym_idx != sym_stop;
               ++sym_idx) {
            auto& grid_elem        = grid[sym_idx + nof_symbols * rb_idx];
            grid_elem.element_used = true;
            grid_elem.format       = pucch_format::FORMAT_3;
          }
        }
      } else {
        for (unsigned rb_idx = res.starting_prb, rb_stop = res.starting_prb + res_f3.nof_prbs; rb_idx != rb_stop;
             ++rb_idx) {
          for (unsigned sym_idx = res.starting_sym_idx, sym_stop = res.starting_sym_idx + res.nof_symbols;
               sym_idx != sym_stop;
               ++sym_idx) {
            auto& grid_elem        = grid[sym_idx + nof_symbols * rb_idx];
            grid_elem.element_used = true;
            grid_elem.format       = pucch_format::FORMAT_3;
          }
        }
      }
    } else if (res.format == pucch_format::FORMAT_4) {
      srsran_assert(std::holds_alternative<pucch_format_4_cfg>(res.format_params), "Expected PUCCH Format 4");

      if (res.second_hop_prb.has_value()) {
        // First hop.
        for (unsigned rb_idx = res.starting_prb, rb_stop = res.starting_prb + 1; rb_idx != rb_stop; ++rb_idx) {
          for (unsigned sym_idx = res.starting_sym_idx, sym_stop = res.starting_sym_idx + res.nof_symbols / 2;
               sym_idx != sym_stop;
               ++sym_idx) {
            auto& grid_elem        = grid[sym_idx + nof_symbols * rb_idx];
            grid_elem.element_used = true;
            grid_elem.format       = pucch_format::FORMAT_4;
          }
        }
        // Second hop.
        for (unsigned rb_idx = res.second_hop_prb.value(), rb_stop = res.second_hop_prb.value() + 1; rb_idx != rb_stop;
             ++rb_idx) {
          for (unsigned sym_idx  = res.starting_sym_idx + res.nof_symbols / 2,
                        sym_stop = res.starting_sym_idx + res.nof_symbols;
               sym_idx != sym_stop;
               ++sym_idx) {
            auto& grid_elem        = grid[sym_idx + nof_symbols * rb_idx];
            grid_elem.element_used = true;
            grid_elem.format       = pucch_format::FORMAT_4;
          }
        }
      } else {
        for (unsigned rb_idx = res.starting_prb, rb_stop = res.starting_prb + 1; rb_idx != rb_stop; ++rb_idx) {
          for (unsigned sym_idx = res.starting_sym_idx, sym_stop = res.starting_sym_idx + res.nof_symbols;
               sym_idx != sym_stop;
               ++sym_idx) {
            auto& grid_elem        = grid[sym_idx + nof_symbols * rb_idx];
            grid_elem.element_used = true;
            grid_elem.format       = pucch_format::FORMAT_4;
          }
        }
      }
    }
  }

  // Verify if the given resource collides in the grid with the previously allocated resources.
  bool verify_collision(const pucch_resource& res) const
  {
    if (res.format == pucch_format::FORMAT_0) {
      srsran_assert(std::holds_alternative<pucch_format_0_cfg>(res.format_params), "Expected PUCCH Format 0");
      // Intra-slot frequency hopping.
      if (res.second_hop_prb.has_value()) {
        // First hop.
        for (unsigned sym_idx = res.starting_sym_idx, sym_stop = res.starting_sym_idx + res.nof_symbols / 2;
             sym_idx != sym_stop;
             ++sym_idx) {
          const auto& grid_elem = grid[sym_idx + nof_symbols * res.starting_prb];
          if (grid_elem.element_used) {
            return true;
          }
        }
        // Second hop.
        for (unsigned sym_idx  = res.starting_sym_idx + res.nof_symbols / 2,
                      sym_stop = res.starting_sym_idx + res.nof_symbols;
             sym_idx != sym_stop;
             ++sym_idx) {
          const auto& grid_elem = grid[sym_idx + nof_symbols * res.second_hop_prb.value()];
          if (grid_elem.element_used) {
            return true;
          }
        }

      }
      // No intra-slot frequency hopping.
      else {
        for (unsigned sym_idx = res.starting_sym_idx, sym_stop = res.starting_sym_idx + res.nof_symbols;
             sym_idx != sym_stop;
             ++sym_idx) {
          const auto& grid_elem = grid[sym_idx + nof_symbols * res.starting_prb];
          if (grid_elem.element_used) {
            return true;
          }
        }
      }
    } else if (res.format == pucch_format::FORMAT_1) {
      srsran_assert(std::holds_alternative<pucch_format_1_cfg>(res.format_params), "Expected PUCCH Format 1");
      const auto& res_f1 = std::get<pucch_format_1_cfg>(res.format_params);
      // Intra-slot frequency hopping.
      if (res.second_hop_prb.has_value()) {
        // First hop.
        for (unsigned sym_idx = res.starting_sym_idx, sym_stop = res.starting_sym_idx + res.nof_symbols / 2;
             sym_idx != sym_stop;
             ++sym_idx) {
          const auto& grid_elem = grid[sym_idx + nof_symbols * res.starting_prb];
          if (grid_elem.element_used) {
            if (grid_elem.format != pucch_format::FORMAT_1) {
              return true;
            }
            const unsigned occ_cs_idx = res_f1.initial_cyclic_shift + res_f1.time_domain_occ * max_nof_cs;
            srsran_assert(res_f1.initial_cyclic_shift < max_nof_cs, "Initial Cyclic Shift exceeds maximum value 11");
            srsran_assert(res_f1.time_domain_occ < max_nof_occ, " Time Domain OCC exceeds maximum value 7");
            if (grid_elem.allocated_occ_cs_list[occ_cs_idx]) {
              return true;
            }
          }
        }
        // Second hop.
        for (unsigned sym_idx  = res.starting_sym_idx + res.nof_symbols / 2,
                      sym_stop = res.starting_sym_idx + res.nof_symbols;
             sym_idx != sym_stop;
             ++sym_idx) {
          const auto& grid_elem = grid[sym_idx + nof_symbols * res.second_hop_prb.value()];
          if (grid_elem.element_used) {
            if (grid_elem.format != pucch_format::FORMAT_1) {
              return true;
            }
            const unsigned occ_cs_idx = res_f1.initial_cyclic_shift + res_f1.time_domain_occ * max_nof_cs;
            srsran_assert(res_f1.initial_cyclic_shift < max_nof_cs, "Initial Cyclic Shift exceeds maximum value 11");
            srsran_assert(res_f1.time_domain_occ < max_nof_occ, " Time Domain OCC exceeds maximum value 7");
            if (grid_elem.allocated_occ_cs_list[occ_cs_idx]) {
              return true;
            }
          }
        }

      }
      // No intra-slot frequency hopping.
      else {
        for (unsigned sym_idx = res.starting_sym_idx, sym_stop = res.starting_sym_idx + res.nof_symbols;
             sym_idx != sym_stop;
             ++sym_idx) {
          const auto& grid_elem = grid[sym_idx + nof_symbols * res.starting_prb];
          if (grid_elem.element_used) {
            if (grid_elem.format != pucch_format::FORMAT_1) {
              return true;
            }
            const unsigned occ_cs_idx = res_f1.initial_cyclic_shift + res_f1.time_domain_occ * max_nof_cs;
            srsran_assert(res_f1.initial_cyclic_shift < max_nof_cs, "Initial Cyclic Shift exceeds maximum value 11");
            srsran_assert(res_f1.time_domain_occ < max_nof_occ, " Time Domain OCC exceeds maximum value 7");
            if (grid_elem.allocated_occ_cs_list[occ_cs_idx]) {
              return true;
            }
          }
        }
      }
    } else if (res.format == pucch_format::FORMAT_2 or res.format == pucch_format::FORMAT_3) {
      srsran_assert(std::holds_alternative<pucch_format_2_3_cfg>(res.format_params), "Expected PUCCH Format 2");
      const auto& res_f2_f3 = std::get<pucch_format_2_3_cfg>(res.format_params);

      // Intra-slot frequency hopping.
      if (res.second_hop_prb.has_value()) {
        // First hop.
        for (unsigned rb_idx = res.starting_prb, rb_stop = res.starting_prb + res_f2_f3.nof_prbs; rb_idx != rb_stop;
             ++rb_idx) {
          for (unsigned sym_idx = res.starting_sym_idx, sym_stop = res.starting_sym_idx + res.nof_symbols / 2;
               sym_idx != sym_stop;
               ++sym_idx) {
            const auto& grid_unit = grid[sym_idx + nof_symbols * rb_idx];
            if (grid_unit.element_used) {
              return true;
            }
          }
        }
        // Second hop.
        for (unsigned rb_idx = res.second_hop_prb.value(), rb_stop = res.second_hop_prb.value() + res_f2_f3.nof_prbs;
             rb_idx != rb_stop;
             ++rb_idx) {
          for (unsigned sym_idx  = res.starting_sym_idx + res.nof_symbols / 2,
                        sym_stop = res.starting_sym_idx + res.nof_symbols;
               sym_idx != sym_stop;
               ++sym_idx) {
            const auto& grid_unit = grid[sym_idx + nof_symbols * rb_idx];
            if (grid_unit.element_used) {
              return true;
            }
          }
        }
      }
      // No intra-slot frequency hopping.
      else {
        for (unsigned rb_idx = res.starting_prb, rb_stop = res.starting_prb + res_f2_f3.nof_prbs; rb_idx != rb_stop;
             ++rb_idx) {
          for (unsigned sym_idx = res.starting_sym_idx, sym_stop = res.starting_sym_idx + res.nof_symbols;
               sym_idx != sym_stop;
               ++sym_idx) {
            const auto& grid_unit = grid[sym_idx + nof_symbols * rb_idx];
            if (grid_unit.element_used) {
              return true;
            }
          }
        }
      }
    } else if (res.format == pucch_format::FORMAT_4) {
      srsran_assert(std::holds_alternative<pucch_format_4_cfg>(res.format_params), "Expected PUCCH Format 4");
      const auto& res_f4 = std::get<pucch_format_4_cfg>(res.format_params);

      // Intra-slot frequency hopping.
      if (res.second_hop_prb.has_value()) {
        // First hop.
        for (unsigned rb_idx = res.starting_prb, rb_stop = res.starting_prb + 1; rb_idx != rb_stop; ++rb_idx) {
          for (unsigned sym_idx = res.starting_sym_idx, sym_stop = res.starting_sym_idx + res.nof_symbols / 2;
               sym_idx != sym_stop;
               ++sym_idx) {
            const auto& grid_unit = grid[sym_idx + nof_symbols * rb_idx];
            if (grid_unit.element_used) {
              if (grid_unit.format != pucch_format::FORMAT_4) {
                return true;
              }
              const auto occ_index = static_cast<unsigned>(res_f4.occ_index);
              srsran_assert(occ_index < static_cast<unsigned>(res_f4.occ_length), " OCC index exceeds OCC length");
              if (grid_unit.allocated_occ_cs_list[occ_index]) {
                return true;
              }
            }
          }
        }
        // Second hop.
        for (unsigned rb_idx = res.second_hop_prb.value(), rb_stop = res.second_hop_prb.value() + 1; rb_idx != rb_stop;
             ++rb_idx) {
          for (unsigned sym_idx  = res.starting_sym_idx + res.nof_symbols / 2,
                        sym_stop = res.starting_sym_idx + res.nof_symbols;
               sym_idx != sym_stop;
               ++sym_idx) {
            const auto& grid_unit = grid[sym_idx + nof_symbols * rb_idx];
            if (grid_unit.element_used) {
              if (grid_unit.format != pucch_format::FORMAT_4) {
                return true;
              }
              const auto occ_index = static_cast<unsigned>(res_f4.occ_index);
              srsran_assert(occ_index < static_cast<unsigned>(res_f4.occ_length), " OCC index exceeds OCC length");
              if (grid_unit.allocated_occ_cs_list[occ_index]) {
                return true;
              }
            }
          }
        }
      }
      // No intra-slot frequency hopping.
      else {
        for (unsigned rb_idx = res.starting_prb, rb_stop = res.starting_prb + 1; rb_idx != rb_stop; ++rb_idx) {
          for (unsigned sym_idx = res.starting_sym_idx, sym_stop = res.starting_sym_idx + res.nof_symbols;
               sym_idx != sym_stop;
               ++sym_idx) {
            const auto& grid_unit = grid[sym_idx + nof_symbols * rb_idx];
            if (grid_unit.element_used) {
              if (grid_unit.format != pucch_format::FORMAT_4) {
                return true;
              }
              const auto occ_index = static_cast<unsigned>(res_f4.occ_index);
              srsran_assert(occ_index < static_cast<unsigned>(res_f4.occ_length), " OCC index exceeds OCC length");
              if (grid_unit.allocated_occ_cs_list[occ_index]) {
                return true;
              }
            }
          }
        }
      }
    } else {
      return true;
    }
    return false;
  }

private:
  // Maximum 7 different OCCs, as per \c PUCCH-format1, in \c PUCCH-Config, TS 38.331.
  static const unsigned max_nof_occ = 7;
  // Maximum 12 different CSs, as per \c PUCCH-format1, in \c PUCCH-Config, TS 38.331.
  static const unsigned max_nof_cs     = 12;
  static const unsigned max_nof_occ_cs = max_nof_occ * max_nof_cs;
  struct grid_element {
    bool         element_used;
    pucch_format format;
    // Saves the used OCCs and CSs for a given resource element.
    std::array<bool, max_nof_occ_cs> allocated_occ_cs_list;
  };

  const unsigned            nof_rbs;
  const unsigned            nof_symbols;
  std::vector<grid_element> grid;
};

class test_pucch_res_generator : public ::testing::Test
{
public:
  test_pucch_res_generator() : grid(bwp_size, nof_symbols_per_slot) {}

protected:
  // Parameters that are passed by the routing to run the tests.
  const unsigned bwp_size{106};
  const unsigned nof_symbols_per_slot{14};
  pucch_grid     grid;
};

class test_pucch_res_generator_params : public ::testing::TestWithParam<pucch_gen_params>
{
public:
  test_pucch_res_generator_params() : grid(bwp_size, nof_symbols_per_slot) {}

protected:
  // Parameters that are passed by the routing to run the tests.
  const unsigned bwp_size{106};
  const unsigned nof_symbols_per_slot{14};
  pucch_grid     grid;
};

TEST_P(test_pucch_res_generator_params, test_pucch_res_given_number)
{
  const unsigned nof_res_f0 = GetParam().nof_res_f0;
  const unsigned nof_res_f1 = GetParam().nof_res_f1;
  const unsigned nof_res_f2 = GetParam().nof_res_f2;
  const unsigned nof_res_f3 = GetParam().nof_res_f3;
  const unsigned nof_res_f4 = GetParam().nof_res_f4;

  ASSERT_FALSE(nof_res_f0 != 0 and nof_res_f1 != 0) << "PUCCH Format 0 and Format 1 resources cannot be used together";
  ASSERT_FALSE((nof_res_f2 != 0) + (nof_res_f3 != 0) + (nof_res_f4 != 0) > 1)
      << "PUCCH Format 2/3/4 resources cannot be used together";

  pucch_f0_params params_f0{.nof_symbols            = GetParam().f0_nof_symbols,
                            .intraslot_freq_hopping = GetParam().f0_intraslot_freq_hopping};

  pucch_f1_params params_f1{.nof_cyc_shifts         = GetParam().nof_cyc_shifts,
                            .occ_supported          = GetParam().occ_supported,
                            .nof_symbols            = GetParam().f1_nof_symbols,
                            .intraslot_freq_hopping = GetParam().f1_intraslot_freq_hopping};

  pucch_f2_params params_f2{.nof_symbols            = GetParam().f2_nof_symbols,
                            .max_nof_rbs            = GetParam().max_nof_rbs,
                            .max_payload_bits       = GetParam().max_payload_bits,
                            .max_code_rate          = GetParam().max_code_rate,
                            .intraslot_freq_hopping = GetParam().f2_intraslot_freq_hopping};

  pucch_f3_params params_f3{.nof_symbols            = GetParam().f3_nof_symbols,
                            .max_nof_rbs            = GetParam().max_nof_rbs,
                            .max_payload_bits       = GetParam().max_payload_bits,
                            .max_code_rate          = GetParam().max_code_rate,
                            .intraslot_freq_hopping = GetParam().f3_intraslot_freq_hopping,
                            .additional_dmrs        = GetParam().f3_additional_dmrs,
                            .pi2_bpsk               = GetParam().f3_pi2_bpsk};

  pucch_f4_params params_f4{.nof_symbols            = GetParam().f4_nof_symbols,
                            .max_code_rate          = GetParam().max_code_rate,
                            .intraslot_freq_hopping = GetParam().f4_intraslot_freq_hopping,
                            .additional_dmrs        = GetParam().f4_additional_dmrs,
                            .pi2_bpsk               = GetParam().f4_pi2_bpsk};

  std::variant<pucch_f1_params, pucch_f0_params> f0_f1_params;
  unsigned                                       nof_res_f0_f1;
  pucch_format                                   set0_format;
  if (nof_res_f0 != 0) {
    set0_format = pucch_format::FORMAT_0;
    f0_f1_params.emplace<pucch_f0_params>(params_f0);
    nof_res_f0_f1 = nof_res_f0;
  } else {
    set0_format = pucch_format::FORMAT_1;
    f0_f1_params.emplace<pucch_f1_params>(params_f1);
    nof_res_f0_f1 = nof_res_f1;
  }

  std::variant<pucch_f2_params, pucch_f3_params, pucch_f4_params> f2_f3_f4_params;
  unsigned                                                        nof_res_f2_f3_f4;
  pucch_format                                                    set1_format;
  if (nof_res_f2 != 0) {
    set1_format = pucch_format::FORMAT_2;
    f2_f3_f4_params.emplace<pucch_f2_params>(params_f2);
    nof_res_f2_f3_f4 = nof_res_f2;
  } else if (nof_res_f3 != 0) {
    set1_format = pucch_format::FORMAT_3;
    f2_f3_f4_params.emplace<pucch_f3_params>(params_f3);
    nof_res_f2_f3_f4 = nof_res_f3;
  } else {
    set1_format = pucch_format::FORMAT_4;
    f2_f3_f4_params.emplace<pucch_f4_params>(params_f4);
    nof_res_f2_f3_f4 = nof_res_f4;
  }

  std::vector<pucch_resource> res_list = config_helpers::generate_cell_pucch_res_list(
      nof_res_f0_f1, nof_res_f2_f3_f4, f0_f1_params, f2_f3_f4_params, bwp_size, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP);

  ASSERT_FALSE(res_list.empty());
  ASSERT_EQ(nof_res_f0_f1 + nof_res_f2_f3_f4, res_list.size());

  // Check that the formats of each resource are the expected ones.
  for (unsigned i = 0; i != nof_res_f0_f1; ++i) {
    ASSERT_EQ(set0_format, res_list[i].format);
  }
  for (unsigned i = nof_res_f0_f1, i_end = res_list.size(); i != i_end; ++i) {
    ASSERT_EQ(set1_format, res_list[i].format);
  }

  for (const auto& pucch_res : res_list) {
    ASSERT_FALSE(grid.verify_collision(pucch_res));
    grid.add_resource(pucch_res);
  }
}

INSTANTIATE_TEST_SUITE_P(test_res_generation_given_number,
                         test_pucch_res_generator_params,
                         ::testing::Values(
                             // F1 and F2 test cases.
                             pucch_gen_params{.nof_res_f1                = 15,
                                              .nof_res_f2                = 10,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::twelve,
                                              .occ_supported             = false,
                                              .f1_nof_symbols            = 7,
                                              .f1_intraslot_freq_hopping = true,
                                              .f2_nof_symbols            = 2,
                                              .max_nof_rbs               = 2,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_25,
                                              .f2_intraslot_freq_hopping = false},
                             pucch_gen_params{.nof_res_f1                = 39,
                                              .nof_res_f2                = 19,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::twelve,
                                              .occ_supported             = false,
                                              .f1_nof_symbols            = 7,
                                              .f1_intraslot_freq_hopping = true,
                                              .f2_nof_symbols            = 2,
                                              .max_nof_rbs               = 2,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                              .f2_intraslot_freq_hopping = true},
                             pucch_gen_params{.nof_res_f1                = 39,
                                              .nof_res_f2                = 19,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::no_cyclic_shift,
                                              .occ_supported             = true,
                                              .f1_nof_symbols            = 7,
                                              .f1_intraslot_freq_hopping = true,
                                              .f2_nof_symbols            = 2,
                                              .max_nof_rbs               = 2,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_25,
                                              .f2_intraslot_freq_hopping = true},
                             pucch_gen_params{.nof_res_f1                = 39,
                                              .nof_res_f2                = 19,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::no_cyclic_shift,
                                              .occ_supported             = true,
                                              .f1_nof_symbols            = 11,
                                              .f1_intraslot_freq_hopping = true,
                                              .f2_nof_symbols            = 2,
                                              .max_nof_rbs               = 1,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                              .f2_intraslot_freq_hopping = true},
                             pucch_gen_params{.nof_res_f1                = 137,
                                              .nof_res_f2                = 25,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::twelve,
                                              .occ_supported             = true,
                                              .f1_nof_symbols            = 14,
                                              .f1_intraslot_freq_hopping = true,
                                              .f2_nof_symbols            = 2,
                                              .max_nof_rbs               = 1,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                              .f2_intraslot_freq_hopping = false},
                             pucch_gen_params{.nof_res_f1                = 36,
                                              .nof_res_f2                = 27,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::three,
                                              .occ_supported             = true,
                                              .f1_nof_symbols            = 9,
                                              .f1_intraslot_freq_hopping = false,
                                              .f2_nof_symbols            = 2,
                                              .max_nof_rbs               = 7,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                              .f2_intraslot_freq_hopping = true},
                             pucch_gen_params{.nof_res_f1                = 34,
                                              .nof_res_f2                = 10,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::two,
                                              .occ_supported             = false,
                                              .f1_nof_symbols            = 9,
                                              .f1_intraslot_freq_hopping = false,
                                              .f2_nof_symbols            = 1,
                                              .max_nof_rbs               = 7,
                                              .max_payload_bits          = 11,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_08,
                                              .f2_intraslot_freq_hopping = false},
                             pucch_gen_params{.nof_res_f1                = 15,
                                              .nof_res_f2                = 10,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::no_cyclic_shift,
                                              .occ_supported             = false,
                                              .f1_nof_symbols            = 9,
                                              .f1_intraslot_freq_hopping = false,
                                              .f2_nof_symbols            = 2,
                                              .max_nof_rbs               = 7,
                                              .max_payload_bits          = 11,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_08,
                                              .f2_intraslot_freq_hopping = true},
                             pucch_gen_params{.nof_res_f1                = 34,
                                              .nof_res_f2                = 10,
                                              .f0_nof_symbols            = 1,
                                              .f0_intraslot_freq_hopping = false,
                                              .f2_nof_symbols            = 1,
                                              .max_nof_rbs               = 7,
                                              .max_payload_bits          = 11,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_08,
                                              .f2_intraslot_freq_hopping = false},
                             // F0 and F2 test cases.
                             pucch_gen_params{.nof_res_f0                = 15,
                                              .nof_res_f2                = 10,
                                              .f0_nof_symbols            = 2,
                                              .f0_intraslot_freq_hopping = true,
                                              .f2_nof_symbols            = 2,
                                              .max_nof_rbs               = 2,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_25,
                                              .f2_intraslot_freq_hopping = false},
                             pucch_gen_params{.nof_res_f0                = 39,
                                              .nof_res_f2                = 19,
                                              .f0_nof_symbols            = 2,
                                              .f0_intraslot_freq_hopping = true,
                                              .f2_nof_symbols            = 2,
                                              .max_nof_rbs               = 2,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                              .f2_intraslot_freq_hopping = true},
                             pucch_gen_params{.nof_res_f0                = 39,
                                              .nof_res_f2                = 19,
                                              .f0_nof_symbols            = 1,
                                              .f0_intraslot_freq_hopping = false,
                                              .f2_nof_symbols            = 2,
                                              .max_nof_rbs               = 1,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                              .f2_intraslot_freq_hopping = true},
                             pucch_gen_params{.nof_res_f0                = 9,
                                              .nof_res_f2                = 19,
                                              .f0_nof_symbols            = 2,
                                              .f0_intraslot_freq_hopping = true,
                                              .f2_nof_symbols            = 2,
                                              .max_nof_rbs               = 1,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                              .f2_intraslot_freq_hopping = true},
                             pucch_gen_params{.nof_res_f0                = 39,
                                              .nof_res_f2                = 25,
                                              .f0_nof_symbols            = 2,
                                              .f0_intraslot_freq_hopping = true,
                                              .f2_nof_symbols            = 2,
                                              .max_nof_rbs               = 1,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                              .f2_intraslot_freq_hopping = false},
                             pucch_gen_params{.nof_res_f0                = 36,
                                              .nof_res_f2                = 27,
                                              .f0_nof_symbols            = 1,
                                              .f0_intraslot_freq_hopping = false,
                                              .f2_nof_symbols            = 2,
                                              .max_nof_rbs               = 7,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                              .f2_intraslot_freq_hopping = true},
                             pucch_gen_params{.nof_res_f0                = 15,
                                              .nof_res_f2                = 10,
                                              .f0_nof_symbols            = 1,
                                              .f0_intraslot_freq_hopping = false,
                                              .f2_nof_symbols            = 2,
                                              .max_nof_rbs               = 7,
                                              .max_payload_bits          = 11,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_08,
                                              .f2_intraslot_freq_hopping = true},
                             // F1 and F3 test cases.
                             pucch_gen_params{.nof_res_f1                = 15,
                                              .nof_res_f3                = 10,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::twelve,
                                              .occ_supported             = false,
                                              .f1_nof_symbols            = 7,
                                              .f1_intraslot_freq_hopping = true,
                                              .max_nof_rbs               = 2,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_25,
                                              .f3_nof_symbols            = 4,
                                              .f3_intraslot_freq_hopping = false},
                             pucch_gen_params{.nof_res_f1                = 39,
                                              .nof_res_f3                = 19,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::twelve,
                                              .occ_supported             = false,
                                              .f1_nof_symbols            = 7,
                                              .f1_intraslot_freq_hopping = true,
                                              .max_nof_rbs               = 2,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                              .f3_nof_symbols            = 4,
                                              .f3_intraslot_freq_hopping = true},
                             pucch_gen_params{.nof_res_f1                = 39,
                                              .nof_res_f3                = 19,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::no_cyclic_shift,
                                              .occ_supported             = true,
                                              .f1_nof_symbols            = 7,
                                              .f1_intraslot_freq_hopping = true,
                                              .max_nof_rbs               = 2,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_25,
                                              .f3_nof_symbols            = 4,
                                              .f3_intraslot_freq_hopping = true},
                             pucch_gen_params{.nof_res_f1                = 39,
                                              .nof_res_f3                = 19,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::no_cyclic_shift,
                                              .occ_supported             = true,
                                              .f1_nof_symbols            = 11,
                                              .f1_intraslot_freq_hopping = true,
                                              .max_nof_rbs               = 1,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                              .f3_nof_symbols            = 4,
                                              .f3_intraslot_freq_hopping = true},
                             pucch_gen_params{.nof_res_f1                = 137,
                                              .nof_res_f3                = 25,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::twelve,
                                              .occ_supported             = true,
                                              .f1_nof_symbols            = 14,
                                              .f1_intraslot_freq_hopping = true,
                                              .max_nof_rbs               = 1,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                              .f3_nof_symbols            = 4,
                                              .f3_intraslot_freq_hopping = false},
                             pucch_gen_params{.nof_res_f1                = 36,
                                              .nof_res_f3                = 27,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::three,
                                              .occ_supported             = true,
                                              .f1_nof_symbols            = 9,
                                              .f1_intraslot_freq_hopping = false,
                                              .max_nof_rbs               = 2,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                              .f3_nof_symbols            = 4,
                                              .f3_intraslot_freq_hopping = true},
                             pucch_gen_params{.nof_res_f1                = 34,
                                              .nof_res_f3                = 10,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::two,
                                              .occ_supported             = false,
                                              .f1_nof_symbols            = 9,
                                              .f1_intraslot_freq_hopping = false,
                                              .max_nof_rbs               = 7,
                                              .max_payload_bits          = 11,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_08,
                                              .f3_nof_symbols            = 4,
                                              .f3_intraslot_freq_hopping = false},
                             pucch_gen_params{.nof_res_f1                = 15,
                                              .nof_res_f3                = 10,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::no_cyclic_shift,
                                              .occ_supported             = false,
                                              .f1_nof_symbols            = 9,
                                              .f1_intraslot_freq_hopping = false,
                                              .max_nof_rbs               = 7,
                                              .max_payload_bits          = 11,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_08,
                                              .f3_nof_symbols            = 4,
                                              .f3_intraslot_freq_hopping = true},
                             // F1 and F4 test cases.
                             pucch_gen_params{.nof_res_f1                = 15,
                                              .nof_res_f4                = 10,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::twelve,
                                              .occ_supported             = false,
                                              .f1_nof_symbols            = 7,
                                              .f1_intraslot_freq_hopping = true,
                                              .max_nof_rbs               = 2,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_25,
                                              .f4_nof_symbols            = 4,
                                              .f4_intraslot_freq_hopping = false},
                             pucch_gen_params{.nof_res_f1                = 39,
                                              .nof_res_f4                = 19,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::twelve,
                                              .occ_supported             = false,
                                              .f1_nof_symbols            = 7,
                                              .f1_intraslot_freq_hopping = true,
                                              .max_nof_rbs               = 2,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                              .f4_nof_symbols            = 4,
                                              .f4_intraslot_freq_hopping = true},
                             pucch_gen_params{.nof_res_f1                = 39,
                                              .nof_res_f4                = 19,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::no_cyclic_shift,
                                              .occ_supported             = true,
                                              .f1_nof_symbols            = 7,
                                              .f1_intraslot_freq_hopping = true,
                                              .max_nof_rbs               = 2,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_25,
                                              .f4_nof_symbols            = 4,
                                              .f4_intraslot_freq_hopping = true},
                             pucch_gen_params{.nof_res_f1                = 39,
                                              .nof_res_f4                = 19,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::no_cyclic_shift,
                                              .occ_supported             = true,
                                              .f1_nof_symbols            = 11,
                                              .f1_intraslot_freq_hopping = true,
                                              .max_nof_rbs               = 1,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                              .f4_nof_symbols            = 4,
                                              .f4_intraslot_freq_hopping = true},
                             pucch_gen_params{.nof_res_f1                = 137,
                                              .nof_res_f4                = 25,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::twelve,
                                              .occ_supported             = true,
                                              .f1_nof_symbols            = 14,
                                              .f1_intraslot_freq_hopping = true,
                                              .max_nof_rbs               = 1,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                              .f4_nof_symbols            = 4,
                                              .f4_intraslot_freq_hopping = false},
                             pucch_gen_params{.nof_res_f1                = 36,
                                              .nof_res_f4                = 27,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::three,
                                              .occ_supported             = true,
                                              .f1_nof_symbols            = 9,
                                              .f1_intraslot_freq_hopping = false,
                                              .max_nof_rbs               = 2,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                              .f4_nof_symbols            = 4,
                                              .f4_intraslot_freq_hopping = true},
                             pucch_gen_params{.nof_res_f1                = 34,
                                              .nof_res_f4                = 10,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::two,
                                              .occ_supported             = false,
                                              .f1_nof_symbols            = 9,
                                              .f1_intraslot_freq_hopping = false,
                                              .max_nof_rbs               = 7,
                                              .max_payload_bits          = 11,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_08,
                                              .f4_nof_symbols            = 4,
                                              .f4_intraslot_freq_hopping = false},
                             pucch_gen_params{.nof_res_f1                = 15,
                                              .nof_res_f4                = 10,
                                              .nof_cyc_shifts            = pucch_nof_cyclic_shifts::no_cyclic_shift,
                                              .occ_supported             = false,
                                              .f1_nof_symbols            = 9,
                                              .f1_intraslot_freq_hopping = false,
                                              .max_nof_rbs               = 7,
                                              .max_payload_bits          = 11,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_08,
                                              .f4_nof_symbols            = 4,
                                              .f4_intraslot_freq_hopping = true},
                             pucch_gen_params{.nof_res_f1                = 34,
                                              .nof_res_f4                = 10,
                                              .f0_nof_symbols            = 1,
                                              .f0_intraslot_freq_hopping = false,
                                              .f2_nof_symbols            = 1,
                                              .max_nof_rbs               = 7,
                                              .max_payload_bits          = 11,
                                              .max_code_rate             = srsran::max_pucch_code_rate::dot_08,
                                              .f4_nof_symbols            = 4,
                                              .f4_intraslot_freq_hopping = false}));

///////////////////

class test_ue_pucch_config_builder : public ::testing::TestWithParam<pucch_cfg_builder_params>
{
protected:
  test_ue_pucch_config_builder() :
    nof_f0_res_harq_per_ue(GetParam().nof_res_f0_harq),
    nof_f1_res_harq_per_ue(GetParam().nof_res_f1_harq),
    nof_f2_res_harq_per_ue(GetParam().nof_res_f2_harq),
    nof_f3_res_harq_per_ue(GetParam().nof_res_f3_harq),
    nof_f4_res_harq_per_ue(GetParam().nof_res_f4_harq),
    nof_harq_cfg_per_ue(GetParam().nof_harq_cfg),
    nof_sr_res_per_cell(GetParam().nof_res_sr),
    nof_csi_res_per_cell(GetParam().nof_res_csi),
    f0_params(pucch_f0_params{.nof_symbols = 2, .intraslot_freq_hopping = false}),
    f1_params(pucch_f1_params{.nof_cyc_shifts         = pucch_nof_cyclic_shifts::no_cyclic_shift,
                              .occ_supported          = false,
                              .nof_symbols            = 14,
                              .intraslot_freq_hopping = true}),
    f2_params(pucch_f2_params{.nof_symbols            = 2,
                              .max_nof_rbs            = 2,
                              .max_code_rate          = max_pucch_code_rate::dot_25,
                              .intraslot_freq_hopping = false}),
    f3_params(pucch_f3_params{.nof_symbols            = 4,
                              .max_nof_rbs            = 2,
                              .max_code_rate          = max_pucch_code_rate::dot_25,
                              .intraslot_freq_hopping = false,
                              .additional_dmrs        = false,
                              .pi2_bpsk               = false}),
    f4_params(pucch_f4_params{.nof_symbols            = 4,
                              .max_code_rate          = max_pucch_code_rate::dot_25,
                              .intraslot_freq_hopping = false,
                              .additional_dmrs        = false,
                              .pi2_bpsk               = false,
                              .occ_length             = pucch_f4_occ_len::n2}),
    serv_cell_cfg(sched_config_helper::create_default_sched_ue_creation_request().cfg.cells->front().serv_cell_cfg)
  {
    if (GetParam().nof_res_f0_harq != 0) {
      auto& pucch_res_list = serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg->pucch_res_list;
      for (auto& res : pucch_res_list) {
        if (res.format == pucch_format::FORMAT_1) {
          res.format = pucch_format::FORMAT_0;
          res.format_params.emplace<pucch_format_0_cfg>();
        }
      }
    }

    if (GetParam().nof_res_f3_harq != 0) {
      auto& pucch_res_list = serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg->pucch_res_list;
      for (auto& res : pucch_res_list) {
        if (res.format == pucch_format::FORMAT_2) {
          res.format = pucch_format::FORMAT_3;
        }
      }
    } else if (GetParam().nof_res_f4_harq != 0) {
      auto& pucch_res_list = serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg->pucch_res_list;
      for (auto& res : pucch_res_list) {
        if (res.format == pucch_format::FORMAT_2) {
          res.format = pucch_format::FORMAT_4;
        }
      }
    }

    if (GetParam().nof_res_csi == 0) {
      serv_cell_cfg.csi_meas_cfg.reset();
    }
  }

  bool verify_nof_res_and_idx(unsigned harq_cfg_idx, unsigned sr_idx, unsigned csi_idx) const
  {
    srsran_assert(harq_cfg_idx < nof_harq_cfg_per_ue,
                  "HARQ resources config index must be smaller than the number of HARQ resources configurations");
    srsran_assert(sr_idx < nof_sr_res_per_cell,
                  "SR resource config index must be smaller than the number of cell SR resources");
    const bool has_csi = serv_cell_cfg.csi_meas_cfg.has_value();
    if (has_csi) {
      srsran_assert(csi_idx < nof_csi_res_per_cell,
                    "CSI resource config index must be smaller than the number of cell CSI resources");
    }

    const pucch_config& pucch_cfg          = serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.value();
    const unsigned      nof_sr_res_per_ue  = 1;
    const unsigned      nof_csi_res_per_ue = has_csi ? 1U : 0U;

    const bool     has_f0_res                   = nof_f0_res_harq_per_ue > 0;
    const unsigned nof_f0_f1_res_harq_per_ue    = has_f0_res ? nof_f0_res_harq_per_ue : nof_f1_res_harq_per_ue;
    const bool     has_f2_res                   = nof_f2_res_harq_per_ue > 0;
    unsigned       nof_f2_f3_f4_res_harq_per_ue = nof_f2_res_harq_per_ue;
    if (nof_f3_res_harq_per_ue > 0) {
      nof_f2_f3_f4_res_harq_per_ue = nof_f3_res_harq_per_ue;
    } else if (nof_f4_res_harq_per_ue > 0) {
      nof_f2_f3_f4_res_harq_per_ue = nof_f4_res_harq_per_ue;
    }

    bool test_result = true;
    // Check the number of resources in the PUCCH resource list is correct.
    if (has_f0_res and has_f2_res) {
      // For Format 0 and Format 2, the pucch_res_list contains 2 extra resources, 1 per PUCCH resource set.
      const unsigned extra_resources = has_csi ? 2U : 1U;
      test_result = pucch_cfg.pucch_res_list.size() == nof_f0_f1_res_harq_per_ue + nof_f2_f3_f4_res_harq_per_ue +
                                                           nof_sr_res_per_ue + nof_csi_res_per_ue + extra_resources;
    } else {
      test_result = pucch_cfg.pucch_res_list.size() ==
                    nof_f0_f1_res_harq_per_ue + nof_f2_f3_f4_res_harq_per_ue + nof_sr_res_per_ue + nof_csi_res_per_ue;
    }

    // Check the number of resources in the PUCCH resource sets is correct.
    if (has_f0_res and has_f2_res) {
      // For Format 0 and Format 2, each PUCCH resource set contains extra resources. If CSI is enabled, then each PUCCH
      // resource set contains 1 (new) PUCCH resource that is added to the pucch_res_list plus 1 that maps to the
      // SR (in set 0) or CSI (in set 1). If CSI is not enabled, 1 PUCCH then each PUCCH resource set contains
      // 1 PUCCH resource only.
      const unsigned extra_res_per_set = has_csi ? 2U : 1U;
      test_result                      = test_result && pucch_cfg.pucch_res_set[0].pucch_res_id_list.size() ==
                                       nof_f0_f1_res_harq_per_ue + extra_res_per_set;
      test_result = test_result && pucch_cfg.pucch_res_set[1].pucch_res_id_list.size() ==
                                       nof_f2_f3_f4_res_harq_per_ue + extra_res_per_set;
    } else {
      test_result = test_result && pucch_cfg.pucch_res_set[0].pucch_res_id_list.size() == nof_f0_f1_res_harq_per_ue;
      test_result = test_result && pucch_cfg.pucch_res_set[1].pucch_res_id_list.size() == nof_f2_f3_f4_res_harq_per_ue;
    }

    // Helper to retrieve a given PUCCH resource given its ID from the PUCCH resource list.
    auto get_pucch_resource_with_id = [&pucch_cfg](pucch_res_id_t res_id) {
      return std::find_if(pucch_cfg.pucch_res_list.begin(),
                          pucch_cfg.pucch_res_list.end(),
                          [res_id](const pucch_resource& res) { return res.res_id == res_id; });
    };

    // Check SR and related PUCCH resource.
    test_result                          = test_result and pucch_cfg.sr_res_list.size() == 1;
    const pucch_res_id_t sr_pucch_res_id = pucch_cfg.sr_res_list.front().pucch_res_id;
    const bool           sr_cell_res_id_test =
        sr_pucch_res_id.cell_res_id == nof_f0_f1_res_harq_per_ue * nof_harq_cfg_per_ue + sr_idx;
    if (has_f0_res and has_f2_res) {
      const unsigned extra_res         = has_csi ? 1U : 0U;
      const bool     sr_ue_res_id_test = sr_pucch_res_id.ue_res_id == nof_f0_f1_res_harq_per_ue + extra_res;
      test_result                      = test_result and sr_cell_res_id_test and sr_ue_res_id_test;
    } else {
      const bool sr_ue_res_id_test = sr_pucch_res_id.ue_res_id == nof_f0_f1_res_harq_per_ue;
      test_result                  = test_result and sr_cell_res_id_test and sr_ue_res_id_test;
    }
    // Check the PUCCH resouce for SR present in the list.
    const auto* res_it           = get_pucch_resource_with_id(pucch_cfg.sr_res_list.front().pucch_res_id);
    test_result                  = test_result and res_it != pucch_cfg.pucch_res_list.end();
    const auto& sr_pucch_res_cfg = *res_it;

    // Check CSI and related PUCCH resource.
    if (has_csi) {
      const auto& csi_cfg = serv_cell_cfg.csi_meas_cfg.value();
      srsran_assert(not csi_cfg.csi_report_cfg_list.empty() and
                        std::holds_alternative<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                            csi_cfg.csi_report_cfg_list.front().report_cfg_type) and
                        not std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                                csi_cfg.csi_report_cfg_list.front().report_cfg_type)
                                .pucch_csi_res_list.empty(),
                    "PUCCH-CSI-ResourceList has not been configured in the CSI-reportConfig");

      const pucch_res_id_t csi_res_id = std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                                            csi_cfg.csi_report_cfg_list.front().report_cfg_type)
                                            .pucch_csi_res_list.front()
                                            .pucch_res_id;

      // Check the PUCCH resouce ID for CSI is correct.
      if (has_f0_res and has_f2_res) {
        const bool csi_cell_res_id_test =
            csi_res_id.cell_res_id == nof_f0_f1_res_harq_per_ue * nof_harq_cfg_per_ue + nof_sr_res_per_cell +
                                          nof_f2_f3_f4_res_harq_per_ue * nof_harq_cfg_per_ue + csi_idx;
        const bool csi_ue_res_id_test =
            csi_res_id.ue_res_id == nof_f0_f1_res_harq_per_ue + 2U + nof_f2_f3_f4_res_harq_per_ue;
        test_result = test_result and csi_cell_res_id_test and csi_ue_res_id_test;
      } else {
        const bool csi_cell_res_id_test =
            csi_res_id.cell_res_id == nof_f0_f1_res_harq_per_ue * nof_harq_cfg_per_ue + nof_sr_res_per_cell +
                                          nof_f2_f3_f4_res_harq_per_ue * nof_harq_cfg_per_ue + csi_idx;
        const bool csi_ue_res_id_test =
            csi_res_id.ue_res_id == nof_f0_f1_res_harq_per_ue + 1 + nof_f2_f3_f4_res_harq_per_ue;
        test_result = test_result and csi_cell_res_id_test and csi_ue_res_id_test;
      }
      // Check the PUCCH resouce for SR present in the list.
      res_it      = get_pucch_resource_with_id(csi_res_id);
      test_result = test_result and res_it != pucch_cfg.pucch_res_list.end();
    }

    // Check the PUCCH resources indexing in PUCCH resource Set 0.
    for (pucch_res_id_t res_idx : pucch_cfg.pucch_res_set[0].pucch_res_id_list) {
      // Make sure the PUCCH resource ID in the set has a corresponding resource in the PUCCH resource list.
      res_it = get_pucch_resource_with_id(res_idx);

      if (has_f0_res and has_f2_res) {
        test_result = test_result and res_it != pucch_cfg.pucch_res_list.end();
        // The checks on the PUCCH resource indices are only valid for configurations that doesn't have Format 0.

        const bool pucch_cell_res_id_test = (res_idx.cell_res_id >= nof_f0_f1_res_harq_per_ue * harq_cfg_idx and
                                             res_idx.cell_res_id < nof_f0_f1_res_harq_per_ue * (harq_cfg_idx + 1)) or
                                            res_idx.cell_res_id == sr_pucch_res_id.cell_res_id or
                                            res_idx.cell_res_id == std::numeric_limits<unsigned>::max();
        const bool pucch_ue_res_id_test =
            res_idx.ue_res_id < nof_f0_f1_res_harq_per_ue + 1U or res_idx.ue_res_id == sr_pucch_res_id.ue_res_id;
        test_result = test_result and pucch_cell_res_id_test and pucch_ue_res_id_test and
                      res_it != pucch_cfg.pucch_res_list.end();
      } else {
        // The checks on the PUCCH resource indices are only valid for configurations that doesn't have Format 0.
        const bool pucch_cell_res_id_test = res_idx.cell_res_id >= nof_f0_f1_res_harq_per_ue * harq_cfg_idx and
                                            res_idx.cell_res_id < nof_f0_f1_res_harq_per_ue * (harq_cfg_idx + 1);
        // The PUCCH resource ID for the ASN1 message for PUCCH.
        // F1 resources is expected to be from 0 to nof_f0_f1_res_harq_per_ue for all UEs.
        const bool pucch_ue_res_id_test = res_idx.ue_res_id < nof_f0_f1_res_harq_per_ue;
        const bool pucch_ue_cell_res_id_test =
            res_idx.cell_res_id - nof_f0_f1_res_harq_per_ue * harq_cfg_idx == res_idx.ue_res_id;

        test_result = test_result and pucch_cell_res_id_test and pucch_ue_res_id_test and pucch_ue_cell_res_id_test and
                      res_it != pucch_cfg.pucch_res_list.end();
      }
    }
    // Check the PUCCH resources indexing in PUCCH resource Set 1.
    for (pucch_res_id_t res_idx : pucch_cfg.pucch_res_set[1].pucch_res_id_list) {
      // Make sure the PUCCH resource ID in the set has a corresponding resource in the PUCCH resource list.
      res_it = get_pucch_resource_with_id(res_idx);

      if (has_f0_res and has_f2_res) {
        // The checks on the PUCCH resource indices are only valid for configurations that doesn't have Format 0.
        bool pucch_cell_res_id_test = true;
        bool pucch_ue_res_id_test   = true;
        if (has_csi) {
          const pucch_res_id_t csi_res_id =
              std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                  serv_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list.front().report_cfg_type)
                  .pucch_csi_res_list.front()
                  .pucch_res_id;
          pucch_cell_res_id_test =
              (res_idx.cell_res_id >= nof_f0_f1_res_harq_per_ue * nof_harq_cfg_per_ue + nof_sr_res_per_cell +
                                          nof_f2_res_harq_per_ue * harq_cfg_idx and
               res_idx.cell_res_id < nof_f0_f1_res_harq_per_ue * nof_harq_cfg_per_ue + nof_sr_res_per_cell +
                                         nof_f2_res_harq_per_ue * (harq_cfg_idx + 1)) or
              res_idx.cell_res_id == csi_res_id.cell_res_id or
              res_idx.cell_res_id == std::numeric_limits<unsigned>::max();

          // The PUCCH resource ID for the ASN1 message for PUCCH
          // F2 resources is expected to be from (nof_f0_f1_res_harq_per_ue + 1) to (nof_f0_f1_res_harq_per_ue + 1 +
          // nof_f2_res_harq_per_ue) for all UEs.
          pucch_ue_res_id_test = (res_idx.ue_res_id >= nof_f0_f1_res_harq_per_ue + 2U and
                                  res_idx.ue_res_id < nof_f0_f1_res_harq_per_ue + 3U + nof_f2_res_harq_per_ue) or
                                 res_idx.ue_res_id == csi_res_id.ue_res_id;
        } else {
          pucch_cell_res_id_test =
              (res_idx.cell_res_id >= nof_f0_f1_res_harq_per_ue * nof_harq_cfg_per_ue + nof_sr_res_per_cell +
                                          nof_f2_res_harq_per_ue * harq_cfg_idx and
               res_idx.cell_res_id < nof_f0_f1_res_harq_per_ue * nof_harq_cfg_per_ue + nof_sr_res_per_cell +
                                         nof_f2_res_harq_per_ue * (harq_cfg_idx + 1)) or
              res_idx.cell_res_id == std::numeric_limits<unsigned>::max();

          // The PUCCH resource ID for the ASN1 message for PUCCH
          // F2 resources is expected to be from (nof_f0_f1_res_harq_per_ue + 1) to (nof_f0_f1_res_harq_per_ue + 1 +
          // nof_f2_res_harq_per_ue) for all UEs.
          pucch_ue_res_id_test = res_idx.ue_res_id >= nof_f0_f1_res_harq_per_ue + 1U and
                                 res_idx.ue_res_id < nof_f0_f1_res_harq_per_ue + 2U + nof_f2_res_harq_per_ue;
        }

        test_result = (has_f0_res and has_f2_res) ? res_it != pucch_cfg.pucch_res_list.end()
                                                  : test_result and pucch_cell_res_id_test and pucch_ue_res_id_test and
                                                        res_it != pucch_cfg.pucch_res_list.end();
      } else {
        // The checks on the PUCCH resource indices are only valid for configurations that doesn't have Format 0.
        const bool pucch_cell_res_id_test =
            res_idx.cell_res_id >= nof_f0_f1_res_harq_per_ue * nof_harq_cfg_per_ue + nof_sr_res_per_cell +
                                       nof_f2_f3_f4_res_harq_per_ue * harq_cfg_idx and
            res_idx.cell_res_id < nof_f0_f1_res_harq_per_ue * nof_harq_cfg_per_ue + nof_sr_res_per_cell +
                                      nof_f2_f3_f4_res_harq_per_ue * (harq_cfg_idx + 1);
        // The PUCCH resource ID for the ASN1 message for PUCCH
        // F2 resources is expected to be from (nof_f0_f1_res_harq_per_ue + 1) to (nof_f0_f1_res_harq_per_ue + 1 +
        // nof_f2_f3_f4_res_harq_per_ue) for all UEs.
        const bool pucch_ue_res_id_test =
            res_idx.ue_res_id >= nof_f0_f1_res_harq_per_ue + 1 and
            res_idx.ue_res_id < nof_f0_f1_res_harq_per_ue + 1 + nof_f2_f3_f4_res_harq_per_ue;
        // Check if the PUCCH cell resourece ID is set correspondingly to the PUCCH UE resource ID.
        const bool pucch_ue_cell_res_id_test =
            res_idx.cell_res_id - (nof_f0_f1_res_harq_per_ue * nof_harq_cfg_per_ue + nof_sr_res_per_cell +
                                   nof_f2_f3_f4_res_harq_per_ue * harq_cfg_idx) ==
            res_idx.ue_res_id - (nof_f0_f1_res_harq_per_ue + 1);

        test_result = test_result and pucch_cell_res_id_test and pucch_ue_res_id_test and pucch_ue_cell_res_id_test and
                      res_it != pucch_cfg.pucch_res_list.end();
      }
    }

    // Check the format correctness.
    const pucch_format format_set_0 = has_f0_res ? pucch_format::FORMAT_0 : pucch_format::FORMAT_1;
    pucch_format       format_set_1 = pucch_format::FORMAT_2;
    if (nof_f3_res_harq_per_ue > 0) {
      format_set_1 = pucch_format::FORMAT_3;
    } else if (nof_f4_res_harq_per_ue > 0) {
      format_set_1 = pucch_format::FORMAT_4;
    }

    for (const auto& res : pucch_cfg.pucch_res_list) {
      // Take into account the extra resource introduced in set 0 in the case of F0 and F2.
      const unsigned     extra_set_0_res = has_f0_res and has_f2_res and has_csi ? 2U : 1U;
      const pucch_format expected_format =
          res.res_id.ue_res_id < nof_f0_f1_res_harq_per_ue + extra_set_0_res ? format_set_0 : format_set_1;
      test_result = test_result && expected_format == res.format;
    }

    // Special case of F0 and F2.
    if (has_f0_res and has_f2_res) {
      srsran_assert(sr_pucch_res_cfg.format == pucch_format::FORMAT_0 and
                        std::holds_alternative<pucch_format_0_cfg>(sr_pucch_res_cfg.format_params),
                    "SR PUCCH resource must be of Format 0 if any of the PUCCH resources from set 0 has Format 0.");

      const auto harq_res_set_0_for_sr_idx =
          pucch_cfg.pucch_res_set[0].pucch_res_id_list[pucch_cfg.pucch_res_set[0].pucch_res_id_list.size() - 1];
      const auto harq_res_set_0_for_sr = pucch_cfg.pucch_res_list[harq_res_set_0_for_sr_idx.ue_res_id];
      test_result                      = test_result and harq_res_set_0_for_sr == sr_pucch_res_cfg;

      const auto harq_res_set_1_for_sr_idx = pucch_cfg.pucch_res_set[1].pucch_res_id_list.back();
      const auto harq_res_set_1_for_sr     = pucch_cfg.pucch_res_list[harq_res_set_1_for_sr_idx.ue_res_id];
      test_result = test_result and harq_res_set_1_for_sr.starting_prb == sr_pucch_res_cfg.starting_prb and
                    harq_res_set_1_for_sr.second_hop_prb == sr_pucch_res_cfg.second_hop_prb and
                    harq_res_set_1_for_sr.starting_sym_idx == sr_pucch_res_cfg.starting_sym_idx and
                    harq_res_set_1_for_sr.nof_symbols == sr_pucch_res_cfg.nof_symbols;

      if (has_csi) {
        const pucch_res_id_t csi_res_id =
            std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                serv_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list.front().report_cfg_type)
                .pucch_csi_res_list.front()
                .pucch_res_id;

        const auto* csi_it = get_pucch_resource_with_id(csi_res_id);
        srsran_assert(csi_it != pucch_cfg.pucch_res_list.end(),
                      "CSI PUCCH resource not found in the PUCCH resource list");
        const auto& csi_pucch_res_cfg = *csi_it;

        srsran_assert(csi_pucch_res_cfg.format == pucch_format::FORMAT_2 and
                          std::holds_alternative<pucch_format_2_3_cfg>(csi_pucch_res_cfg.format_params),
                      "CSI PUCCH resource must be of Format 2");

        const auto harq_res_set_1_for_csi_idx =
            pucch_cfg.pucch_res_set[1].pucch_res_id_list[pucch_cfg.pucch_res_set[1].pucch_res_id_list.size() - 2U];
        const auto harq_res_set_1_for_csi = pucch_cfg.pucch_res_list[harq_res_set_1_for_csi_idx.ue_res_id];
        test_result                       = test_result and harq_res_set_1_for_csi == csi_pucch_res_cfg;

        const auto harq_res_set_0_for_csi_idx =
            pucch_cfg.pucch_res_set[0].pucch_res_id_list[pucch_cfg.pucch_res_set[0].pucch_res_id_list.size() - 2U];
        const auto harq_res_set_0_for_csi = pucch_cfg.pucch_res_list[harq_res_set_0_for_csi_idx.ue_res_id];
        test_result = test_result and harq_res_set_0_for_csi.starting_prb == csi_pucch_res_cfg.starting_prb and
                      harq_res_set_0_for_csi.second_hop_prb == csi_pucch_res_cfg.second_hop_prb and
                      harq_res_set_0_for_csi.starting_sym_idx == csi_pucch_res_cfg.starting_sym_idx and
                      harq_res_set_0_for_csi.nof_symbols == csi_pucch_res_cfg.nof_symbols;
      }
    }

    return test_result;
  }

  // Parameters that are passed by the routing to run the tests.
  const unsigned        bwp_size{400};
  const unsigned        nof_f0_res_harq_per_ue;
  const unsigned        nof_f1_res_harq_per_ue;
  const unsigned        nof_f2_res_harq_per_ue;
  const unsigned        nof_f3_res_harq_per_ue;
  const unsigned        nof_f4_res_harq_per_ue;
  const unsigned        nof_harq_cfg_per_ue;
  const unsigned        nof_sr_res_per_cell;
  const unsigned        nof_csi_res_per_cell;
  const unsigned        nof_symbols_per_slot{14};
  const pucch_f0_params f0_params;
  const pucch_f1_params f1_params;
  const pucch_f2_params f2_params;
  const pucch_f3_params f3_params;
  const pucch_f4_params f4_params;
  serving_cell_config   serv_cell_cfg;
};

TEST_P(test_ue_pucch_config_builder, test_validator_too_many_resources)
{
  // Generate the cell list of resources with many resources.
  const unsigned              nof_f0_res = nof_f0_res_harq_per_ue * nof_harq_cfg_per_ue + nof_sr_res_per_cell;
  const unsigned              nof_f1_res = nof_f1_res_harq_per_ue * nof_harq_cfg_per_ue + nof_sr_res_per_cell;
  const unsigned              nof_f2_res = nof_f2_res_harq_per_ue * nof_harq_cfg_per_ue + nof_csi_res_per_cell;
  const unsigned              nof_f3_res = nof_f3_res_harq_per_ue * nof_harq_cfg_per_ue + nof_csi_res_per_cell;
  const unsigned              nof_f4_res = nof_f4_res_harq_per_ue * nof_harq_cfg_per_ue + nof_csi_res_per_cell;
  std::vector<pucch_resource> res_list;

  ASSERT_FALSE(nof_f0_res_harq_per_ue != 0 and nof_f1_res_harq_per_ue != 0)
      << "PUCCH Format 0 and Format 1 resources cannot be used together";

  ASSERT_FALSE(nof_f2_res_harq_per_ue != 0 and nof_f3_res_harq_per_ue != 0)
      << "PUCCH Format 2 and Format 3 resources cannot be used together";

  ASSERT_FALSE(nof_f0_res_harq_per_ue != 0 and nof_f3_res_harq_per_ue != 0)
      << "PUCCH Format 0 and Format 3 resources cannot be used together";

  ASSERT_FALSE(nof_f0_res_harq_per_ue != 0 and nof_f4_res_harq_per_ue != 0)
      << "PUCCH Format 0 and Format 4 resources cannot be used together";

  unsigned                                       nof_res_f0_f1;
  unsigned                                       nof_ue_pucch_f0_f1_res_harq;
  std::variant<pucch_f1_params, pucch_f0_params> f0_f1_params;
  if (nof_f0_res_harq_per_ue != 0) {
    nof_res_f0_f1               = nof_f0_res;
    nof_ue_pucch_f0_f1_res_harq = nof_f0_res_harq_per_ue;
    f0_f1_params.emplace<pucch_f0_params>(f0_params);
  } else {
    nof_res_f0_f1               = nof_f1_res;
    nof_ue_pucch_f0_f1_res_harq = nof_f1_res_harq_per_ue;
    f0_f1_params.emplace<pucch_f1_params>(f1_params);
  }

  unsigned                                                        nof_res_f2_f3_f4;
  unsigned                                                        nof_ue_pucch_f2_f3_f4_res_harq;
  std::variant<pucch_f2_params, pucch_f3_params, pucch_f4_params> f2_f3_f4_params;
  if (nof_f2_res_harq_per_ue != 0) {
    nof_res_f2_f3_f4               = nof_f2_res;
    nof_ue_pucch_f2_f3_f4_res_harq = nof_f2_res_harq_per_ue;
    f2_f3_f4_params.emplace<pucch_f2_params>(f2_params);
  } else if (nof_f3_res_harq_per_ue != 0) {
    nof_res_f2_f3_f4               = nof_f3_res;
    nof_ue_pucch_f2_f3_f4_res_harq = nof_f3_res_harq_per_ue;
    f2_f3_f4_params.emplace<pucch_f3_params>(f3_params);
  } else {
    nof_res_f2_f3_f4               = nof_f4_res;
    nof_ue_pucch_f2_f3_f4_res_harq = nof_f4_res_harq_per_ue;
    f2_f3_f4_params.emplace<pucch_f4_params>(f4_params);
  }

  res_list = config_helpers::generate_cell_pucch_res_list(
      nof_res_f0_f1, nof_res_f2_f3_f4, f0_f1_params, f2_f3_f4_params, bwp_size, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP);

  const auto harq_idx_cfg = test_rgen::uniform_int<unsigned>(0, nof_harq_cfg_per_ue - 1);
  const auto sr_idx_cfg   = test_rgen::uniform_int<unsigned>(0, nof_sr_res_per_cell - 1);
  const auto csi_idx_cfg  = test_rgen::uniform_int<unsigned>(0, nof_csi_res_per_cell - 1);

  // Update pucch_cfg with the UE list of resources (with at max 8 HARQ F1, 8 HARQ F2, 4 SR).
  config_helpers::ue_pucch_config_builder(serv_cell_cfg,
                                          res_list,
                                          harq_idx_cfg,
                                          sr_idx_cfg,
                                          csi_idx_cfg,
                                          nof_ue_pucch_f0_f1_res_harq,
                                          nof_ue_pucch_f2_f3_f4_res_harq,
                                          nof_harq_cfg_per_ue,
                                          nof_sr_res_per_cell,
                                          nof_csi_res_per_cell);

  ASSERT_TRUE(verify_nof_res_and_idx(harq_idx_cfg, sr_idx_cfg, csi_idx_cfg));
}

INSTANTIATE_TEST_SUITE_P(ue_pucch_config_builder,
                         test_ue_pucch_config_builder,
                         // clang-format off
                        //                                   nof:  f0  |  f1  |  f2  |  f3  |  f4  | harq |  sr  | csi
                        //                                   nof:  f0  |  f1  |  f2  |  f3  |  f4  | cfg  |  sr  | csi
                        ::testing::Values(
                                          // Format 0 and Format 2 test cases.
                                          pucch_cfg_builder_params{ 6, 0, 6, 0, 0, 1, 8, 8 },
                                          pucch_cfg_builder_params{ 5, 0, 3, 0, 0, 1, 1, 1 },
                                          pucch_cfg_builder_params{ 6, 0, 6, 0, 0, 1, 4, 1 },
                                          pucch_cfg_builder_params{ 2, 0, 2, 0, 0, 1, 1, 1 },
                                          pucch_cfg_builder_params{ 3, 0, 5, 0, 0, 1, 3, 1 },
                                          pucch_cfg_builder_params{ 3, 0, 5, 0, 0, 1, 1, 3 },
                                          pucch_cfg_builder_params{ 6, 0, 6, 0, 0, 4, 4, 4 },
                                          pucch_cfg_builder_params{ 5, 0, 2, 0, 0, 10, 2, 7 },
                                          pucch_cfg_builder_params{ 2, 0, 5, 0, 0, 10, 2, 7 },
                                          pucch_cfg_builder_params{ 2, 0, 5, 0, 0, 10, 7, 2 },
                                          pucch_cfg_builder_params{ 4, 0, 4, 0, 0, 10, 21, 14 },
                                          pucch_cfg_builder_params{ 2, 0, 6, 0, 0, 3, 7, 3 },
                                          pucch_cfg_builder_params{ 6, 0, 4, 0, 0, 5, 6, 2 },
                                          pucch_cfg_builder_params{ 6, 0, 6, 0, 0, 3, 6, 0 },
                                          // Format 1 and Format 2 test cases.
                                          pucch_cfg_builder_params{ 0, 3, 6, 0, 0, 1, 2, 1 },
                                          pucch_cfg_builder_params{ 0, 7, 3, 0, 0, 1, 1, 1 },
                                          pucch_cfg_builder_params{ 0, 8, 8, 0, 0, 1, 4, 1 },
                                          pucch_cfg_builder_params{ 0, 1, 1, 0, 0, 1, 1, 1 },
                                          pucch_cfg_builder_params{ 0, 7, 7, 0, 0, 1, 3, 1 },
                                          pucch_cfg_builder_params{ 0, 8, 8, 0, 0, 4, 4, 4 },
                                          pucch_cfg_builder_params{ 0, 5, 2, 0, 0, 8, 2, 7 },
                                          pucch_cfg_builder_params{ 0, 2, 7, 0, 0, 3, 7, 3 },
                                          pucch_cfg_builder_params{ 0, 6, 4, 0, 0, 5, 6, 2 },
                                          // Format 1 and Format 3 test cases.
                                          pucch_cfg_builder_params{ 0, 3, 0, 6, 0, 1, 2, 1 },
                                          pucch_cfg_builder_params{ 0, 7, 0, 3, 0, 1, 1, 1 },
                                          pucch_cfg_builder_params{ 0, 8, 0, 8, 0, 1, 4, 1 },
                                          pucch_cfg_builder_params{ 0, 1, 0, 1, 0, 1, 1, 1 },
                                          pucch_cfg_builder_params{ 0, 7, 0, 7, 0, 1, 3, 1 },
                                          pucch_cfg_builder_params{ 0, 8, 0, 8, 0, 4, 4, 4 },
                                          pucch_cfg_builder_params{ 0, 5, 0, 2, 0, 8, 2, 7 },
                                          pucch_cfg_builder_params{ 0, 2, 0, 7, 0, 3, 7, 3 },
                                          pucch_cfg_builder_params{ 0, 6, 0, 4, 0, 5, 6, 2 },
                                          // Format 1 and Format 4 test cases.
                                          pucch_cfg_builder_params{ 0, 3, 0, 0, 6, 1, 2, 1 },
                                          pucch_cfg_builder_params{ 0, 7, 0, 0, 3, 1, 1, 1 },
                                          pucch_cfg_builder_params{ 0, 8, 0, 0, 8, 1, 4, 1 },
                                          pucch_cfg_builder_params{ 0, 1, 0, 0, 1, 1, 1, 1 },
                                          pucch_cfg_builder_params{ 0, 7, 0, 0, 7, 1, 3, 1 },
                                          pucch_cfg_builder_params{ 0, 8, 0, 0, 8, 4, 4, 4 },
                                          pucch_cfg_builder_params{ 0, 5, 0, 0, 2, 8, 2, 7 },
                                          pucch_cfg_builder_params{ 0, 2, 0, 0, 7, 3, 7, 3 },
                                          pucch_cfg_builder_params{ 0, 6, 0, 0, 4, 5, 6, 2 }
)
                         // clang-format on
);
