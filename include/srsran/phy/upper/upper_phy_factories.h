/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/phy/metrics/phy_metrics_notifiers.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/pdsch/factories.h"
#include "srsran/phy/upper/channel_processors/pusch/factories.h"
#include "srsran/phy/upper/downlink_processor.h"
#include "srsran/phy/upper/phy_tap/phy_tap.h"
#include "srsran/phy/upper/rx_buffer_pool.h"
#include "srsran/phy/upper/uplink_processor.h"
#include "srsran/phy/upper/upper_phy.h"
#include "srsran/phy/upper/upper_phy_execution_configuration.h"
#include <memory>
#include <variant>

namespace srsran {

class task_executor;
class upper_phy_rg_gateway;
class upper_phy_rx_symbol_request_notifier;

/// Configuration parameters for uplink processors.
struct uplink_processor_config {
  /// Uplink processor result notifier.
  upper_phy_rx_results_notifier& notifier;
  /// Rate Matching receive buffer pool.
  rx_buffer_pool& rm_buffer_pool;
  /// Number of receive ports.
  unsigned nof_rx_ports;
  /// Resource grid bandwidth in resource blocks.
  unsigned nof_rb;
  /// PUSCH allocation maximum number of layers.
  unsigned max_nof_layers;
};

/// Uplink processor factory.
class uplink_processor_factory
{
public:
  /// Default destructor.
  virtual ~uplink_processor_factory() = default;

  /// \brief Creates an uplink processor with the given configuration.
  ///
  /// \param[in] config Contains the configuration to create the uplink processor.
  /// \return A unique pointer to the new uplink processor if successful or \c nullptr otherwise.
  virtual std::unique_ptr<uplink_processor> create(const uplink_processor_config& config) = 0;

  /// \brief Creates an uplink processor with the given configuration with logging capabilities.
  ///
  /// \param[in] config                Contains the configuration to create the uplink processor.
  /// \param[in] logger                Logger instance for logging.
  /// \param[in] log_all_opportunities Set to true for logging all PRACH opportunities. Otherwise, it logs only the
  ///                                  detected preambles.
  /// \return A unique pointer to the new uplink processor if successful or \c nullptr otherwise.
  virtual std::unique_ptr<uplink_processor>
  create(const uplink_processor_config& config, srslog::basic_logger& logger, bool log_all_opportunities) = 0;

  /// \brief Creates an uplink PDU validator.
  /// \return A unique pointer to the new uplink PDU validator if successful or \c nullptr otherwise.
  virtual std::unique_ptr<uplink_pdu_validator> create_pdu_validator() = 0;
};

/// Describes all uplink processors in a pool.
struct uplink_processor_pool_config {
  /// Set of uplink processors for a given numerology.
  struct uplink_processor_set {
    /// Subcarrier spacing.
    subcarrier_spacing scs;
    /// Pointers to the actual uplink processors.
    std::vector<std::unique_ptr<uplink_processor>> procs;
  };

  /// Collection of all uplink processors, organized by numerology.
  std::vector<uplink_processor_set> ul_processors;
  /// Default uplink processor used for handling PRACH and receive symbols when no processor has been assigned.
  std::unique_ptr<uplink_processor> default_processor;
};

/// \brief Creates and returns an uplink processor pool.
std::unique_ptr<uplink_processor_pool> create_uplink_processor_pool(uplink_processor_pool_config config);

/// Configuration parameters for downlink processors.
struct downlink_processor_config {
  /// Radio sector identifier.
  unsigned id;
  /// Resource grid gateway.
  upper_phy_rg_gateway* gateway;
};

/// Factory that allows to create downlink processors.
class downlink_processor_factory
{
public:
  virtual ~downlink_processor_factory() = default;

  /// \brief Creates a downlink processor.
  virtual std::unique_ptr<downlink_processor_base> create(const downlink_processor_config& config) = 0;

  /// \brief Creates a downlink processor with logging capabilities.
  virtual std::unique_ptr<downlink_processor_base>
  create(const downlink_processor_config& config, srslog::basic_logger& logger, bool enable_broadcast) = 0;

