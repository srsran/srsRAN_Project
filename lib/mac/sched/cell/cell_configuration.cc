
#include "cell_configuration.h"

using namespace srsgnb;

cell_configuration::cell_configuration(const cell_configuration_request_message& msg) :
  is_tdd(msg.tdd_ul_dl_cfg_common.has_value()),
  cell_index(msg.cell_index),
  pci(msg.pci),
  nof_dl_prbs(52),
  nof_ul_prbs(52),
  dl_cfg_common(msg.dl_cfg_common),
  ul_cfg_common(msg.ul_cfg_common),
  tdd_cfg_common(msg.tdd_ul_dl_cfg_common)
{}

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
