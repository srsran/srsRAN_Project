#include "gnb_appconfig_validators.h"
#include "srsgnb/adt/span.h"

using namespace srsgnb;

/// Validates the given RU application configuration. Returns true on success, otherwise false.
static bool validate_rf_driver_appconfig(const rf_driver_appconfig& config)
{
  return true;
}

/// Validates the given DU application configuration. Returns true on success, otherwise false.
static bool validate_cells_appconfig(span<const cell_appconfig> config)
{
  if (config.size() != 1) {
    return false;
  }

  return true;
}

/// Validates the given CU application configuration. Returns true on success, otherwise false.
static bool validate_cu_appconfig(const cu_cp_appconfig& config)
{
  return true;
}

bool srsgnb::validate_appconfig(const gnb_appconfig& config)
{
  if (!validate_cu_appconfig(config.cu_cfg)) {
    return false;
  }

  if (!validate_cells_appconfig(config.cells_cfg)) {
    return false;
  }

  if (!validate_rf_driver_appconfig(config.rf_driver_cfg)) {
    return false;
  }

  return true;
}