  /// \brief Creates a downlink PDU validator.
  /// \return A unique pointer to the new downlink PDU validator if successful or \c nullptr otherwise.
  virtual std::unique_ptr<downlink_pdu_validator> create_pdu_validator() = 0;
};

/// Generic PDSCH processor configuration parameters.
struct pdsch_processor_generic_configuration {
  // No parameter.
};

/// \brief Flexible PDSCH processor configuration parameters.
///
/// Collects the necessary parameters for creating a flexible PDSCH processor. See \ref pdsch_processor_flexible_impl
/// for implementation details.
struct pdsch_processor_flexible_configuration {
  /// CB batch length for guaranteeing a single batch in synchronous operation mode.
  static constexpr unsigned synchronous_cb_batch_length = std::numeric_limits<unsigned>::max();
  /// \brief Length of the codeblock-batch per thread.
  ///
  /// Set to zero if not initialized, which splits the codeblocks homogeneously amongst all threads. Set to
  /// \ref synchronous_cb_batch_length for guaranteeing a memory-optimized synchronous operation.
  unsigned cb_batch_length = 0;
};

/// \brief Downlink processor software factory configuration.
///
/// \remark Default values are empty strings which are invalid.
struct downlink_processor_factory_sw_config {
  /// \brief LDPC encoder type.
  ///
  /// Use of these options:
  /// - \c auto: let the factory select the most efficient given the CPU architecture, or
  /// - \c generic: for using unoptimized LDPC encoder, or
  /// - \c avx2: for using AVX2 optimized LDPC encoder (x86_64 CPUs only), or
  /// - \c neon: for using NEON optimized LDPC encoder (ARM CPUs only).
  std::string ldpc_encoder_type;
  /// \brief CRC calculator type.
  ///
  /// Use of these options:
  /// - \c auto: let the factory select the most efficient given the CPU architecture, or
  /// - \c lut: for using a look-up table CRC calculator, or
  /// - \c clmul: for using a look-up table CRC calculator (x86_64 CPUs only).
  std::string crc_calculator_type;
  /// \brief PDSCH processor type.
  ///
  /// Use of these options:
  /// - \c generic: for using unoptimized PDSCH processing, or
  /// - \c flexible: for using configurable processor that optimizes memory or performance via concurrency.
  std::variant<pdsch_processor_generic_configuration, pdsch_processor_flexible_configuration> pdsch_processor;
  /// \brief Optional hardware-accelerated PDSCH encoder factory.
  ///
  /// if the optional is not set, a software PDSCH encoder factory will be used.
  std::optional<std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory>> hw_encoder_factory;
  /// Task executor for processing PDCCH.
  upper_phy_executor pdcch_executor;
  /// Task executor for processing PDSCH.
  upper_phy_executor pdsch_executor;
  /// Task executor for processing SSB.
  upper_phy_executor ssb_executor;
  /// Task executor for processing NZP-CSI-RS.
  upper_phy_executor csi_rs_executor;
  /// Task executor for processing PRS.
  upper_phy_executor prs_executor;
};

/// \brief Downlink processor software factory configuration.
///
/// \remark Default values are empty strings which are invalid.
struct downlink_processor_factory_sw_dependencies {
  /// PDSCH codeblock executor. The generic PDSCH processor is used if this is invalid.
  upper_phy_executor pdsch_codeblock_executor;
  /// Optional metric notifier.
  upper_phy_metrics_notifiers* metric_notifier;
};

/// Creates a full software based downlink processor factory.
std::shared_ptr<downlink_processor_factory>
create_downlink_processor_factory_sw(const downlink_processor_factory_sw_config&       config,
                                     const downlink_processor_factory_sw_dependencies& dependencies);

/// Describes all downlink processors in a pool.
struct downlink_processor_pool_config {
  /// Set of downlink processors for a given numerology.
  struct downlink_processor_set {
    /// Subcarrier spacing.
    subcarrier_spacing scs;
    /// Pointers to the actual downlink processors.
    std::vector<std::unique_ptr<downlink_processor_base>> procs;
  };

