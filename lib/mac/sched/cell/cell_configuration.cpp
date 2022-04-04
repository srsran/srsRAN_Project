
#include "cell_configuration.h"
#include "srsgnb/asn1/rrc_nr/tdd_cfg_helper.h"

using namespace srsgnb;

cell_configuration::cell_configuration(const cell_configuration_request_message& msg) :
  cell_index(msg.cell_index),
  pci(msg.pci),
  nof_dl_prbs(52),
  nof_ul_prbs(52),
  nof_slots_per_frame(get_nof_slots_per_subframe(msg.dl_cfg_common.init_dl_bwp.generic_params.subcarrier_spacing)),
  dl_cfg_common(msg.dl_cfg_common),
  ul_cfg_common(msg.ul_cfg_common),
  tdd_cfg_common(msg.tdd_ul_dl_cfg_common)
{
  using namespace tdd_cfg_helper;

  if (tdd_cfg_common.has_value()) {
    // Cache list of DL and UL slots in case of TDD
    uint8_t numerology_idx = msg.tdd_ul_dl_cfg_common->ref_subcarrier_spacing.value;
    dl_enabled_slot_lst.resize(nof_slots_per_period(*msg.tdd_ul_dl_cfg_common));
    ul_enabled_slot_lst.resize(nof_slots_per_period(*msg.tdd_ul_dl_cfg_common));
    for (unsigned i = 0; i < dl_enabled_slot_lst.size(); ++i) {
      dl_enabled_slot_lst[i] = (uint8_t)slot_is_dl(*msg.tdd_ul_dl_cfg_common, slot_point{numerology_idx, i});
      ul_enabled_slot_lst[i] = (uint8_t)slot_is_ul(*msg.tdd_ul_dl_cfg_common, slot_point{numerology_idx, i});
    }
  }
}

#define CHECK(cond, ...)                                                                                               \
  if (not(cond))                                                                                                       \
    return {fmt::format(__VA_ARGS__)};

error_type<std::string> srsgnb::is_cell_configuration_request_valid(const cell_configuration_request_message& msg)
{
  CHECK(msg.cell_index < MAX_NOF_CELLS, "Invalid cell index={}", msg.cell_index);
  CHECK(msg.nof_beams <= 64, "Invalid number of beams={}", msg.nof_beams);
  CHECK(msg.pci <= MAX_PCI, "Invalid pci={}", msg.pci);
  CHECK(msg.dl_cfg_common.init_dl_bwp.pdsch_cfg_common_present, "PDSCHConfigCommon must be present");
  CHECK(msg.dl_cfg_common.init_dl_bwp.pdcch_cfg_common_present, "PDCCHConfigCommon must be present");
  // TODO
  return {};
}
