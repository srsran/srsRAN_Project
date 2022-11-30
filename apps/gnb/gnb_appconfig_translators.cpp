#include "gnb_appconfig_translators.h"
#include "gnb_appconfig.h"
#include "srsgnb/scheduler/config/scheduler_expert_config_validator.h"

using namespace srsgnb;

/// Static configuration that the gnb supports.
static constexpr subcarrier_spacing scs       = subcarrier_spacing::kHz15;
static constexpr cyclic_prefix      cp        = cyclic_prefix::NORMAL;
static constexpr unsigned           nof_ports = 1U;
static constexpr double             tx_gain   = 60.0;
static constexpr double             rx_gain   = 70.0;

srs_cu_cp::cu_cp_configuration srsgnb::generate_cu_cp_config(const gnb_appconfig& config)
{
  srs_cu_cp::cu_cp_configuration out_cfg = config_helpers::make_default_cu_cp_config();
  out_cfg.ngc_config.gnb_id              = config.gnb_id;
  out_cfg.ngc_config.ran_node_name       = config.ran_node_name;
  out_cfg.ngc_config.plmn                = config.common_cell_cfg.plmn;
  out_cfg.ngc_config.tac                 = config.common_cell_cfg.tac;

  if (!config_helpers::is_valid_configuration(out_cfg)) {
    srsgnb_terminate("Invalid CU-CP configuration. Exiting.\n");
  }

  return out_cfg;
}

std::vector<du_cell_config> srsgnb::generate_du_cell_config(const gnb_appconfig& config)
{
  std::vector<du_cell_config> out_cfg;
  out_cfg.reserve(config.cells_cfg.size());

  unsigned cell_id = 0;
  for (const auto& cell : config.cells_cfg) {
    cell_config_builder_params param;
    const base_cell_appconfig& base_cell = cell.cell;
    param.pci                            = cell.pci;
    param.scs_common                     = scs;
    param.channel_bw_mhz                 = base_cell.channel_bw_mhz;
    param.nof_crbs = band_helper::get_n_rbs_from_bw(base_cell.channel_bw_mhz, param.scs_common, frequency_range::FR1);
    param.dl_arfcn = base_cell.dl_arfcn;
    param.band     = band_helper::get_band_from_dl_arfcn(base_cell.dl_arfcn);

    static const uint8_t                              ss0_idx      = 0; // TODO: Parameterize.
    optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc = band_helper::get_ssb_coreset0_freq_location(
        base_cell.dl_arfcn, base_cell.band, param.nof_crbs, scs, scs, ss0_idx);

    if (!ssb_freq_loc.has_value()) {
      srsgnb_terminate("Unable to derive a valid SSB pointA and k_SSB for cell id ({}). Exiting.", cell.pci);
    }

    srslog::basic_logger& logger = srslog::fetch_basic_logger("gNB appconfig translator", false);
    logger.set_level(srslog::basic_levels::info);

    logger.info("SSB derived parameters for cell: {}, dl_arfcn:{}, crbs: {} scs:{}, ssb_scs:{}:\n\t - SSB offset "
                "pointA:{} \n\t - k_SSB:{} \n\t - SSB arfcn:{} \n\t - Coreset index:{} \n\t - Searchspace index:{}",
                cell.pci,
                base_cell.dl_arfcn,
                param.nof_crbs,
                scs,
                scs,
                (*ssb_freq_loc).offset_to_point_A.to_uint(),
                (*ssb_freq_loc).k_ssb.to_uint(),
                (*ssb_freq_loc).ssb_arfcn,
                (*ssb_freq_loc).coreset0_idx,
                (*ssb_freq_loc).searchspace0_idx);


    param.offset_to_point_a = (*ssb_freq_loc).offset_to_point_A;
    param.k_ssb             = (*ssb_freq_loc).k_ssb;
    param.coreset0_index    = (*ssb_freq_loc).coreset0_idx;

    // Create the configuration.
    out_cfg.push_back(config_helpers::make_default_du_cell_config(param));

    // Set the rest of the parameters.
    du_cell_config& out_cell = out_cfg.back();
    out_cell.plmn            = base_cell.plmn;
    out_cell.tac             = base_cell.tac;
    out_cell.cell_id         = cell_id;

    // Carrier config.
    out_cell.dl_carrier.nof_ant = base_cell.nof_antennas_dl;
    out_cell.ul_carrier.nof_ant = base_cell.nof_antennas_ul;

    // PRACH config.
    rach_config_common& rach_cfg                           = *out_cell.ul_cfg_common.init_ul_bwp.rach_cfg_common;
    rach_cfg.rach_cfg_generic.prach_config_index           = base_cell.prach_cfg.prach_config_index;
    rach_cfg.prach_root_seq_index                          = base_cell.prach_cfg.prach_root_sequence_index;
    rach_cfg.rach_cfg_generic.zero_correlation_zone_config = base_cell.prach_cfg.zero_correlation_zone;

    error_type<std::string> error = is_du_cell_config_valid(out_cfg.back());
    if (!error) {
      srsgnb_terminate("Invalid configuration DU cell detected.");
    }
    ++cell_id;
  }

  return out_cfg;
}