  /// Collection of all downlink processors, organized by numerology.
  std::vector<downlink_processor_set> dl_processors;
};

/// \brief Creates and returns a downlink processor pool.
std::unique_ptr<downlink_processor_pool> create_dl_processor_pool(downlink_processor_pool_config config);

/// Collects upper PHY configuration parameters used to create a new upper PHY object.
struct upper_phy_configuration {
  /// Number of transmit antenna ports.
  unsigned nof_tx_ports;
  /// Number of receive antenna ports.
  unsigned nof_rx_ports;
  /// Number of downlink resource grids. Downlink resource grids minimum reuse time is \c dl_rg_expire_timeout_slots
  /// slots.
  unsigned nof_dl_rg;
  /// Number of uplink resource grids. They are reused after \c nof_ul_rg slots.
  unsigned nof_ul_rg;
  /// Number of PRACH buffer.
  unsigned nof_prach_buffer;
  /// Maximum number of time-domain occasions.
  unsigned max_nof_td_prach_occasions;
  /// Maximum number of frequency-domain occasions.
  unsigned max_nof_fd_prach_occasions;
  /// Set to true if the PRACH preamble is long.
  bool is_prach_long_format;
  /// Maximum number of concurrent downlink processes.
  unsigned nof_dl_processors;
  /// Number of RBs for downlink.
  unsigned dl_bw_rb;
  /// Number of RBs for uplink.
  unsigned ul_bw_rb;
  /// Maximum number of layers for PUSCH transmissions.
  unsigned pusch_max_nof_layers;
  /// List of active subcarrier spacing, indexed by numerology.
  std::array<bool, to_numerology_value(subcarrier_spacing::invalid)> active_scs;
  /// Receive buffer pool configuration.
  rx_buffer_pool_config rx_buffer_config;
};

/// Collects the necessary dependencies for creating an upper PHY.
struct upper_phy_dependencies {
  /// Upper PHY resource grid gateway.
  upper_phy_rg_gateway* rg_gateway;
  /// Received symbol request notifier.
  upper_phy_rx_symbol_request_notifier* rx_symbol_request_notifier;
};

/// Returns true if the given upper PHY configuration is valid, otherwise false.
inline bool is_valid_upper_phy_config(const upper_phy_configuration& config)
{
  // :TODO: Implement me!
  return true;
}

/// \brief Factory that builds upper PHY objects.
class upper_phy_factory
{
public:
  virtual ~upper_phy_factory() = default;

  /// \brief Creates and returns an upper PHY object.
  virtual std::unique_ptr<upper_phy> create(const upper_phy_configuration& config,
                                            const upper_phy_dependencies&  dependencies) = 0;
};

/// Collects the necessary configuration parameters for creating an upper physical layer factory.
struct upper_phy_factory_configuration {
  /// \brief Logging level.
  /// - \c none: No logging is enabled, or
  /// - \c info: all processing PDUs and their results are logged.
  srslog::basic_levels log_level;
  /// Enable logging broadcast channels such as SSB, PDSCH and PDCCH channels with broadcast RNTIs.
  bool enable_logging_broadcast;
  /// Logger maximum hexadecimal dump size. Set to zero for none.
  unsigned logger_max_hex_size;
  /// Enable metrics in the upper PHY.
  bool enable_metrics;
  /// Selects the PUSCH SINR calculation method used for choosing the modulation and coding scheme.
  channel_state_information::sinr_type pusch_sinr_calc_method;
  /// Receive symbol printer. Leave empty to disable.
  std::string rx_symbol_printer_filename;
  /// Receive port the UL symbols are dumped from. Leave emtpy for all ports.
  std::optional<unsigned> rx_symbol_printer_port;
  /// Boolean flag for dumping PRACH symbols when set to true.
  bool rx_symbol_printer_prach;
  /// Enables the PHY tap plugin if present.
  std::optional<std::string> phy_tap_arguments;
  /// \brief LDPC encoder type.
  ///
  /// Use of these options:
  /// - \c auto: let the factory select the most efficient given the CPU architecture, or
  /// - \c generic: for using unoptimized LDPC encoder, or
  /// - \c avx2: for using AVX2 optimized LDPC encoder (x86_64 CPUs only), or
  /// - \c neon: for using NEON optimized LDPC encoder (ARM CPUs only).
  std::string ldpc_encoder_type;
  /// \brief LDPC decoder type.
  ///
  /// Use one of these options:
  /// - \c auto: let the factory select the most efficient given the CPU architecture, or
  /// - \c generic: for using generic instructions, or
  /// - \c avx2: for using AVX2 instructions (x86_64 CPUs only), or
  /// - \c avx512: for using AVX512 instructions (x86_64 CPUs only), or
  /// - \c neon: for using NEON instructions (ARM CPUs only).
  std::string ldpc_decoder_type;
  /// \brief LDPC rate dematcher type.
  ///
  /// Use one of these options:
  /// - \c auto: let the factory select the most efficient given the CPU architecture, or
  /// - \c generic: for using generic instructions, or
  /// - \c avx2: for using AVX2 instructions (x86_64 CPUs only), or
  /// - \c avx512: for using AVX512 instructions (x86_64 CPUs only), or
  /// - \c neon: for using NEON instructions (ARM CPUs only).
  std::string ldpc_rate_dematcher_type;
  /// \brief CRC calculator type.
  ///
  /// Use one of these options:
  /// - \c auto: let the factory select the most efficient given the CPU architecture, or
  /// - \c lut: for using a look-up table CRC calculator, or
  /// - \c clmul: for using a look-up table CRC calculator (x86_64 CPUs only).
  std::string crc_calculator_type;
  /// \brief PUSCH channel estimator frequency-domain smoothing strategy.
  ///
  /// Use one of these options:
  /// - \c filter: applies a low pass filter to the channel estimates, or
  /// - \c mean: averages the channel estimates, or
  /// - \c none: it does not apply any smoothing strategy.
  std::string pusch_channel_estimator_fd_strategy;
  /// \brief PUSCH channel estimator time-domain interpolation strategy.
  ///
  /// Use one of these options:
  /// - \c average: averages the DM-RS in time domain, or
  /// - \c interpolate: performs linear interpolation between the OFDM symbols containing DM-RS.
  ///
  /// The \c average strategy is more robust against noise and interference while \c interpolate is more robust for
  /// fast fading channels.
  std::string pusch_channel_estimator_td_strategy;
  /// PUSCH channel estimator CFO compensation.
  bool pusch_channel_estimator_compensate_cfo;
  /// \brief PUSCH channel equalizer algorithm.
  ///
  /// Use one of these options:
  /// - \c zf: use zero-forcing algorithm, or
  /// - \c mmse: use minimum mean square error algorithm.
  std::string pusch_channel_equalizer_algorithm;
  /// Number of LDPC decoder iterations.
  unsigned ldpc_decoder_iterations;
  /// Set to true to enable the LDPC decoder early stop.
  bool ldpc_decoder_early_stop;
  /// Set to true for forcing the LDPC decoder to decode even if the number of soft bits is insufficient.
  bool ldpc_decoder_force_decoding;
  /// Number of receive antenna ports.
  unsigned nof_rx_ports;
  /// Number of RBs for uplink.
  unsigned ul_bw_rb;
  /// Maximum number of layers for PUSCH transmissions.
  unsigned pusch_max_nof_layers;
  /// \brief PDSCH processor type.
  ///
  /// Use of these options:
  /// - \c generic: for using unoptimized PDSCH processing, or
  /// - \c flexible: for using configurable processor that optimizes memory or performance via concurrency.
  std::variant<pdsch_processor_generic_configuration, pdsch_processor_flexible_configuration> pdsch_processor;
};

/// Collects the necessary dependencies for creating an upper physical layer factory.
struct upper_phy_factory_dependencies {
  /// Execution configuration
  upper_phy_execution_configuration executors;
  /// \brief Optional hardware-accelerated PDSCH encoder factory.
  ///
  /// if the optional is not set, a software PDSCH encoder factory will be used.
  std::optional<std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory>> hw_encoder_factory;
  /// \brief Optional hardware-accelerated PUSCH decoder factory.
  ///
  /// if the optional is not set, a software PUSCH decoder factory will be used.
  std::optional<std::shared_ptr<hal::hw_accelerator_pusch_dec_factory>> hw_decoder_factory;
};

/// Creates and returns an upper PHY factory.
std::unique_ptr<upper_phy_factory> create_upper_phy_factory(const upper_phy_factory_configuration& factory_config,
                                                            const upper_phy_factory_dependencies&  factory_deps);

/// Factory interface for creating upper physical layer receive symbol handlers.
class upper_phy_rx_symbol_handler_factory
{
public:
  /// Default destructor.
  virtual ~upper_phy_rx_symbol_handler_factory() = default;

