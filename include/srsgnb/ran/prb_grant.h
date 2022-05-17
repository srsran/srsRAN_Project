/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PRB_GRANT_H
#define SRSGNB_PRB_GRANT_H

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/scheduler/sched_consts.h"

namespace srsgnb {

/// Bitset of PRBs with size up to 275.
using prb_bitmap = bounded_bitset<MAX_NOF_PRBS, true>;

/// Bitset of RBGs with size up to 18
using rbg_bitmap = bounded_bitset<MAX_NOF_RBGS, true>;

/// Struct to express a {min,...,max} range of PRBs
struct prb_interval : public interval<uint32_t> {
  using interval::interval;
};

/// PRB grant that can be of allocation type 0 (RBGs) or 1 (PRB interval)
struct prb_grant {
  prb_grant() = default;
  prb_grant(const prb_interval& other) noexcept : alloc_type_0(false), alloc(other) {}
  prb_grant(const rbg_bitmap& other) noexcept : alloc_type_0(true), alloc(other) {}
  prb_grant(const prb_grant& other) noexcept : alloc_type_0(other.alloc_type_0), alloc(other.alloc_type_0, other.alloc)
  {}
  prb_grant& operator=(const prb_grant& other) noexcept
  {
    if (this == &other) {
      return *this;
    }
    if (other.alloc_type_0) {
      *this = other.rbgs();
    } else {
      *this = other.prbs();
    }
    return *this;
  }
  prb_grant& operator=(const prb_interval& prbs)
  {
    if (alloc_type_0) {
      alloc_type_0 = false;
      alloc.rbgs.~rbg_bitmap();
      new (&alloc.interv) prb_interval(prbs);
    } else {
      alloc.interv = prbs;
    }
    return *this;
  }
  prb_grant& operator=(const rbg_bitmap& rbgs)
  {
    if (alloc_type_0) {
      alloc.rbgs = rbgs;
    } else {
      alloc_type_0 = true;
      alloc.interv.~prb_interval();
      new (&alloc.rbgs) rbg_bitmap(rbgs);
    }
    return *this;
  }
  ~prb_grant()
  {
    if (is_alloc_type0()) {
      alloc.rbgs.~rbg_bitmap();
    } else {
      alloc.interv.~prb_interval();
    }
  }

  bool              is_alloc_type0() const { return alloc_type_0; }
  bool              is_alloc_type1() const { return not is_alloc_type0(); }
  const rbg_bitmap& rbgs() const
  {
    srsran_assert(is_alloc_type0(), "Invalid access to rbgs() field of grant with alloc type 1");
    return alloc.rbgs;
  }
  const prb_interval& prbs() const
  {
    srsran_assert(is_alloc_type1(), "Invalid access to prbs() field of grant with alloc type 0");
    return alloc.interv;
  }
  rbg_bitmap& rbgs()
  {
    srsran_assert(is_alloc_type0(), "Invalid access to rbgs() field of grant with alloc type 1");
    return alloc.rbgs;
  }
  prb_interval& prbs()
  {
    srsran_assert(is_alloc_type1(), "Invalid access to prbs() field of grant with alloc type 0");
    return alloc.interv;
  }

  prb_grant& operator&=(const prb_interval interv)
  {
    if (is_alloc_type0()) {
      alloc.rbgs &= rbg_bitmap{alloc.rbgs.size()}.fill(interv.start(), interv.stop());
    } else {
      alloc.interv.intersect(interv);
    }
    return *this;
  }

private:
  bool alloc_type_0 = false;
  union alloc_t {
    rbg_bitmap   rbgs;
    prb_interval interv;

    alloc_t() : interv(0, 0) {}
    explicit alloc_t(const prb_interval& prbs) : interv(prbs) {}
    explicit alloc_t(const rbg_bitmap& rbgs_) : rbgs(rbgs_) {}
    alloc_t(bool type0, const alloc_t& other)
    {
      if (type0) {
        new (&rbgs) rbg_bitmap(other.rbgs);
      } else {
        new (&interv) prb_interval(other.interv);
      }
    }
  } alloc;
};

inline prb_bitmap& operator|=(prb_bitmap& prb_bitmap, const prb_interval& grant)
{
  prb_bitmap.fill(grant.start(), grant.stop());
  return prb_bitmap;
}

} // namespace srsgnb

namespace fmt {

/// FMT formatter for prb_intervals
template <>
struct formatter<srsgnb::prb_interval> : public formatter<srsgnb::interval<uint32_t> > {};

} // namespace fmt

#endif // SRSGNB_PRB_GRANT_H
