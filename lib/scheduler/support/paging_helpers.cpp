#include "paging_helpers.h"
#include "pdcch/pdcch_type0_helpers.h"
#include "ssb_helpers.h"

using namespace srsran;

paging_slot_helper::paging_slot_helper(const cell_configuration&                       cell_cfg_,
                                       const sched_cell_configuration_request_message& msg) :
  cell_cfg(cell_cfg_)
{
  if (cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.has_value()) {
    for (const auto& cfg : cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces) {
      if (cfg.get_id() != cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.value()) {
        continue;
      }
      ss_cfg = &cfg;
      break;
    }

    if (ss_cfg == nullptr) {
      srsran_assertion_failure("Paging Search Space not configured in DL BWP.");
    }

    if (cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.value() == 0) {
      // PDCCH monitoring occasions for paging are same as for RMSI. See TS 38.304, clause 7.1.
      // NOTE: We currently support only SS/PBCH and CORESET multiplexing patter 1.
      if (cell_cfg.dl_cfg_common.pcch_cfg.nof_pf == pcch_config::nof_pf_per_drx_cycle::oneT) {
        srsran_assertion_failure(
            "Invalid nof. Paging frames per DRX Cycle for SS/PBCH and CORESET multiplexing patter 1.");
      }
      // As per TS 38.304, clause 7.1, For Ns = 2, PO is either in the first half frame (i_s = 0) or the second half
      // frame (i_s = 1) of the PF.  This is possible only when using ssb periodicity of 5ms which is in turn possible
      // only when using SS/PBCH and CORESET multiplexing patter 2 or 3.
      srsran_assert(
          cell_cfg.dl_cfg_common.pcch_cfg.ns == pcch_config::nof_po_per_pf::one,
          "Number of Paging Occasions per Paging Frame must be 1 for SS/PBCH and CORESET multiplexing patter 1.");
      for (size_t i_ssb = 0; i_ssb < MAX_NUM_BEAMS; i_ssb++) {
        if (not is_nth_ssb_beam_active(cell_cfg.ssb_cfg.ssb_bitmap, i_ssb)) {
          continue;
        }
        // For Ns = 1, there is only one PO which starts from the first PDCCH monitoring occasion for paging in the PF.
        // TS 38.304, clause 7.1. Hence, n0 slot must be use and not n0 + 1 slot.
        type0_pdcch_css_slots[i_ssb] =
            precompute_type0_pdcch_css_n0(msg.searchspace0, msg.coreset0, cell_cfg, msg.scs_common, i_ssb);
      }
    } else {
      if (ss_cfg->get_coreset_id() != to_coreset_id(0) and
          ((not cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.has_value()) or
           (cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value().id != ss_cfg->get_coreset_id()))) {
        srsran_assertion_failure("CORESET configuration for Paging Search Space not configured in DL BWP.");
      }
      if (ss_cfg->get_coreset_id() == to_coreset_id(0) and
          (not cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.has_value())) {
        srsran_assertion_failure("CORESET0 configuration for Paging Search Space not configured in DL BWP.");
      }
      precompute_type2_pdcch_slots(msg.scs_common);
    }
  } else {
    srsran_assertion_failure("Paging Search Space not configured in DL BWP.");
  }
}

bool paging_slot_helper::is_paging_slot(slot_point pdcch_slot, unsigned i_s) const
{
  const auto paging_search_space = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.value();

  if (paging_search_space > 0) {
    return is_paging_slot_in_search_space_id_gt_0(pdcch_slot, i_s);
  }
  return is_paging_slot_in_search_space0(pdcch_slot, i_s);
}

bool paging_slot_helper::is_paging_slot_in_search_space_id_gt_0(slot_point pdcch_slot, unsigned i_s) const
{
  // Following values for Ns are valid as per TS 38.304, clause 7.1 and TS 38.331, clause 5.2.1 when
  // pagingSearchSpace > 0
  // - Ns can be 1, 2 or 4 provided SearchSpace has sufficient PDCCH Monitoring Occasions to match nof. PO(s).

  // - [Implementation defined] Currently, PDCCH allocator does not support more than one monitoring occasion per slot
  //   and the monitoring occasion should start at the first symbol of the slot. Therefore, DU must configure paging
  //   search space taking this into account when defining SearchSpace > 0 for paging.

  // - [Implementation defined] PDCCH Monitoring Occasions (PMO) are pre-computed over max of nof. slots in radio frame
  // and nof. slots in SearchSpace periodicity. In order to take into account PMOs in atleast Paging Frame.
  const auto periodicity_in_slots =
      std::max(ss_cfg->get_monitoring_slot_periodicity(), pdcch_slot.nof_slots_per_frame());

  // For each beam, check if the paging needs to be allocated in this slot.
  for (unsigned ssb_idx = 0; ssb_idx < MAX_NUM_BEAMS; ssb_idx++) {
    // Do not schedule the paging for the SSB indices that are not used.
    if (not is_nth_ssb_beam_active(cell_cfg.ssb_cfg.ssb_bitmap, ssb_idx)) {
      continue;
    }

    // TODO: Support multi-beam operations. As per TS 38.304, clause 7.1, In multi-beam operations, the same paging
    //  message are repeated in all transmitted beams.

    if (pdcch_slot.to_uint() % periodicity_in_slots == type2_pdcch_css_slots[ssb_idx][i_s].to_uint()) {
      // All conditions are satisfied.
      return true;
    }
  }

  return false;
}