  /// \brief Creates a new upper physical layer receive symbol handler.
  ///
  /// \param[in] ul_processor_pool_ Uplink slot processor pool.
  virtual std::unique_ptr<upper_phy_rx_symbol_handler> create(uplink_slot_processor_pool& ul_processor_pool_) = 0;
};

/// Creates an RX symbol handler factory.
std::shared_ptr<upper_phy_rx_symbol_handler_factory> create_rx_symbol_handler_factory();

/// \brief Creates an RX symbol handler printer decorator factory.
///
/// This factory is used to create an RX symbol handler that prints the received symbols to a file.
std::shared_ptr<upper_phy_rx_symbol_handler_factory>
create_rx_symbol_handler_printer_decorator_factory(std::shared_ptr<upper_phy_rx_symbol_handler_factory> factory_,
                                                   srslog::basic_logger&                                logger_,
                                                   const std::string&                                   filename_,
                                                   unsigned                                             nof_rb_,
                                                   interval<unsigned>                                   ul_print_ports_,
                                                   bool                                                 print_prach_);

/// Factory interface for creating an upper physical layer tap. This factory msut be defined in the physical layer tap
/// plugin.
class phy_tap_factory
{
public:
  /// Default destructor.
  virtual ~phy_tap_factory() = default;

  /// Creates a new upper physical layer tap.
  virtual std::unique_ptr<phy_tap> create() = 0;
};

/// Creates a physical layer tap factory.
std::shared_ptr<phy_tap_factory>
create_phy_tap_factory(unsigned nof_rb, unsigned nof_ports, const std::string& processor_arguments);

} // namespace srsran
