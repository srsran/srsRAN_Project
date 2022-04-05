#include "srsgnb/radio/radio_factory.h"
#include "uhd/radio_uhd_impl.h"

using namespace srsgnb;

std::unique_ptr<radio_factory> srsgnb::create_radio_factory(std::string driver_name)
{
  // Convert driver name to lower case.
  for (char& c : driver_name) {
    c = std::tolower(c);
  }

  // Check for UHD.
#ifdef ENABLE_UHD
  if (driver_name == "uhd") {
    return std::make_unique<radio_factory_uhd_impl>();
  }
#endif // ENABLE_UHD

  return nullptr;
}
