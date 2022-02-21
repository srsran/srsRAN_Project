
#include "mac_ctrl.h"

using namespace srsgnb;

mac_ue_map::mac_ue_map()
{
  std::fill(rnti_to_ue_index_map.begin(), rnti_to_ue_index_map.end(), MAX_NOF_UES);
}

mac_ue_ctrl* mac_ue_map::find_by_rnti(rnti_t rnti)
{
  srsran_assert(rnti != INVALID_RNTI, "Invalid rnti=0x%x", rnti);
  du_ue_index_t ue_index = rnti_to_ue_index_map[rnti % MAX_NOF_UES];
  if (ue_index == MAX_NOF_UES) {
    return nullptr;
  }
  return find(ue_index);
}

mac_ue_ctrl* mac_ue_map::find(du_ue_index_t ue_index)
{
  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index=%d", ue_index);
  return ue_db[ue_index].has_value() ? &ue_db[ue_index].value() : nullptr;
}

bool mac_ue_map::insert(du_ue_index_t ue_index, rnti_t crnti, du_cell_index_t cell_index)
{
  srsran_assert(crnti != INVALID_RNTI, "Invalid RNTI");
  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index=%d", ue_index);
  if (rnti_to_ue_index_map[crnti % MAX_NOF_UES] < MAX_NOF_UES) {
    // rnti already exists
    return false;
  }

  if (ue_db[ue_index].has_value()) {
    // UE already existed with same ue_index
    return false;
  }

  // Create UE object
  ue_db[ue_index].emplace();
  ue_db[ue_index]->rnti        = crnti;
  ue_db[ue_index]->du_ue_index = ue_index;
  ue_db[ue_index]->pcell_idx   = cell_index;

  rnti_to_ue_index_map[crnti % MAX_NOF_UES] = ue_index;
  return true;
}

bool mac_ue_map::erase(du_ue_index_t ue_index)
{
  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index=%d", ue_index);

  if (not ue_db[ue_index].has_value()) {
    // no UE existed with provided ue_index
    return false;
  }
  srsran_sanity_check(ue_db[ue_index]->rnti != INVALID_RNTI, "ue_index=%d has invalid RNTI", ue_index);
  ue_db[ue_index].reset();

  return true;
}
