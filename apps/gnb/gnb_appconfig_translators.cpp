#include "gnb_appconfig_translators.h"
#include "gnb_appconfig.h"

using namespace srsgnb;

srs_cu_cp::cu_cp_configuration srsgnb::generate_cu_cp_config(const gnb_appconfig& config)
{
  srs_cu_cp::cu_cp_configuration out_cfg = config_helpers::make_default_cu_cp_config();
  out_cfg.ngc_config.gnb_id              = config.cu_cfg.gnb_id;
  out_cfg.ngc_config.ran_node_name       = config.cu_cfg.ran_node_name;
  out_cfg.ngc_config.plmn_id             = config.cu_cfg.plmn_id;
  out_cfg.ngc_config.tac                 = config.cu_cfg.tac;

  // :TODO: out config validator should be called here.

  return out_cfg;
}

std::vector<du_cell_config> srsgnb::generate_du_cell_config(const gnb_appconfig& config)
{
  std::vector<du_cell_config> out_cfg;
  out_cfg.reserve(config.cells_cfg.size());

  for (const auto& cell : config.cells_cfg) {
    const rf_cell&             rf = cell.rf;
    cell_config_builder_params param;
    param.pci               = cell.pci;
    param.scs_common        = rf.scs_common;
    param.channel_bw_mhz    = rf.channel_bw_mhz;
    param.nof_crbs          = band_helper::get_n_rbs_from_bw(rf.channel_bw_mhz, rf.scs_common, frequency_range::FR1);
    param.dl_arfcn          = rf.dl_arfcn;
    param.band              = band_helper::get_band_from_dl_arfcn(rf.dl_arfcn);
    param.offset_to_point_a = rf.offset_to_point_a;
    param.coreset0_index    = 6;
    param.k_ssb             = rf.k_ssb;

    out_cfg.push_back(config_helpers::make_default_du_cell_config(param));

    error_type<std::string> error = is_du_cell_config_valid(out_cfg.back());
    if (!error) {
      srsgnb_terminate("Invalid configuration DU cell detected");
    }
  }

  return out_cfg;
}

lower_phy_configuration srsgnb::generate_ru_config(const gnb_appconfig& config)
{
  lower_phy_configuration out_cfg;

  {
    const rf_cell& cell = config.cells_cfg.front().rf;

    out_cfg.scs                        = cell.scs_common;
    out_cfg.cp                         = cyclic_prefix::NORMAL;
    out_cfg.max_processing_delay_slots = 4;
    out_cfg.srate                      = sampling_rate::from_MHz(config.rf_driver_cfg.srate_MHz);
    out_cfg.ta_offset                  = lower_phy_ta_offset::n0;
    out_cfg.time_advance_calibration   = phy_time_unit::from_seconds(-16.0F / out_cfg.srate.to_Hz());
    out_cfg.tx_scale =
        1.0F /
        std::sqrt(NRE * band_helper::get_n_rbs_from_bw(cell.channel_bw_mhz, cell.scs_common, frequency_range::FR1));
    out_cfg.ul_to_dl_subframe_offset         = 1;
    out_cfg.amplitude_config.ceiling_dBFS    = -0.1F;
    out_cfg.amplitude_config.enable_clipping = false;
    out_cfg.amplitude_config.full_scale_lin  = 1.F;
    out_cfg.amplitude_config.input_gain_dB   = -2.5F;
    out_cfg.log_level                        = "info";
  }

  for (unsigned sector_id = 0; sector_id != config.cells_cfg.size(); ++sector_id) {
    lower_phy_sector_description sector_config;
    const rf_cell&               cell = config.cells_cfg[sector_id].rf;
    sector_config.bandwidth_rb =
        band_helper::get_n_rbs_from_bw(cell.channel_bw_mhz, cell.scs_common, frequency_range::FR1);
    sector_config.dl_freq_hz = band_helper::nr_arfcn_to_freq(cell.dl_arfcn);
    sector_config.ul_freq_hz = band_helper::nr_arfcn_to_freq(band_helper::get_ul_arfcn_from_dl_arfcn(cell.dl_arfcn));
    for (unsigned port_id = 0; port_id != config.rf_driver_cfg.nof_ports; ++port_id) {
      lower_phy_sector_port_mapping port_mapping;
      port_mapping.stream_id  = sector_id;
      port_mapping.channel_id = port_id;
      sector_config.port_mapping.push_back(port_mapping);
    }
    out_cfg.sectors.push_back(sector_config);
    out_cfg.nof_channels_per_stream.push_back(config.rf_driver_cfg.nof_ports);
  }

  // :TODO: add a validator for this config.

  return out_cfg;
}

static std::vector<std::string> split_rf_driver_args(const std::string& driver_args)
{
  std::stringstream        ss(driver_args);
  std::vector<std::string> result;

  while (ss.good()) {
    std::string str;
    getline(ss, str, ',');
    if (!str.empty()) {
      result.push_back(str);
    }
  }

  return result;
}

static std::vector<std::string> extract_zmq_ports(const std::string& driver_args, const std::string& port_id)
{
  std::vector<std::string> ports;

  const std::vector<std::string>& splitted_args = split_rf_driver_args(driver_args);
  for (const auto& arg : splitted_args) {
    auto I = arg.find(port_id);

    if (I == std::string::npos) {
      continue;
    }

    I = arg.find("=");
    ports.push_back(arg.substr(++I));
  }

  return ports;
}

