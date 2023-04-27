/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

/// RB grant that can be of allocation type 0 (RBGs) or 1 (VRB range).
struct rb_grant {
  /// Creates an empty interval.
  rb_grant() = default;

  /// \brief Creates a RB grant of allocation type1, i.e. a contiguous range of RBs.
  rb_grant(const vrb_interval& other) noexcept : alloc_type_0(false), alloc(other) {}

  /// \brief Creates a RB grant of allocation type0, i.e. a set of potentially non-contiguous RBGs.
  rb_grant(const rbg_bitmap& other) noexcept : alloc_type_0(true), alloc(other) {}

  /// \brief Creates a copy of the RB grant.
  rb_grant(const rb_grant& other) noexcept : alloc_type_0(other.alloc_type_0), alloc(other.alloc_type_0, other.alloc) {}

  rb_grant& operator=(const rb_grant& other) noexcept
  {
    if (this == &other) {
      return *this;
    }
    if (other.alloc_type_0) {
      *this = other.rbgs();
    } else {
      *this = other.vrbs();
    }
    return *this;
  }
  rb_grant& operator=(const vrb_interval& vrbs)
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
  rb_grant& operator=(const rbg_bitmap& rbgs)
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
  ~rb_grant()
  {
    if (is_alloc_type0()) {
      alloc.rbgs.~rbg_bitmap();
    } else {
      alloc.interv.~vrb_interval();
    }
  }

  /// Checks whether the grant is of type0.
  bool is_alloc_type0() const { return alloc_type_0; }

  /// Checks whether the grant is of type1.
  bool is_alloc_type1() const { return not is_alloc_type0(); }

  /// Extracts the RBG bitmap of the grant, in case it is of type0. This function fails if allocation is of type1.
  const rbg_bitmap& rbgs() const
  {
    srsran_assert(is_alloc_type0(), "Access to rbgs() for prb_grant with allocation type 1 is invalid");
    return alloc.rbgs;
  }
  rbg_bitmap& rbgs()
  {
    srsran_assert(is_alloc_type0(), "Access to rbgs() for prb_grant with allocation type 1 is invalid");
    return alloc.rbgs;
  }

  /// Extracts the VRB interval of the grant, in case it is of type1. This function fails if allocation is of type0.
  const vrb_interval& vrbs() const
  {
    srsran_assert(is_alloc_type1(), "Access to vrbs() for prb_grant with allocation type 0 is invalid");
    return alloc.interv;
  }
  vrb_interval& vrbs()
  {
    srsran_assert(is_alloc_type1(), "Access to vrbs() of prb_grant with allocation type 0 is invalid");
    return alloc.interv;
  }

  /// \brief Verifies if grant is not empty in terms of RBs.
  bool any() const { return is_alloc_type0() ? rbgs().count() : vrbs().empty(); }

  /// Applies to "this" the intersection of "this" with the provided interval.
  rb_grant& operator&=(const vrb_interval interv)
  {
    if (is_alloc_type0()) {
      alloc.rbgs &= rbg_bitmap(alloc.rbgs.size()).fill(interv.start(), interv.stop());
    } else {
      alloc.interv.intersect(interv);
    }
    return *this;
  }

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
struct formatter<srsran::rb_grant> : public formatter<srsran::vrb_interval> {
  template <typename FormatContext>
  auto format(const srsran::rb_grant& grant, FormatContext& ctx)
  {
    if (grant.is_alloc_type0()) {
      return format_to(ctx.out(), "{}", grant.rbgs());
    }
    return format_to(ctx.out(), "{}", grant.vrbs());
  }
};

} // namespace fmt
