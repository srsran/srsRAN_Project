/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_SCHED_NR_RB_H
#define SRSRAN_SCHED_NR_RB_H

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/interval.h"
#include "srsgnb/ran/prb_grant.h"

namespace srsgnb {

/// TS 38.214, Table 6.1.2.2.1-1 - Nominal RBG size P
uint32_t get_P(uint32_t bwp_nof_prb, bool config_1_or_2);

/// TS 38.214 - total number of RBGs for a uplink bandwidth part of size "bwp_nof_prb" PRBs
uint32_t get_nof_rbgs(uint32_t bwp_nof_prb, uint32_t bwp_start, bool config1_or_2);

struct bwp_rb_bitmap {
public:
  bwp_rb_bitmap() = default;
  bwp_rb_bitmap(uint32_t bwp_nof_prbs, uint32_t bwp_prb_start_, bool config1_or_2);

  void reset()
  {
    prbs_.reset();
    rbgs_.reset();
  }

  template <typename T>
  void operator|=(const T& grant)
  {
    add(grant);
  }

  void add(const prb_interval& prbs)
  {
    prbs_.fill(prbs.start(), prbs.stop());
    add_prbs_to_rbgs(prbs);
  }
  void add(const prb_bitmap& grant)
  {
    prbs_ |= grant;
    add_prbs_to_rbgs(grant);
  }
  void add(const rbg_bitmap& grant)
  {
    rbgs_ |= grant;
    add_rbgs_to_prbs(grant);
  }
  void add(const prb_grant& grant)
  {
    if (grant.is_alloc_type0()) {
      add(grant.rbgs());
    } else {
      add(grant.prbs());
    }
  }
  bool collides(const prb_grant& grant) const
  {
    if (grant.is_alloc_type0()) {
      return (rbgs() & grant.rbgs()).any();
    }
    return prbs().any(grant.prbs().start(), grant.prbs().stop());
  }
  bool test(uint32_t prb_idx) { return prbs().test(prb_idx); }
  void set(uint32_t prb_idx)
  {
    prbs_.set(prb_idx);
    rbgs_.set(prb_to_rbg_idx(prb_idx));
  }

  const prb_bitmap& prbs() const { return prbs_; }
  const rbg_bitmap& rbgs() const { return rbgs_; }
  uint32_t          P() const { return P_; }
  uint32_t          nof_prbs() const { return prbs_.size(); }
  uint32_t          nof_rbgs() const { return rbgs_.size(); }

  uint32_t prb_to_rbg_idx(uint32_t prb_idx) const;

  bwp_rb_bitmap& operator|=(const bwp_rb_bitmap& other)
  {
    prbs_ |= other.prbs_;
    rbgs_ |= other.rbgs_;
    return *this;
  }
  bwp_rb_bitmap& operator|=(const rbg_bitmap& other)
  {
    add(other);
    return *this;
  }
  bwp_rb_bitmap& operator|=(const prb_bitmap& other)
  {
    add(other);
    return *this;
  }

private:
  prb_bitmap prbs_;
  rbg_bitmap rbgs_;
  uint32_t   P_             = 0;
  uint32_t   Pnofbits       = 0;
  uint32_t   first_rbg_size = 0;

  void add_prbs_to_rbgs(const prb_bitmap& grant);
  void add_prbs_to_rbgs(const prb_interval& grant);
  void add_rbgs_to_prbs(const rbg_bitmap& grant);
};

template <typename Other>
bwp_rb_bitmap operator|(const bwp_rb_bitmap& lhs, const Other& rhs)
{
  return bwp_rb_bitmap(lhs) |= rhs;
}

inline prb_interval
find_next_empty_interval(const prb_bitmap& mask, size_t start_prb_idx = 0, size_t last_prb_idx = MAX_NOF_PRBS)
{
  int rb_start = mask.find_lowest(start_prb_idx, std::min(mask.size(), last_prb_idx), false);
  if (rb_start != -1) {
    int rb_end = mask.find_lowest(rb_start + 1, std::min(mask.size(), last_prb_idx), true);
    return {(uint32_t)rb_start, (uint32_t)(rb_end < 0 ? mask.size() : rb_end)};
  }
  return {};
}

inline prb_interval find_empty_interval_of_length(const prb_bitmap& mask, size_t nof_prbs, uint32_t start_prb_idx = 0)
{
  prb_interval max_interv;
  do {
    prb_interval interv = find_next_empty_interval(mask, start_prb_idx, mask.size());
    if (interv.empty()) {
      break;
    }
    if (interv.length() >= nof_prbs) {
      max_interv.set(interv.start(), interv.start() + nof_prbs);
      break;
    }
    if (interv.length() > max_interv.length()) {
      max_interv = interv;
    }
    start_prb_idx = interv.stop() + 1;
  } while (start_prb_idx < mask.size());
  return max_interv;
}

} // namespace srsgnb

namespace fmt {

template <>
struct formatter<srsgnb::prb_grant> : public formatter<srsgnb::rbg_bitmap> {
  template <typename FormatContext>
  auto format(const srsgnb::prb_grant& grant, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    if (grant.is_alloc_type1()) {
      return formatter<srsgnb::interval<uint32_t> >{}.format(grant.prbs(), ctx);
    }
    return formatter<srsgnb::rbg_bitmap>::format(grant.rbgs(), ctx);
  }
};

} // namespace fmt

#endif // SRSRAN_SCHED_NR_RB_H
