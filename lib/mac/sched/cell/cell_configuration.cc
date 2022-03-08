
#include "cell_configuration.h"

using namespace srsgnb;

cell_configuration::cell_configuration(const cell_configuration_request_message& msg) :
  cell_index(msg.cell_index), pci(msg.pci), duplex_mode(msg.duplex_mode), nof_dl_prbs(52), nof_ul_prbs(52)
{}

#define CHECK(cond, fmtstr, ...)                                                                                       \
  if (not(cond))                                                                                                       \
    return {fmt::format(fmtstr, __VA_ARGS__)};

error_type<std::string> srsgnb::is_cell_configuration_request_valid(const cell_configuration_request_message& msg)
{
  CHECK(msg.cell_index < MAX_NOF_CELLS, "Invalid cell index={}", msg.cell_index);
  CHECK(msg.nof_beams <= 64, "Invalid number of beams={}", msg.nof_beams);
  CHECK(msg.pci <= MAX_PCI, "Invalid pci={}", msg.pci);
  // TODO
  return {};
}
