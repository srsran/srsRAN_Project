#include "gnb_appconfig_profiles.h"
#include "srsgnb/phy/lower/sampling_rate.h"
#include "srsgnb/radio/radio_configuration.h"

using namespace srsgnb;

static rf_driver_appconfig make_uhd_20MHz_n7_rf_driver_appconfig()
{
  rf_driver_appconfig config = {};
  config.srate_MHz           = 23.04F;
  config.device_driver       = "uhd";
  config.device_args         = "type=b200";

  return config;
}

rf_driver_appconfig build_profile_rf_driver_appconfig(const std::string& profile_name)
{
  if (profile_name == "zmq_20MHz_n7") {
    return {};
  }

  if (profile_name == "uhd_20MHz_n7") {
    return make_uhd_20MHz_n7_rf_driver_appconfig();
  }

  if (profile_name == "zmq_20MHz_n41") {
    return {};
  }

  srsgnb_terminate("Invalid profile detected");
}

static std::vector<cell_appconfig> make_zmq_20MHz_n41_cells_appconfig()
{
  std::vector<cell_appconfig> out_cfg = {{}};
  cell_appconfig&             cell    = out_cfg.front();
  cell.dl_arfcn                       = 520000;

  return out_cfg;
}

static std::vector<cell_appconfig> build_profile_cells_appconfig(const std::string& profile_name)
{
  if (profile_name == "zmq_20MHz_n7") {
    return {{}};
  }

  if (profile_name == "uhd_20MHz_n7") {
    return {{}};
  }

  if (profile_name == "zmq_20MHz_n41") {
    return make_zmq_20MHz_n41_cells_appconfig();
  }

  srsgnb_terminate("Invalid profile detected");
}

gnb_appconfig srsgnb::build_profile_gnb_appconfig(const std::string& profile_name)
{
  gnb_appconfig config;
  config.rf_driver_cfg = build_profile_rf_driver_appconfig(profile_name);
  config.cells_cfg     = build_profile_cells_appconfig(profile_name);

  return config;
}