lower_phy_configuration srsgnb::generate_ru_config(const gnb_appconfig& config)
{
  lower_phy_configuration out_cfg;

  {
    const base_cell_appconfig& cell = config.cells_cfg.front().cell;

    out_cfg.log_level                  = config.log_level;
    out_cfg.scs                        = scs;
    out_cfg.cp                         = cp;
    out_cfg.max_processing_delay_slots = 4;
    out_cfg.ul_to_dl_subframe_offset   = 1;

    out_cfg.srate = sampling_rate::from_MHz(config.rf_driver_cfg.srate_MHz);

    out_cfg.ta_offset                = lower_phy_ta_offset::n0;
    out_cfg.time_advance_calibration = phy_time_unit::from_seconds(0.0);
    // NOTE: ZMQ has a delay of 16 samples, so the time advance calibration is adjusted.
    if (config.rf_driver_cfg.device_driver == "zmq") {
      out_cfg.time_advance_calibration = phy_time_unit::from_seconds(-16.0 / out_cfg.srate.to_Hz());
    }

    out_cfg.tx_scale =
        1.0F / std::sqrt(NRE * band_helper::get_n_rbs_from_bw(cell.channel_bw_mhz, scs, frequency_range::FR1));
    out_cfg.amplitude_config.ceiling_dBFS    = -0.1F;
    out_cfg.amplitude_config.enable_clipping = false;
    out_cfg.amplitude_config.full_scale_lin  = 1.F;
    out_cfg.amplitude_config.input_gain_dB   = -2.5F;
  }

  for (unsigned sector_id = 0; sector_id != config.cells_cfg.size(); ++sector_id) {
    lower_phy_sector_description sector_config;
    const base_cell_appconfig&   cell = config.cells_cfg[sector_id].cell;
    sector_config.bandwidth_rb        = band_helper::get_n_rbs_from_bw(cell.channel_bw_mhz, scs, frequency_range::FR1);
    sector_config.dl_freq_hz          = band_helper::nr_arfcn_to_freq(cell.dl_arfcn);
    sector_config.ul_freq_hz = band_helper::nr_arfcn_to_freq(band_helper::get_ul_arfcn_from_dl_arfcn(cell.dl_arfcn));
    for (unsigned port_id = 0; port_id != nof_ports; ++port_id) {
      lower_phy_sector_port_mapping port_mapping;
      port_mapping.stream_id  = sector_id;
      port_mapping.channel_id = port_id;
      sector_config.port_mapping.push_back(port_mapping);
    }
    out_cfg.sectors.push_back(sector_config);
    out_cfg.nof_channels_per_stream.push_back(nof_ports);
  }

  if (!is_valid_lower_phy_config(out_cfg)) {
    srsgnb_terminate("Invalid lower PHY configuration. Exiting.");
  }

  return out_cfg;
}

/// Slice the given string by the ',' limiter, and returns a vector with each position containing one slice of the
/// string.
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

/// Finds the ZMQ ports within the given driver arguments. Returns a vector that contains with the ZMQ transmission or
/// reception ports.
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

  out_cfg.log_level        = config.log_level;
  out_cfg.sampling_rate_hz = config.rf_driver_cfg.srate_MHz * 1e6;
  out_cfg.args             = config.rf_driver_cfg.device_args;
  out_cfg.otw_format       = radio_configuration::over_the_wire_format::DEFAULT;
  out_cfg.clock.clock      = radio_configuration::clock_sources::source::DEFAULT;
  out_cfg.clock.sync       = radio_configuration::clock_sources::source::DEFAULT;

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

    const base_cell_appconfig& cell = config.cells_cfg[sector_id].cell;
    for (unsigned port_id = 0; port_id != nof_ports; ++port_id) {
      // Create channel configuration and append it to the previous ones.
      radio_configuration::channel tx_ch_config;
      tx_ch_config.freq.center_frequency_hz = band_helper::nr_arfcn_to_freq(cell.dl_arfcn);

      tx_ch_config.gain_dB = tx_gain;
      // Add the tx ports.
      if (config.rf_driver_cfg.device_driver == "zmq") {
        srsgnb_assert(sector_id * nof_ports + port_id < zmq_tx_addr.size(),
                      "Transmission channel arguments out of bounds");
        tx_ch_config.args = zmq_tx_addr[sector_id * nof_ports + port_id];
      }
      tx_stream_config.channels.emplace_back(tx_ch_config);

      radio_configuration::channel rx_ch_config;
      rx_ch_config.freq.center_frequency_hz =
          band_helper::nr_arfcn_to_freq(band_helper::get_ul_arfcn_from_dl_arfcn(cell.dl_arfcn));
      rx_ch_config.gain_dB = rx_gain;
      if (config.rf_driver_cfg.device_driver == "zmq") {
        srsgnb_assert(sector_id * nof_ports + port_id < zmq_rx_addr.size(),
                      "Reception channel arguments out of bounds");
        rx_ch_config.args = zmq_rx_addr[sector_id * nof_ports + port_id];
      }
      rx_stream_config.channels.emplace_back(rx_ch_config);
    }
    out_cfg.tx_streams.emplace_back(tx_stream_config);
    out_cfg.rx_streams.emplace_back(rx_stream_config);
  }

  if (!radio_configuration::is_valid_radio_config(out_cfg)) {
    srsgnb_terminate("Invalid radio configuration. Exiting.");
  }

  return out_cfg;
}