bool paging_slot_helper::is_paging_slot_in_search_space0(slot_point pdcch_slot, unsigned i_s) const
{
  // Following values for Ns are valid as per TS 38.304, clause 7.1 and TS 38.331, clause 5.2.1 when
  // pagingSearchSpace = 0
  // - Ns can be 1 when multiplexing pattern 1.
  // - Ns can be 2 when multiplexing pattern is 2 or 3 and SS/PBCH periodicity is 5ms

  // TS 38.304, clause 7.1, When SearchSpaceId = 0 is configured for pagingSearchSpace and Ns = 2, PO is either in the
  // first half frame (i_s = 0) or the second half frame (i_s = 1) of the PF.
  if ((i_s == 0 and pdcch_slot.is_odd_hrf()) or (i_s == 1 and (not pdcch_slot.is_odd_hrf()))) {
    return false;
  }

  // SearchSpace#0 periodicity is always 20 ms when using multiplexing pattern 1. See TS 38.213, clause 13.
  // TODO: Populate SS#0 periodicity based on config when other multiplexing patterns are supported.
  static const unsigned ss0_periodicity_in_ms = 20;

  // The paging_period_slots is expressed in unit of slots.
  // NOTE: As paging_period_slots is expressed in milliseconds or subframes, we need to convert them into slots.
  const unsigned paging_period_slots =
      ss0_periodicity_in_ms * static_cast<unsigned>(pdcch_slot.nof_slots_per_subframe());

  // For each beam, check if the paging needs to be allocated in this slot.
  for (unsigned ssb_idx = 0; ssb_idx < MAX_NUM_BEAMS; ssb_idx++) {
    // Do not schedule the paging for the SSB indices that are not used.
    if (not is_nth_ssb_beam_active(cell_cfg.ssb_cfg.ssb_bitmap, ssb_idx)) {
      continue;
    }

    // TODO: Support multi-beam operations. As per TS 38.304, clause 7.1, In multi-beam operations, the same paging
    //  message are repeated in all transmitted beams.

    if (pdcch_slot.to_uint() % paging_period_slots == type0_pdcch_css_slots[ssb_idx].to_uint()) {
      // All conditions are satisfied.
      return true;
    }
  }

  return false;
}

void paging_slot_helper::precompute_type2_pdcch_slots(subcarrier_spacing scs_common)
{
  const auto& ss_periodicity = ss_cfg->get_monitoring_slot_periodicity();
  const auto& ss_slot_offset = ss_cfg->get_monitoring_slot_offset();
  const auto& ss_duration    = ss_cfg->get_duration();

  std::vector<slot_point> pdcch_monitoring_occasions;
  // Initialize slot point to 0.
  slot_point sl = slot_point{to_numerology_value(scs_common), 0};
  // - [Implementation defined] Compute all PDCCH Monitoring Occasions (PMO) over max of nof. slots in radio frame
  // and nof. slots in SearchSpace periodicity. In order to take into account PMOs in atleast Paging Frame.
  const auto periodicity_in_slots = std::max(ss_periodicity, sl.nof_slots_per_frame());
  for (unsigned slot_num = 0; slot_num < periodicity_in_slots; slot_num += ss_duration) {
    const slot_point ref_sl = sl + slot_num;
    if ((slot_num - ss_slot_offset) % ss_periodicity == 0) {
      // - [Implementation defined] Currently, PDCCH allocator does not support more than one monitoring occasion
      //   per slot.
      for (unsigned duration = 0; duration < ss_duration; duration++) {
        const slot_point add_pmo_slot = ref_sl + duration;
        // Ensure slot for Paging has DL enabled.
        if (not cell_cfg.is_dl_enabled(add_pmo_slot)) {
          continue;
        }
        pdcch_monitoring_occasions.push_back(add_pmo_slot);
      }
    }
  }

  unsigned nof_ssb_transmitted{};
  for (size_t i_ssb = 0; i_ssb < MAX_NUM_BEAMS; i_ssb++) {
    if (not is_nth_ssb_beam_active(cell_cfg.ssb_cfg.ssb_bitmap, i_ssb)) {
      continue;
    }
    nof_ssb_transmitted++;
  }

  // NOTE: For active BWP not equal to Initial DL BWP, the value of firstPDCCH-MonitoringOccasionOfPO must be taken from
  // PDCCH-ConfigCommon IE of the active BWP.
  const auto& first_pmo_of_po = cell_cfg.dl_cfg_common.pcch_cfg.first_pdcch_monitoring_occasion_of_po_value;
  const auto  nof_po_per_pf   = static_cast<unsigned>(cell_cfg.dl_cfg_common.pcch_cfg.ns);
  for (unsigned po_idx = 0; po_idx < nof_po_per_pf; po_idx++) {
    for (size_t i_ssb = 0; i_ssb < MAX_NUM_BEAMS; i_ssb++) {
      if (not is_nth_ssb_beam_active(cell_cfg.ssb_cfg.ssb_bitmap, i_ssb)) {
        continue;
      }
      // See TS 38.304, clause 7.1.
      const auto pmo_idx =
          first_pmo_of_po.empty()
              ? (po_idx * nof_ssb_transmitted * pcch_config::NR_OF_PDCCH_MONITORING_OCCASION_PER_SSB_IN_PO)
              : first_pmo_of_po[po_idx];
      srsran_assert(pmo_idx < pdcch_monitoring_occasions.size(),
                    "Not enough PDCCH Monitoring Occasions for Paging Occasion");
      type2_pdcch_css_slots[i_ssb].push_back(pdcch_monitoring_occasions[pmo_idx]);
    }
  }
}