radio_configuration::radio srsgnb::generate_radio_config(const gnb_appconfig& config)
{
  radio_configuration::radio out_cfg;

  out_cfg.log_level        = "info";
  out_cfg.sampling_rate_hz = config.rf_driver_cfg.srate_MHz * 1e6;
  out_cfg.args             = config.rf_driver_cfg.device_args;
  out_cfg.otw_format       = radio_configuration::over_the_wire_format::DEFAULT;
  out_cfg.clock.clock      = radio_configuration::clock_sources::source::DEFAULT;
  out_cfg.clock.sync       = radio_configuration::clock_sources::source::DEFAULT;

  // :TODO: Should we expose the clock?
  if (config.rf_driver_cfg.device_driver == "uhd") {
    out_cfg.otw_format  = radio_configuration::over_the_wire_format::SC12;
    out_cfg.clock.clock = radio_configuration::clock_sources::source::INTERNAL;
    out_cfg.clock.sync  = radio_configuration::clock_sources::source::INTERNAL;
  }

  const std::vector<std::string>& zmq_tx_addr = extract_zmq_ports(config.rf_driver_cfg.device_args, "tx_port");
  const std::vector<std::string>& zmq_rx_addr = extract_zmq_ports(config.rf_driver_cfg.device_args, "rx_port");

  for (unsigned sector_id = 0; sector_id != config.cells_cfg.size(); ++sector_id) {
    // For each channel in the streams...
    radio_configuration::stream tx_stream_config;
    radio_configuration::stream rx_stream_config;

    const rf_cell& cell = config.cells_cfg[sector_id].rf;
    for (unsigned port_id = 0; port_id != config.rf_driver_cfg.nof_ports; ++port_id) {
      // Create channel configuration and append it to the previous ones.
      radio_configuration::channel tx_ch_config;
      tx_ch_config.freq.center_frequency_hz = band_helper::nr_arfcn_to_freq(cell.dl_arfcn);
      tx_ch_config.gain_dB                  = config.rf_driver_cfg.tx_gain;
      // Add the tx ports.
      if (config.rf_driver_cfg.device_driver == "zmq") {
        srsgnb_assert(sector_id * config.rf_driver_cfg.nof_ports + port_id < zmq_tx_addr.size(),
                      "Transmission channel arguments out of bounds");
        tx_ch_config.args = zmq_tx_addr[sector_id * config.rf_driver_cfg.nof_ports + port_id];
      }
      tx_stream_config.channels.emplace_back(tx_ch_config);

      radio_configuration::channel rx_ch_config;
      rx_ch_config.freq.center_frequency_hz =
          band_helper::nr_arfcn_to_freq(band_helper::get_ul_arfcn_from_dl_arfcn(cell.dl_arfcn));
      rx_ch_config.gain_dB = config.rf_driver_cfg.rx_gain;
      if (config.rf_driver_cfg.device_driver == "zmq") {
        srsgnb_assert(sector_id * config.rf_driver_cfg.nof_ports + port_id < zmq_rx_addr.size(),
                      "Reception channel arguments out of bounds");
        rx_ch_config.args = zmq_rx_addr[sector_id * config.rf_driver_cfg.nof_ports + port_id];
      }
      rx_stream_config.channels.emplace_back(rx_ch_config);
    }
    out_cfg.tx_streams.emplace_back(tx_stream_config);
    out_cfg.rx_streams.emplace_back(rx_stream_config);
  }

  // :TODO: add a validator for this config.

  return out_cfg;
}

std::vector<upper_phy_config> srsgnb::generate_du_low_config(const gnb_appconfig& config)
{
  std::vector<upper_phy_config> out_cfg;
  out_cfg.reserve(config.cells_cfg.size());

  for (unsigned i = 0, e = config.cells_cfg.size(); i != e; ++i) {
    const cell_appconfig& cell = config.cells_cfg[i];
    upper_phy_config      cfg;

    // :TODO: should we expose this variable for expert configuration?
    static constexpr unsigned dl_pipeline_depth = 4;

    cfg.log_level         = srslog::basic_levels::info;
    cfg.sector_id         = i;
    cfg.nof_ports         = config.rf_driver_cfg.nof_ports;
    cfg.nof_slots_dl_rg   = 2 * dl_pipeline_depth;
    cfg.nof_dl_processors = 2 * dl_pipeline_depth;
    cfg.dl_bw_rb = band_helper::get_n_rbs_from_bw(cell.rf.channel_bw_mhz, cell.rf.scs_common, frequency_range::FR1);
    cfg.ul_bw_rb = band_helper::get_n_rbs_from_bw(cell.rf.channel_bw_mhz, cell.rf.scs_common, frequency_range::FR1);
    cfg.nof_ul_processors                      = 8 * dl_pipeline_depth;
    cfg.softbuffer_config.max_softbuffers      = 4 * dl_pipeline_depth;
    cfg.softbuffer_config.max_nof_codeblocks   = 128;
    cfg.softbuffer_config.max_codeblock_size   = (1U << 18);
    cfg.softbuffer_config.expire_timeout_slots = 100 * get_nof_slots_per_subframe(cell.rf.scs_common);

    out_cfg.push_back(cfg);
  }

  return out_cfg;
}
