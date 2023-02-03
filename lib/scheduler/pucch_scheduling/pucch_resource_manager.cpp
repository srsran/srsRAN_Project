/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../support/pucch/pucch_default_resource.h"
#include "pucch_allocator_impl.h"

using namespace srsgnb;

/////////////    RESOURCE MANAGER     /////////////

/////////////   Public methods   /////////////

int get_pucch_res_idx_for_csi(const ue_cell_configuration& ue_cell_cfg)
{
  // We assume we use only 1 CSI report.
  const unsigned csi_report_cfg_idx = 0;
  // We assume we use the First BWP.
  // TODO: extend by passing the BWP id.
  const bwp_id_t bwp_id      = srsgnb::MIN_BWP_ID;
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
    res_counter.ue_using_sr_resource  = INVALID_RNTI;
    res_counter.ue_using_csi_resource = INVALID_RNTI;
    for (auto& ue_rec : res_counter.ues_using_format1_res) {
      ue_rec = INVALID_RNTI;
    }
    for (auto& ue_rec : res_counter.ues_using_format2_res) {
      ue_rec = INVALID_RNTI;
    }
  };

  std::for_each(resource_slots.begin(), resource_slots.end(), reset_slot_record);
}

void pucch_resource_manager::slot_indication(slot_point slot_tx)
{
  // Update Slot.
  last_sl_ind = slot_tx;

  rnti_pucch_res_id_slot_record& res_counter = get_slot_resource_counter(last_sl_ind - 1);

  res_counter.ue_using_sr_resource  = INVALID_RNTI;
  res_counter.ue_using_csi_resource = INVALID_RNTI;
  for (auto& ue_rec : res_counter.ues_using_format1_res) {
    ue_rec = INVALID_RNTI;
  }
  for (auto& ue_rec : res_counter.ues_using_format2_res) {
    ue_rec = INVALID_RNTI;
  }
}

