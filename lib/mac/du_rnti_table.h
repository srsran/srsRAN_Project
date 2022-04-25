
#ifndef SRSGNB_DU_RNTI_TABLE_H
#define SRSGNB_DU_RNTI_TABLE_H

#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/support/srsran_assert.h"
#include <atomic>
#include <limits>
#include <vector>

namespace srsgnb {

/// \brief Table used by MAC to convert from RNTI to DU UE index.
/// This class is thread-safe.
class du_rnti_table
{
public:
  du_rnti_table() : rnti_to_ue_index_map(std::make_unique<array_type>())
  {
    for (unsigned idx = 0; idx < rnti_to_ue_index_map->size(); ++idx) {
      (*rnti_to_ue_index_map)[idx].store(MAX_DU_NOF_UES, std::memory_order_relaxed);
    }
  }
  du_rnti_table(const du_rnti_table&) = delete;
  du_rnti_table(du_rnti_table&&)      = delete;
  du_rnti_table& operator=(const du_rnti_table&) = delete;
  du_rnti_table& operator=(du_rnti_table&&) = delete;

  void add_ue(rnti_t crnti, du_ue_index_t ue_index)
  {
    srsran_sanity_check(is_crnti(crnti), "Invalid C-RNTI={:#x}", crnti);
    srsran_sanity_check(is_du_ue_index_valid(ue_index), "Invalid ueId={}", ue_index);
    std::atomic<du_ue_index_t>& ue_pos      = get(crnti);
    du_ue_index_t               prev_ue_idx = ue_pos.exchange(ue_index, std::memory_order_relaxed);
    if (not is_du_ue_index_valid(prev_ue_idx)) {
      nof_ues_.fetch_add(1, std::memory_order_relaxed);
    }
  }

  void rem_ue(rnti_t crnti)
  {
    srsran_sanity_check(is_crnti(crnti), "Invalid C-RNTI={:#x}", crnti);
    std::atomic<du_ue_index_t>& ue_pos      = get(crnti);
    du_ue_index_t               prev_ue_idx = ue_pos.exchange(MAX_DU_NOF_UES, std::memory_order_relaxed);
    if (is_du_ue_index_valid(prev_ue_idx)) {
      nof_ues_.fetch_sub(1, std::memory_order_relaxed);
    }
  }

  /// Get an estimate of the current number of UEs present in the table.
  size_t nof_ues() const { return nof_ues_.load(std::memory_order_relaxed); }

  /// Checks whether passed RNTI is registered as a UE.
  bool has_rnti(rnti_t crnti) const { return is_du_ue_index_valid(this->operator[](crnti)); }

  du_ue_index_t operator[](rnti_t crnti) const
  {
    srsran_sanity_check(is_crnti(crnti), "Invalid C-RNTI={:#x}", crnti);
    const std::atomic<du_ue_index_t>& ue_pos = get(crnti);
    return ue_pos.load(std::memory_order_relaxed);
  }

private:
  static constexpr rnti_t MIN_CRNTI  = rnti_t::MIN_CRNTI;
  static constexpr rnti_t MAX_CRNTI  = rnti_t::MAX_CRNTI;
  static constexpr int    RNTI_RANGE = MAX_CRNTI + 1 - MIN_CRNTI;
  using array_type                   = std::array<std::atomic<du_ue_index_t>, RNTI_RANGE>;

  std::atomic<du_ue_index_t>&       get(rnti_t rnti) { return (*rnti_to_ue_index_map)[rnti - MIN_CRNTI]; }
  const std::atomic<du_ue_index_t>& get(rnti_t rnti) const { return (*rnti_to_ue_index_map)[rnti - MIN_CRNTI]; }

  // Table of RNTI -> UE index with size 65535.
  std::unique_ptr<array_type> rnti_to_ue_index_map;

  std::atomic<size_t> nof_ues_{0};
};

} // namespace srsgnb

#endif // SRSGNB_DU_RNTI_TABLE_H
