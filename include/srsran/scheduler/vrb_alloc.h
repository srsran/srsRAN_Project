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

#include "resource_block_group.h"
#include "srsran/adt/bounded_bitset.h"
#include "srsran/ran/resource_allocation/rb_interval.h"
#include "srsran/ran/resource_block.h"
#include "srsran/ran/sliv.h"

namespace srsran {

/// Bitset of PRBs with size up to 275.
using prb_bitmap = bounded_bitset<MAX_NOF_PRBS, true>;

/// VRB Resource Allocation that can be of allocation type 0 (RBGs) or 1 (VRB range).
struct vrb_alloc {
  /// Creates an empty interval.
  vrb_alloc() = default;

  /// \brief Creates a RB grant of allocation type1, i.e. a contiguous range of RBs.
  vrb_alloc(const vrb_interval& other) noexcept : alloc_type_0(false), alloc(other) {}

  /// \brief Creates a RB grant of allocation type0, i.e. a set of potentially non-contiguous RBGs.
  vrb_alloc(const rbg_bitmap& other) noexcept : alloc_type_0(true), alloc(other) {}

  /// \brief Creates a copy of the RB grant.
  vrb_alloc(const vrb_alloc& other) noexcept : alloc_type_0(other.alloc_type_0), alloc(other.alloc_type_0, other.alloc)
  {
  }

  vrb_alloc& operator=(const vrb_alloc& other) noexcept
  {
    if (this == &other) {
      return *this;
    }
    if (other.alloc_type_0) {
      *this = other.type0();
    } else {
      *this = other.type1();
    }
    return *this;
  }
  vrb_alloc& operator=(const vrb_interval& vrbs)
  {
    if (alloc_type_0) {
      alloc_type_0 = false;
      alloc.rbgs.~rbg_bitmap();
      new (&alloc.interv) vrb_interval(vrbs);
    } else {
      alloc.interv = vrbs;
    }
    return *this;
  }
  vrb_alloc& operator=(const rbg_bitmap& rbgs)
  {
    if (alloc_type_0) {
      alloc.rbgs = rbgs;
    } else {
      alloc_type_0 = true;
      alloc.interv.~vrb_interval();
      new (&alloc.rbgs) rbg_bitmap(rbgs);
    }
    return *this;
  }
  ~vrb_alloc()
  {
    if (is_type0()) {
      alloc.rbgs.~rbg_bitmap();
    } else {
      alloc.interv.~vrb_interval();
    }
  }

  /// Checks whether the grant is of type0.
  bool is_type0() const { return alloc_type_0; }

  /// Checks whether the grant is of type1.
  bool is_type1() const { return not is_type0(); }

  /// \brief Extracts the RBG bitmap of the allocation, in the case it is of type0. This function fails if allocation
  /// is of type1.
  const rbg_bitmap& type0() const
  {
    srsran_assert(is_type0(), "Access to type0() for prb_grant with allocation type 1 is invalid");
    return alloc.rbgs;
  }
  rbg_bitmap& type0()
  {
    srsran_assert(is_type0(), "Access to type0() for prb_grant with allocation type 1 is invalid");
    return alloc.rbgs;
  }

  /// \brief Gets the VRB interval of the resource allocation, in case it is of type1. This function fails if
  /// allocation is of type0.
  const vrb_interval& type1() const
  {
    srsran_assert(is_type1(), "Access to type1() for prb_grant with allocation type 0 is invalid");
    return alloc.interv;
  }
  vrb_interval& type1()
  {
    srsran_assert(is_type1(), "Access to type1() of prb_grant with allocation type 0 is invalid");
    return alloc.interv;
  }

  /// \brief Verifies if grant is not empty in terms of RBs.
  bool any() const { return is_type0() ? type0().count() : type1().empty(); }

private:
  bool alloc_type_0 = false;
  union alloc_t {
    rbg_bitmap   rbgs;
    vrb_interval interv;

    alloc_t() : interv(0, 0) {}
    explicit alloc_t(const vrb_interval& prbs) : interv(prbs) {}
    explicit alloc_t(const rbg_bitmap& rbgs_) : rbgs(rbgs_) {}
    alloc_t(bool type0, const alloc_t& other)
    {
      if (type0) {
        new (&rbgs) rbg_bitmap(other.rbgs);
      } else {
        new (&interv) vrb_interval(other.interv);
      }
    }
  } alloc;
};

inline prb_bitmap& operator|=(prb_bitmap& prb_bits, const prb_interval& grant)
{
  prb_bits.fill(grant.start(), grant.stop());
  return prb_bits;
}

/// Converts RBG bitmap to PRB bitmap given a BWP PRB dimensions and the nominal RBG-size.
/// \remark See TS 38.214, Sections 5.1.2.2.1 and 6.1.2.2.1.
prb_bitmap convert_rbgs_to_prbs(const rbg_bitmap& rbgs, crb_interval bwp_rbs, nominal_rbg_size P);

} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::vrb_alloc> : public formatter<srsran::rbg_bitmap> {
  template <typename FormatContext>
  auto format(const srsran::vrb_alloc& grant, FormatContext& ctx)
  {
    if (grant.is_type0()) {
      if (this->mode == hexadecimal) {
        return format_to(ctx.out(), "{:x}", grant.type0());
      }
      return format_to(ctx.out(), "{}", grant.type0());
    }
    return format_to(ctx.out(), "{}", grant.type1());
  }
};

} // namespace fmt