pucch_harq_resource_alloc_record pucch_resource_manager::reserve_next_harq_res_available(slot_point          slot_harq,
                                                                                         rnti_t              crnti,
                                                                                         const pucch_config& pucch_cfg)
{
  srsgnb_sanity_check(slot_harq < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated to far into the future");

  // Get resource list of wanted slot.
  rnti_pucch_res_id_slot_record& res_counter = get_slot_resource_counter(slot_harq);

  const auto available_resource = std::find_if(res_counter.ues_using_format1_res.begin(),
                                               res_counter.ues_using_format1_res.end(),
                                               [](const rnti_t rnti) { return rnti == INVALID_RNTI; });

  const auto& pucch_res_list = pucch_cfg.pucch_res_list;

  const unsigned pucch_resource_set_format1_idx = 0;

  if (available_resource != res_counter.ues_using_format1_res.end() and
      static_cast<unsigned>(available_resource - res_counter.ues_using_format1_res.begin()) <
          pucch_cfg.pucch_res_set[pucch_resource_set_format1_idx].pucch_res_id_list.size()) {
    const unsigned pucch_res_indicator =
        static_cast<unsigned>(available_resource - res_counter.ues_using_format1_res.begin());
    *available_resource = crnti;
    const unsigned pucch_res_idx_from_list =
        pucch_cfg.pucch_res_set[pucch_resource_set_format1_idx].pucch_res_id_list[pucch_res_indicator];
    return pucch_harq_resource_alloc_record{.pucch_res           = &pucch_res_list[pucch_res_idx_from_list],
                                            .pucch_res_indicator = pucch_res_indicator};
  }
  return pucch_harq_resource_alloc_record{.pucch_res = nullptr};
};

pucch_harq_resource_alloc_record
pucch_resource_manager::reserve_next_format2_res_available(slot_point          slot_harq,
                                                           rnti_t              crnti,
                                                           const pucch_config& pucch_cfg)
{
  srsgnb_sanity_check(slot_harq < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated to far into the future");

  // Get resource list of wanted slot.
  rnti_pucch_res_id_slot_record& res_counter = get_slot_resource_counter(slot_harq);

  auto available_resource = std::find_if(res_counter.ues_using_format2_res.begin(),
                                         res_counter.ues_using_format2_res.end(),
                                         [](const rnti_t rnti) { return rnti == INVALID_RNTI; });

  const auto& pucch_res_list = pucch_cfg.pucch_res_list;

  const unsigned PUCCH_RESOURCE_SET_FORMAT2_IDX = 1;

  if (available_resource != res_counter.ues_using_format2_res.end() and
      static_cast<unsigned>(available_resource - res_counter.ues_using_format2_res.begin()) <
          pucch_cfg.pucch_res_set[PUCCH_RESOURCE_SET_FORMAT2_IDX].pucch_res_id_list.size()) {
    unsigned pucch_res_indicator =
        static_cast<unsigned>(available_resource - res_counter.ues_using_format2_res.begin());
    *available_resource = crnti;
    unsigned pucch_res_idx_from_list =
        pucch_cfg.pucch_res_set[PUCCH_RESOURCE_SET_FORMAT2_IDX].pucch_res_id_list[pucch_res_indicator];

    return pucch_harq_resource_alloc_record{.pucch_res           = &pucch_res_list[pucch_res_idx_from_list],
                                            .pucch_res_indicator = pucch_res_indicator};
  }
  return pucch_harq_resource_alloc_record{.pucch_res = nullptr};
};

const pucch_resource* pucch_resource_manager::reserve_csi_resource(slot_point                   slot_csi,
                                                                   rnti_t                       crnti,
                                                                   const ue_cell_configuration& ue_cell_cfg)
{
  srsgnb_sanity_check(slot_csi < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated to far into the future");

  auto& slot_record = get_slot_resource_counter(slot_csi);

  const pucch_config& pucch_cfg = ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();

  const int csi_pucch_res_idx = get_pucch_res_idx_for_csi(ue_cell_cfg);
  if (csi_pucch_res_idx < 0) {
    return nullptr;
  }

  // Check if the list of PUCCH resources contains the resource indexed to be used for CSI.
  if (slot_record.ue_using_csi_resource == INVALID_RNTI) {
    const auto& pucch_res_list         = pucch_cfg.pucch_res_list;
    const auto* csi_pucch_resource_cfg = std::find_if(
        pucch_res_list.begin(), pucch_res_list.end(), [csi_pucch_res_idx](const pucch_resource& pucch_res) {
          return static_cast<unsigned>(csi_pucch_res_idx) == pucch_res.res_id;
        });

    // If there is no such PUCCH resource, return \c nullptr.
    if (csi_pucch_resource_cfg == pucch_res_list.end()) {
      return nullptr;
    }

    slot_record.ue_using_csi_resource = crnti;
    return &pucch_cfg.pucch_res_list[csi_pucch_res_idx];
  }
  return nullptr;
};

const pucch_resource*
pucch_resource_manager::reserve_sr_res_available(slot_point slot_sr, rnti_t crnti, const pucch_config& pucch_cfg)
{
  srsgnb_sanity_check(slot_sr < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated to far into the future");
  srsgnb_sanity_check(pucch_cfg.sr_res_list.size() == 1, "UE SR resource list must have size 1.");

  auto& slot_record = get_slot_resource_counter(slot_sr);

  if (slot_record.ue_using_sr_resource == INVALID_RNTI) {
    const auto& pucch_res_list = pucch_cfg.pucch_res_list;

    // Check if the list of PUCCH resources (corresponding to \c resourceToAddModList, as part of \c PUCCH-Config, as
    // per TS 38.331) contains the resource indexed to be used for SR.
    const auto* sr_pucch_resource_cfg =
        std::find_if(pucch_res_list.begin(),
                     pucch_res_list.end(),
                     [sr_res_idx = pucch_cfg.sr_res_list[0].pucch_res_id](const pucch_resource& pucch_sr_res_cfg) {
                       return static_cast<unsigned>(sr_res_idx) == pucch_sr_res_cfg.res_id;
                     });

    // If there is no such PUCCH resource, return \c nullptr.
    if (sr_pucch_resource_cfg == pucch_res_list.end()) {
      // TODO: Add information about the LC which this SR is for.
      return nullptr;
    }

    slot_record.ue_using_sr_resource = crnti;
    return &(*sr_pucch_resource_cfg);
  }
  return nullptr;
};

bool pucch_resource_manager::release_harq_resource(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg)
{
  auto& allocated_ues = get_slot_resource_counter(slot_harq).ues_using_format1_res;
  auto  target_res = std::find_if(allocated_ues.begin(), allocated_ues.end(), [target_rnti = crnti](const rnti_t rnti) {
    return rnti == target_rnti;
  });

  // If the resources was found, then release it (i.e., remove the C-RNTI of the user allocated to it).
  if (target_res != allocated_ues.end()) {
    *target_res = INVALID_RNTI;
    return true;
  }

  return false;
}

bool pucch_resource_manager::release_format2_resource(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg)
{
  auto& allocated_ues = get_slot_resource_counter(slot_harq).ues_using_format2_res;
  auto  target_res = std::find_if(allocated_ues.begin(), allocated_ues.end(), [target_rnti = crnti](const rnti_t rnti) {
    return rnti == target_rnti;
  });

  // If the resources was found, then release it (i.e., remove the C-RNTI of the user allocated to it).
  if (target_res != allocated_ues.end()) {
    *target_res = INVALID_RNTI;
    return true;
  }

  return false;
}

bool pucch_resource_manager::release_sr_resource(slot_point slot_sr, rnti_t crnti)
{
  auto& allocated_ue = get_slot_resource_counter(slot_sr).ue_using_sr_resource;

  // If the UE allocated to the SR PUCCH resource matches the given CRNTI, release the resource.
  if (allocated_ue == crnti) {
    allocated_ue = INVALID_RNTI;
    return true;
  }

  return false;
}

bool pucch_resource_manager::release_csi_resource(slot_point slot_sr, rnti_t crnti)
{
  auto& allocated_ue = get_slot_resource_counter(slot_sr).ue_using_csi_resource;

  // If the UE allocated to the SR PUCCH resource matches the given CRNTI, release the resource.
  if (allocated_ue == crnti) {
    allocated_ue = INVALID_RNTI;
    return true;
  }

  return false;
}

int pucch_resource_manager::fetch_f1_pucch_res_indic(slot_point slot_tx, rnti_t crnti)
{
  const auto& ue_recs = get_slot_resource_counter(slot_tx).ues_using_format1_res;

  auto ue_resource = std::find_if(
      ue_recs.begin(), ue_recs.end(), [target_rnti = crnti](const rnti_t rnti) { return rnti == target_rnti; });

  // -1 indicates that the there is no UE record for given RNTI.
  return ue_resource != ue_recs.end() ? static_cast<int>(ue_resource - ue_recs.begin()) : -1;
}

int pucch_resource_manager::fetch_f2_pucch_res_indic(slot_point slot_tx, rnti_t crnti)
{
  const auto& ue_recs = get_slot_resource_counter(slot_tx).ues_using_format2_res;

  auto ue_resource = std::find_if(
      ue_recs.begin(), ue_recs.end(), [target_rnti = crnti](const rnti_t rnti) { return rnti == target_rnti; });

  // -1 indicates that the there is no UE record for given RNTI.
  return ue_resource != ue_recs.end() ? static_cast<int>(ue_resource - ue_recs.begin()) : -1;
}

const pucch_resource* pucch_resource_manager::fetch_csi_pucch_res_config(slot_point                   slot_tx,
                                                                         rnti_t                       crnti,
                                                                         const ue_cell_configuration& ue_cell_cfg)
{
  srsgnb_sanity_check(slot_tx < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated to far into the future");

  rnti_pucch_res_id_slot_record& slot_record = get_slot_resource_counter(slot_tx);

  if (slot_record.ue_using_csi_resource != crnti) {
    return nullptr;
  }

  const pucch_config& pucch_cfg = ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();

  const int csi_pucch_res_idx = get_pucch_res_idx_for_csi(ue_cell_cfg);
  if (csi_pucch_res_idx < 0) {
    return nullptr;
  }

  // Check if the list of PUCCH resources contains the resource indexed to be used for CSI.
  const auto& pucch_res_list = pucch_cfg.pucch_res_list;
  const auto* csi_pucch_resource_cfg =
      std::find_if(pucch_res_list.begin(), pucch_res_list.end(), [csi_pucch_res_idx](const pucch_resource& pucch_res) {
        return static_cast<unsigned>(csi_pucch_res_idx) == pucch_res.res_id;
      });

  return csi_pucch_resource_cfg != pucch_res_list.end() ? &pucch_cfg.pucch_res_list[csi_pucch_res_idx] : nullptr;
}

pucch_resource_manager::rnti_pucch_res_id_slot_record& pucch_resource_manager::get_slot_resource_counter(slot_point sl)
{
  srsgnb_sanity_check(sl < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH resource ring-buffer accessed too far into the future");
  return resource_slots[sl.to_uint() % RES_MANAGER_RING_BUFFER_SIZE];
}
