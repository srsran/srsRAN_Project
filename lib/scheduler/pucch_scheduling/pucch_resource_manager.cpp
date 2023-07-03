/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../support/pucch/pucch_default_resource.h"
#include "pucch_allocator_impl.h"

using namespace srsran;

/////////////    RESOURCE MANAGER     /////////////

/////////////   Static function   /////////////

static int get_pucch_res_idx_for_csi(const ue_cell_configuration& ue_cell_cfg)
{
  // We assume we use only 1 CSI report.
  const unsigned csi_report_cfg_idx = 0;
  // We assume we use the First BWP.
  // TODO: extend by passing the BWP id.
  const bwp_id_t bwp_id      = srsran::MIN_BWP_ID;
  const auto& csi_report_cfg = ue_cell_cfg.cfg_dedicated().csi_meas_cfg.value().csi_report_cfg_list[csi_report_cfg_idx];
  auto&       csi_pucch_res_list =
      variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(csi_report_cfg.report_cfg_type)
          .pucch_csi_res_list;

  const auto& it = std::find_if(csi_pucch_res_list.begin(),
                                csi_pucch_res_list.end(),
                                [](const csi_report_config::pucch_csi_resource& csi) { return csi.ul_bwp == bwp_id; });

  if (it != csi_pucch_res_list.end()) {
    return static_cast<int>(it->pucch_res_id);
  }

  return -1;
}

/////////////   Public methods   /////////////

pucch_resource_manager::pucch_resource_manager()
{
  auto reset_slot_record = [](rnti_pucch_res_id_slot_record& res_counter) {
    for (auto& ue_rec : res_counter.ues_using_pucch_res) {
      ue_rec.rnti   = INVALID_RNTI;
      ue_rec.format = pucch_format::NOF_FORMATS;
    }
    for (auto& res : res_counter.used_common_resources) {
      res = false;
    }
  };

  std::for_each(resource_slots.begin(), resource_slots.end(), reset_slot_record);
}

void pucch_resource_manager::slot_indication(slot_point slot_tx)
{
  // If last_sl_ind is not valid (not initialized), then the check sl_tx == last_sl_ind + 1 does not matter.
  srsran_sanity_check(not last_sl_ind.valid() or slot_tx == last_sl_ind + 1, "Detected a skipped slot");

  // Update Slot.
  last_sl_ind = slot_tx;

  rnti_pucch_res_id_slot_record& res_counter = get_slot_resource_counter(last_sl_ind - 1);

  for (auto& ue_rec : res_counter.ues_using_pucch_res) {
    ue_rec.rnti   = INVALID_RNTI;
    ue_rec.format = pucch_format::NOF_FORMATS;
  }
  for (auto& res : res_counter.used_common_resources) {
    res = false;
  }
}

bool pucch_resource_manager::is_common_resource_available(slot_point sl, size_t r_pucch)
{
  srsran_assert(r_pucch < 16, "r_PUCCH must be less than 16");
  return not get_slot_resource_counter(sl).used_common_resources[r_pucch];
}

void pucch_resource_manager::reserve_common_resource(slot_point sl, size_t r_pucch)
{
  srsran_assert(r_pucch < 16, "r_PUCCH must be less than 16");
  get_slot_resource_counter(sl).used_common_resources[r_pucch] = true;
}

pucch_harq_resource_alloc_record
pucch_resource_manager::reserve_next_f1_harq_res_available(slot_point          slot_harq,
                                                           rnti_t              crnti,
                                                           const pucch_config& pucch_cfg)
{
  return reserve_next_harq_res_available(slot_harq, crnti, pucch_cfg, pucch_format::FORMAT_1);
};

pucch_harq_resource_alloc_record
pucch_resource_manager::reserve_next_f2_harq_res_available(slot_point          slot_harq,
                                                           rnti_t              crnti,
                                                           const pucch_config& pucch_cfg)
{
  return reserve_next_harq_res_available(slot_harq, crnti, pucch_cfg, pucch_format::FORMAT_2);
};

