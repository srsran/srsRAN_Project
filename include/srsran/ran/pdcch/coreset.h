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

#pragma once

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/pdcch/pdcch_constants.h"
#include "srsran/ran/resource_allocation/rb_interval.h"

namespace srsran {

/// Max number of CORESETs per BWP per cell (including UE-specific and common CORESETs).
/// \remark See TS 38.331, "PDCCH-Config".
constexpr size_t MAX_NOF_CORESETS_PER_BWP = 3;

/// Describes a frequency resource bitmap for CORESET allocation. Each resource represents a group of 6 PRBs.
/// \remark See TS 38.331, "ControlResourceSet"
using freq_resource_bitmap = bounded_bitset<pdcch_constants::MAX_NOF_FREQ_RESOURCES, true>;

/// \brief CORESET identifier. This value is UE-specific, which means that a UE can have up to
/// "maxNrofControlResourceSets" CORESETS configured.
/// \remark See TS 38.331, "maxNrofControlResourceSets".
enum coreset_id : uint8_t { MAX_CORESET_ID = 11, MAX_NOF_CORESETS = 12 };

constexpr inline coreset_id to_coreset_id(unsigned cs_id)
{
  return static_cast<coreset_id>(cs_id);
}

/// CORESET configuration.
/// \remark See TS 38.331, "ControlResourceSet".
struct coreset_configuration {
  struct interleaved_mapping_type {
    /// Values: (2, 3, 6).
    unsigned reg_bundle_sz;
    /// Values: (2, 3, 6).
    unsigned interleaver_sz;
    /// Values: (0..MAX_NOF_PRBS-1).
    unsigned shift_index;

    bool operator==(const interleaved_mapping_type& rhs) const
    {
      return reg_bundle_sz == rhs.reg_bundle_sz and interleaver_sz == rhs.interleaver_sz and
             shift_index == rhs.shift_index;
    }
  };
  enum class precoder_granularity_type { same_as_reg_bundle, all_contiguous_rbs };

  coreset_id id;
  /// Duration in number of symbols. Values: (1..maxCORESETDuration).
  unsigned                           duration;
  optional<interleaved_mapping_type> interleaved;
  precoder_granularity_type          precoder_granurality;
  /// PDCCH DMRS scrambling initialization. Values: (0..65535).
  optional<unsigned> pdcch_dmrs_scrambling_id;

  void set_coreset0_crbs(crb_interval crbs)
  {
    srsran_assert(id == to_coreset_id(0), "Invalid access to CORESET#0 RBs for a coresetId>0");
    coreset0_rbs = crbs;
  }
  const crb_interval& coreset0_crbs() const
  {
    srsran_assert(id == to_coreset_id(0), "Invalid access to CORESET#0 RBs for a coresetId>0");
    return coreset0_rbs;
  }

  void set_freq_domain_resources(freq_resource_bitmap res_bitmap)
  {
    srsran_assert(id != to_coreset_id(0), "Invalid access to CORESET#0 RBs for a coresetId>0");
    other_coreset_freq_resources = res_bitmap;
  }
  const freq_resource_bitmap& freq_domain_resources() const
  {
    srsran_assert(id != to_coreset_id(0), "Invalid access to CORESET#0 RBs for a coresetId>0");
    return other_coreset_freq_resources;
  }

  bool operator==(const coreset_configuration& rhs) const
  {
    if (std::tie(id, duration, interleaved, precoder_granurality, pdcch_dmrs_scrambling_id) !=
        std::tie(rhs.id, rhs.duration, rhs.interleaved, rhs.precoder_granurality, rhs.pdcch_dmrs_scrambling_id)) {
      return false;
    }
    if (id == to_coreset_id(0)) {
      return (coreset0_crbs() == rhs.coreset0_crbs());
    }
    return (freq_domain_resources() == rhs.freq_domain_resources());
  }

  /// Computes \f$N_{CCE}\f$, as per TS 38.213, Section 9.2.1, which is the number of CCEs in a CORESET of a PDCCH
  /// reception with DCI format 1_0 or DCI format 1_1.
  unsigned get_nof_cces() const
  {
    // For \c frequencyDomainResources, TS 38.331, each 1 in the bitset corresponds to 6 RBs.
    unsigned nof_rbs = id == to_coreset_id(0)
                           ? coreset0_rbs.length()
                           : (other_coreset_freq_resources.count() * pdcch_constants::NOF_RB_PER_FREQ_RESOURCE);
    // A CCE corresponds to 6 REGs, where a REG is 1 RB x 1 symbol.
    return (nof_rbs * duration) / pdcch_constants::NOF_REG_PER_CCE;
  }

  /// Computes the lowest RB used by the CORESET.
  unsigned get_coreset_start_crb() const
  {
    static constexpr unsigned NOF_RBS_PER_GROUP = 6U;
    if (id == to_coreset_id(0)) {
      return coreset0_crbs().start();
    }
    const uint64_t lowest_bit = other_coreset_freq_resources.find_lowest(0, other_coreset_freq_resources.size());
    return lowest_bit * NOF_RBS_PER_GROUP;
  }

private:
  crb_interval         coreset0_rbs;
  freq_resource_bitmap other_coreset_freq_resources;
};

} // namespace srsran
