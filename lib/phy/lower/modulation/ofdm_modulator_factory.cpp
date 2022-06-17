

#include "ofdm_modulator_factory_impl.h"
#include "srsgnb/srslog/bundled/fmt/core.h"
#include <functional>

using namespace srsgnb;

// Describes an OFDM modulator factory entry.
struct ofdm_modulator_factory_entry {
  std::string                                                                                                type;
  std::function<std::unique_ptr<ofdm_modulator_factory>(const ofdm_modulator_factory_configuration& config)> make;
};

// DFT processor factory entries.
static const std::vector<ofdm_modulator_factory_entry> ofdm_modulator_factory_entries = {
    {"generic",
     [](const ofdm_modulator_factory_configuration& config) {
       ofdm_modulator_common_configuration common_config;
       common_config.dft_factory = config.dft_factory;
       return std::make_unique<ofdm_modulator_factory_impl>(common_config);
     }},
};

std::unique_ptr<ofdm_modulator_factory>
srsgnb::create_ofdm_modulator_factory(const ofdm_modulator_factory_configuration& config)
{
  // Iterate all entries and check if the type matches.
  for (const ofdm_modulator_factory_entry& entry : ofdm_modulator_factory_entries) {
    if (config.type == entry.type) {
      return entry.make(config);
    }
  }

  // Iterate all types and append.
  fmt::print("Invalid OFDM modulator factory type {}. Available types: ", config.type);
  for (unsigned entry_id = 0; entry_id != ofdm_modulator_factory_entries.size(); ++entry_id) {
    if (entry_id > 0) {
      if (entry_id != ofdm_modulator_factory_entries.size() - 1) {
        fmt::print(" and ");
      } else {
        fmt::print(", ");
      }
    }
    fmt::print("{}", ofdm_modulator_factory_entries[entry_id].type);
  }
  fmt::print(".\n");

  return nullptr;
}