const pucch_resource* pucch_resource_manager::reserve_specific_format2_res(slot_point          slot_harq,
                                                                           rnti_t              crnti,
                                                                           unsigned            res_indicator,
                                                                           const pucch_config& pucch_cfg)
{
  srsran_sanity_check(slot_harq < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated to far into the future");

  // Get resource list of wanted slot.
  rnti_pucch_res_id_slot_record& res_counter = get_slot_resource_counter(slot_harq);

  const auto& ue_res_id_set_for_harq_f2 = pucch_cfg.pucch_res_set[PUCCH_HARQ_F2_RES_SET_ID].pucch_res_id_list;

  if (res_indicator >= ue_res_id_set_for_harq_f2.size()) {
    return nullptr;
  }

  const unsigned pucch_res_id = ue_res_id_set_for_harq_f2[res_indicator];

  const auto& pucch_res_list    = pucch_cfg.pucch_res_list;
  auto&       pucch_res_tracker = res_counter.ues_using_pucch_res[pucch_res_id];

  if (pucch_res_tracker.rnti == INVALID_RNTI) {
    pucch_res_tracker.rnti   = crnti;
    pucch_res_tracker.format = pucch_format::FORMAT_2;
    return &pucch_res_list[pucch_res_id];
  }
  return nullptr;
}

const pucch_resource* pucch_resource_manager::reserve_csi_resource(slot_point                   slot_csi,
                                                                   rnti_t                       crnti,
                                                                   const ue_cell_configuration& ue_cell_cfg)
{
  srsran_sanity_check(slot_csi < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated to far into the future");

  const int csi_pucch_res_idx = get_pucch_res_idx_for_csi(ue_cell_cfg);
  if (csi_pucch_res_idx < 0) {
    return nullptr;
  }
  auto& slot_record = get_slot_resource_counter(slot_csi);
  if (slot_record.ues_using_pucch_res[csi_pucch_res_idx].rnti != INVALID_RNTI) {
    return nullptr;
  }

  slot_record.ues_using_pucch_res[csi_pucch_res_idx].rnti   = crnti;
  slot_record.ues_using_pucch_res[csi_pucch_res_idx].format = pucch_format::FORMAT_2;
  return &ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value().pucch_res_list[csi_pucch_res_idx];
};

const pucch_resource*
pucch_resource_manager::reserve_sr_res_available(slot_point slot_sr, rnti_t crnti, const pucch_config& pucch_cfg)
{
  srsran_sanity_check(slot_sr < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated to far into the future");
  srsran_sanity_check(pucch_cfg.sr_res_list.size() == 1, "UE SR resource list must have size 1.");

  auto& slot_record = get_slot_resource_counter(slot_sr);

  // We assume each UE only has 1 SR Resource Config configured.
  const unsigned sr_pucch_res_id = pucch_cfg.sr_res_list[0].pucch_res_id;
  if (slot_record.ues_using_pucch_res[sr_pucch_res_id].rnti != INVALID_RNTI) {
    return nullptr;
  }

  slot_record.ues_using_pucch_res[sr_pucch_res_id].rnti   = crnti;
  slot_record.ues_using_pucch_res[sr_pucch_res_id].format = pucch_format::FORMAT_2;
  return &pucch_cfg.pucch_res_list[sr_pucch_res_id];
};

bool pucch_resource_manager::release_harq_f1_resource(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg)
{
  return release_harq_resource(slot_harq, crnti, pucch_cfg, pucch_format::FORMAT_1);
}

bool pucch_resource_manager::release_harq_f2_resource(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg)
{
  return release_harq_resource(slot_harq, crnti, pucch_cfg, pucch_format::FORMAT_2);
}

bool pucch_resource_manager::release_sr_resource(slot_point slot_sr, rnti_t crnti, const pucch_config& pucch_cfg)
{
  srsran_sanity_check(slot_sr < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated to far into the future");
  srsran_sanity_check(pucch_cfg.sr_res_list.size() == 1, "UE SR resource list must have size 1.");

  auto& slot_record = get_slot_resource_counter(slot_sr);

  // We assume each UE only has 1 SR Resource Config configured.
  const unsigned sr_pucch_res_id = pucch_cfg.sr_res_list[0].pucch_res_id;

  if (slot_record.ues_using_pucch_res[sr_pucch_res_id].rnti != crnti) {
    return false;
  }

  slot_record.ues_using_pucch_res[sr_pucch_res_id].rnti   = INVALID_RNTI;
  slot_record.ues_using_pucch_res[sr_pucch_res_id].format = pucch_format::NOF_FORMATS;
  return true;
}

bool pucch_resource_manager::release_csi_resource(slot_point                   slot_sr,
                                                  rnti_t                       crnti,
                                                  const ue_cell_configuration& ue_cell_cfg)
{
  srsran_sanity_check(slot_sr < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated to far into the future");

  auto& slot_record = get_slot_resource_counter(slot_sr);

  // We assume each UE only has 1 SR Resource Config configured.
  const int csi_pucch_res_idx = get_pucch_res_idx_for_csi(ue_cell_cfg);
  if (csi_pucch_res_idx < 0) {
    return false;
  }
  if (slot_record.ues_using_pucch_res[csi_pucch_res_idx].rnti != crnti) {
    return false;
  }

  slot_record.ues_using_pucch_res[csi_pucch_res_idx].rnti   = INVALID_RNTI;
  slot_record.ues_using_pucch_res[csi_pucch_res_idx].format = pucch_format::NOF_FORMATS;
  return true;
}

int pucch_resource_manager::fetch_f1_pucch_res_indic(slot_point slot_tx, rnti_t crnti, const pucch_config& pucch_cfg)
{
  return fetch_pucch_res_indic(slot_tx, crnti, pucch_cfg, pucch_format::FORMAT_1);
}

int pucch_resource_manager::fetch_f2_pucch_res_indic(slot_point slot_tx, rnti_t crnti, const pucch_config& pucch_cfg)
{
  return fetch_pucch_res_indic(slot_tx, crnti, pucch_cfg, pucch_format::FORMAT_2);
}

const pucch_resource* pucch_resource_manager::fetch_csi_pucch_res_config(slot_point                   slot_tx,
                                                                         rnti_t                       crnti,
                                                                         const ue_cell_configuration& ue_cell_cfg)
{
  srsran_sanity_check(slot_tx < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated to far into the future");

  rnti_pucch_res_id_slot_record& slot_record = get_slot_resource_counter(slot_tx);

  // We assume each UE only has 1 SR Resource Config configured.
  const int csi_pucch_res_idx = get_pucch_res_idx_for_csi(ue_cell_cfg);
  if (csi_pucch_res_idx < 0) {
    return nullptr;
  }

  if (slot_record.ues_using_pucch_res[csi_pucch_res_idx].rnti != crnti) {
    return nullptr;
  }

  const pucch_config& pucch_cfg = ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();

  return &pucch_cfg.pucch_res_list[csi_pucch_res_idx];
}

pucch_harq_resource_alloc_record pucch_resource_manager::reserve_next_harq_res_available(slot_point          slot_harq,
                                                                                         rnti_t              crnti,
                                                                                         const pucch_config& pucch_cfg,
                                                                                         pucch_format        format)
{
  srsran_sanity_check(slot_harq < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated to far into the future");
  srsran_assert(format == pucch_format::FORMAT_1 or format == pucch_format::FORMAT_2,
                "Only PUCCH Format 1 and Format 2 are currently supported");

  // Get resource list of wanted slot.
  rnti_pucch_res_id_slot_record& res_counter = get_slot_resource_counter(slot_harq);

  auto& slot_res_array = res_counter.ues_using_pucch_res;

  const unsigned res_set_idx = format == pucch_format::FORMAT_1 ? PUCCH_HARQ_F1_RES_SET_ID : PUCCH_HARQ_F2_RES_SET_ID;

  // Get the span over the array of resources for the specific UE.
  const auto&            ue_res_id_set_for_harq = pucch_cfg.pucch_res_set[res_set_idx].pucch_res_id_list;
  unsigned               ue_first_f1_res_id     = ue_res_id_set_for_harq.front();
  span<resource_tracker> slot_ue_res_array(&slot_res_array[ue_first_f1_res_id], slot_res_array.size());

  auto* available_resource = std::find_if(slot_ue_res_array.begin(),
                                          slot_ue_res_array.end(),
                                          [](const resource_tracker res) { return res.rnti == INVALID_RNTI; });

  const auto& pucch_res_list = pucch_cfg.pucch_res_list;

  if (available_resource != slot_ue_res_array.end() and
      static_cast<unsigned>(available_resource - slot_ue_res_array.begin()) <
          pucch_cfg.pucch_res_set[res_set_idx].pucch_res_id_list.size()) {
    const unsigned pucch_res_indicator = static_cast<unsigned>(available_resource - slot_ue_res_array.begin());

    const unsigned pucch_res_idx_from_list = ue_res_id_set_for_harq[pucch_res_indicator];

    const auto* res_cfg = std::find_if(
        pucch_res_list.begin(), pucch_res_list.end(), [pucch_res_idx_from_list](const pucch_resource& res) {
          return res.res_id == pucch_res_idx_from_list;
        });

    if (res_cfg != pucch_res_list.end()) {
      available_resource->rnti   = crnti;
      available_resource->format = format;
      return pucch_harq_resource_alloc_record{.pucch_res = &(*res_cfg), .pucch_res_indicator = pucch_res_indicator};
    }
  }
  return pucch_harq_resource_alloc_record{.pucch_res = nullptr};
};

bool pucch_resource_manager::release_harq_resource(slot_point          slot_harq,
                                                   rnti_t              crnti,
                                                   const pucch_config& pucch_cfg,
                                                   pucch_format        format)
{
  srsran_sanity_check(slot_harq < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated to far into the future");
  srsran_assert(format == pucch_format::FORMAT_1 or format == pucch_format::FORMAT_2,
                "Only PUCCH Format 1 and Format 2 are currently supported");

  // Get resource list of wanted slot.
  rnti_pucch_res_id_slot_record& res_counter = get_slot_resource_counter(slot_harq);

  auto& slot_res_array = res_counter.ues_using_pucch_res;

  const unsigned res_set_idx = format == pucch_format::FORMAT_1 ? PUCCH_HARQ_F1_RES_SET_ID : PUCCH_HARQ_F2_RES_SET_ID;

  // Get the span over the array of resources for the specific UE.
  const auto&            ue_res_id_set_for_harq = pucch_cfg.pucch_res_set[res_set_idx].pucch_res_id_list;
  unsigned               ue_first_f1_res_id     = ue_res_id_set_for_harq.front();
  span<resource_tracker> slot_ue_res_array(&slot_res_array[ue_first_f1_res_id], slot_res_array.size());

  auto* target_res = std::find_if(slot_ue_res_array.begin(),
                                  slot_ue_res_array.end(),
                                  [crnti](const resource_tracker res) { return res.rnti == crnti; });

  // If the resources was found, then release it (i.e., remove the C-RNTI of the user allocated to it).
  if (target_res != slot_ue_res_array.end()) {
    target_res->rnti   = INVALID_RNTI;
    target_res->format = pucch_format::NOF_FORMATS;
    return true;
  }

  return false;
}

int pucch_resource_manager::fetch_pucch_res_indic(slot_point          slot_tx,
                                                  rnti_t              crnti,
                                                  const pucch_config& pucch_cfg,
                                                  pucch_format        format)
{
  srsran_sanity_check(slot_tx < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated to far into the future");
  srsran_assert(format == pucch_format::FORMAT_1 or format == pucch_format::FORMAT_2,
                "Only PUCCH Format 1 and Format 2 are currently supported");

  // Get resource list of wanted slot.
  rnti_pucch_res_id_slot_record& res_counter = get_slot_resource_counter(slot_tx);

  auto& slot_res_array = res_counter.ues_using_pucch_res;

  const unsigned res_set_idx = format == pucch_format::FORMAT_1 ? PUCCH_HARQ_F1_RES_SET_ID : PUCCH_HARQ_F2_RES_SET_ID;

  // Get the span over the array of resources for the specific UE.
  const auto&            ue_res_id_set_for_harq = pucch_cfg.pucch_res_set[res_set_idx].pucch_res_id_list;
  unsigned               ue_first_f1_res_id     = ue_res_id_set_for_harq.front();
  span<resource_tracker> slot_ue_res_array(&slot_res_array[ue_first_f1_res_id], slot_res_array.size());

  auto ue_resource =
      std::find_if(slot_ue_res_array.begin(),
                   slot_ue_res_array.end(),
                   [target_rnti = crnti](const resource_tracker res) { return res.rnti == target_rnti; });

  if (ue_resource != slot_ue_res_array.end() and static_cast<unsigned>(ue_resource - slot_ue_res_array.begin()) <
                                                     pucch_cfg.pucch_res_set[res_set_idx].pucch_res_id_list.size()) {
    return static_cast<unsigned>(ue_resource - slot_ue_res_array.begin());
  }

  // -1 indicates that the there is no UE record for given RNTI.
  return -1;
}

pucch_resource_manager::rnti_pucch_res_id_slot_record& pucch_resource_manager::get_slot_resource_counter(slot_point sl)
{
  srsran_sanity_check(sl < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH resource ring-buffer accessed too far into the future");
  return resource_slots[sl.to_uint() % RES_MANAGER_RING_BUFFER_SIZE];
}