/// Returns the log level from the given string. Only supports info and debug levels.
static srslog::basic_levels to_srs_log_level(const std::string& level)
{
  if (level == "debug") {
    return srslog::basic_levels::debug;
  }

  return srslog::basic_levels::info;
}

std::vector<upper_phy_config> srsgnb::generate_du_low_config(const gnb_appconfig& config)
{
  std::vector<upper_phy_config> out_cfg;
  out_cfg.reserve(config.cells_cfg.size());

  for (unsigned i = 0, e = config.cells_cfg.size(); i != e; ++i) {
    const base_cell_appconfig& cell = config.cells_cfg[i].cell;
    upper_phy_config           cfg;

    static constexpr unsigned dl_pipeline_depth = 4;

    cfg.log_level = to_srs_log_level(config.log_level);
    cfg.sector_id = i;
    cfg.nof_ports = nof_ports;

    cfg.nof_slots_dl_rg   = 2 * dl_pipeline_depth;
    cfg.nof_dl_processors = 2 * dl_pipeline_depth;
    cfg.nof_ul_processors = 8 * dl_pipeline_depth;

    cfg.dl_bw_rb = band_helper::get_n_rbs_from_bw(cell.channel_bw_mhz, scs, frequency_range::FR1);
    cfg.ul_bw_rb = band_helper::get_n_rbs_from_bw(cell.channel_bw_mhz, scs, frequency_range::FR1);

    cfg.softbuffer_config.max_softbuffers      = 4 * dl_pipeline_depth;
    cfg.softbuffer_config.max_nof_codeblocks   = 128;
    cfg.softbuffer_config.max_codeblock_size   = (1U << 18);
    cfg.softbuffer_config.expire_timeout_slots = 100 * get_nof_slots_per_subframe(scs);

    if (!is_valid_upper_phy_config(cfg)) {
      srsgnb_terminate("Invalid upper PHY configuration. Exiting.");
    }

    out_cfg.push_back(cfg);
  }

  return out_cfg;
}

scheduler_expert_config srsgnb::generate_scheduler_expert_config(const gnb_appconfig& config)
{
  scheduler_expert_config out_cfg = config_helpers::make_default_scheduler_expert_config();

  // UE parameters.
  const pdsch_appconfig& pdsch = config.common_cell_cfg.pdsch_cfg;
  const pusch_appconfig& pusch = config.common_cell_cfg.pusch_cfg;
  out_cfg.ue.fixed_dl_mcs      = pdsch.fixed_ue_mcs;
  out_cfg.ue.fixed_ul_mcs      = pusch.fixed_ue_mcs;

  // RA parameters.
  const prach_appconfig& prach       = config.common_cell_cfg.prach_cfg;
  out_cfg.ra.rar_mcs_index           = pdsch.fixed_rar_mcs;
  out_cfg.ra.max_nof_msg3_harq_retxs = prach.max_msg3_harq_retx;
  out_cfg.ra.msg3_mcs_index          = prach.fixed_msg3_mcs;

  // SI parameters.
  const pdcch_appconfig& pdcch = config.common_cell_cfg.pdcch_cfg;
  out_cfg.si.sib1_mcs_index    = pdsch.fixed_si_mcs;
  out_cfg.si.sib1_dci_aggr_lev = pdcch.si_aggregation_level_index;

  error_type<std::string> error = is_scheduler_expert_config_valid(out_cfg);
  if (!error) {
    srsgnb_terminate("Invalid scheduler expert configuration detected. Exiting. ");
  }

  return out_cfg;
}
