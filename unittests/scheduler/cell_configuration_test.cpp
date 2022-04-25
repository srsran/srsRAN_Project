
#include "config_generators.h"
#include "lib/scheduler/cell/cell_configuration.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

void test_cell_config_request()
{
  cell_configuration_request_message default_msg = make_cell_cfg_req();
  cell_configuration_request_message msg         = default_msg;

  // Test default config
  TESTASSERT(is_cell_configuration_request_valid(msg).has_value());

  // Test invalid cell_index
  msg            = default_msg;
  msg.cell_index = MAX_NOF_CELLS;
  auto err       = is_cell_configuration_request_valid(msg);
  TESTASSERT(err.is_error());
  fmt::print("Error: {}\n", err.error());

  // Test invalid pci
  msg     = default_msg;
  msg.pci = MAX_PCI;
  TESTASSERT(is_cell_configuration_request_valid(msg).has_value());
  msg.pci = MAX_PCI + 1;
  err     = is_cell_configuration_request_valid(msg);
  TESTASSERT(is_cell_configuration_request_valid(msg).is_error());
  fmt::print("Error: {}\n", err.error());
}

int main()
{
  test_cell_config_request();
}