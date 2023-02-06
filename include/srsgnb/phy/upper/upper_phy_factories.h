/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/downlink_processor.h"
#include "srsgnb/phy/upper/rx_softbuffer_pool.h"
#include "srsgnb/phy/upper/uplink_processor.h"
#include "srsgnb/phy/upper/upper_phy.h"
#include <memory>

namespace srsgnb {

class task_executor;
class upper_phy_rg_gateway;
class upper_phy_rx_symbol_request_notifier;

/// Configuration parameters for uplink processors.
struct uplink_processor_config {
  /// Base station sector identifier.
  unsigned sector_id;
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
  /// Uplink processors for a given sector and numerology.
  struct sector_ul_processors {
    /// Base station sector identifier.
    unsigned sector;
    /// Subcarrier spacing.
    subcarrier_spacing scs;
    /// Pointers to the actual uplink processors.
    std::vector<std::unique_ptr<uplink_processor>> procs;
  };

  /// Collection of all uplink processors, organized by radio sector and numerology.
  std::vector<sector_ul_processors> ul_processors;
  /// Number of radio sectors.
  unsigned num_sectors;
};

/// \brief Creates and returns an uplink processor pool.
std::unique_ptr<uplink_processor_pool> create_uplink_processor_pool(uplink_processor_pool_config config);

/// Configuration parameters for downlink processors.
struct downlink_processor_config {
  /// Radio sector identifier.
  unsigned id;
  /// Resource grid gateway.
  upper_phy_rg_gateway* gateway;
  /// Task executor.
  task_executor* executor;
};

/// Factory that allows to create downlink processors.
class downlink_processor_factory
{
public:
  virtual ~downlink_processor_factory() = default;

  /// \brief Creates a downlink processor.
  virtual std::unique_ptr<downlink_processor> create(const downlink_processor_config& config) = 0;

  /// \brief Creates a downlink processor with logging capabilities.
  virtual std::unique_ptr<downlink_processor>
  create(const downlink_processor_config& config, srslog::basic_logger& logger, bool enable_broadcast) = 0;

  /// \brief Creates a downlink PDU validator.
  /// \return A unique pointer to the new downlink PDU validator if successful or \c nullptr otherwise.
  virtual std::unique_ptr<downlink_pdu_validator> create_pdu_validator() = 0;
};

/// \brief Downlink processor software factory configuration.
///
/// \remark Default values are empty strings which are invalid.
struct downlink_processor_factory_sw_config {
  /// \brief LDPC encoder type.
  ///
  /// Use of there options:
  /// - \c auto: let the factory select the most efficient given the CPU architecture, or
  /// - \c generic: for using unoptimized LDPC encoder, or
  /// - \c avx2: for using AVX2 optimized LDPC encoder (x86_64 CPUs only), or
  /// - \c neon: for using NEON optimized LDPC encoder (ARM CPUs only).
  std::string ldpc_encoder_type;
  /// \brief CRC calculator type.
  ///
  /// Use of there options:
  /// - \c auto: let the factory select the most efficient given the CPU architecture, or
  /// - \c lut: for using a look-up table CRC calculator, or
  /// - \c clmul: for using a look-up table CRC calculator (x86_64 CPUs only).
  std::string crc_calculator_type;
};

/// Creates a full software based downlink processor factory.
std::shared_ptr<downlink_processor_factory>
create_downlink_processor_factory_sw(const downlink_processor_factory_sw_config& config);

/// Describes all downlink processors in a pool.
struct downlink_processor_pool_config {
  /// Downlink processors for a given sector and numerology.
  struct sector_dl_processor {
    /// Base station sector identifier.
    unsigned sector;
    /// Subcarrier spacing.
    subcarrier_spacing scs;
    /// Pointers to the actual downlink processors.
    std::vector<std::unique_ptr<downlink_processor>> procs;
  };

  /// Collection of all downlink processors, organized by radio sector and numerology.
  std::vector<sector_dl_processor> dl_processors;
  /// Number of base station sector.
  unsigned num_sectors;
  /// Null processor.
  std::unique_ptr<downlink_processor> null_proc;
};

/// \brief Creates and returns a downlink processor pool.
std::unique_ptr<downlink_processor_pool> create_dl_processor_pool(downlink_processor_pool_config config);

/// Upper PHY configuration parameters used to create a new upper PHY object.
struct upper_phy_config {
  /// \brief Logging level.
  /// - \c none: No logging is enabled, or
  /// - \c info: all processing PDUs and their results are logged.
  srslog::basic_levels log_level;
  /// Enable logging broadcast channels such as SSB, PDSCH and PDCCH channels with broadcast RNTIs.
  bool enable_logging_broadcast;
  /// Logger maximum hexadecimal dump size. Set to zero for none.
  unsigned logger_max_hex_size;
  /// Enables the EVM calculation for PUSCH.
  bool enable_evm;
  /// Receive symbol printer. Leave empty to disable.
  std::string rx_symbol_printer_filename;
  /// \brief LDPC decoder type.
  ///
  /// Use of there options:
  /// - \c auto: let the factory select the most efficient given the CPU architecture, or
  /// - \c generic: for using generic instructions, or
  /// - \c avx2: for using AVX2 instructions (x86_64 CPUs only), or
  /// - \c avx512: for using AVX512 instructions (x86_64 CPUs only), or
  /// - \c neon: for using NEON instructions (ARM CPUs only).
  std::string ldpc_decoder_type;
  /// \brief LDPC rate dematcher type.
  ///
  /// Use of there options:
  /// - \c auto: let the factory select the most efficient given the CPU architecture, or
  /// - \c generic: for using generic instructions, or
  /// - \c avx2: for using AVX2 instructions (x86_64 CPUs only), or
  /// - \c avx512: for using AVX512 instructions (x86_64 CPUs only), or
  /// - \c neon: for using NEON instructions (ARM CPUs only).
  std::string ldpc_rate_dematcher_type;
  /// \brief CRC calculator type.
  ///
  /// Use of there options:
  /// - \c auto: let the factory select the most efficient given the CPU architecture, or
  /// - \c lut: for using a look-up table CRC calculator, or
  /// - \c clmul: for using a look-up table CRC calculator (x86_64 CPUs only).
  std::string crc_calculator_type;
  /// Number of LDPC decoder iterations.
  unsigned ldpc_decoder_iterations;
  /// Set to true to enable the LDPC decoder early stop.
  bool ldpc_decoder_early_stop;
  /// Radio sector identifier.
  unsigned sector_id;
  /// Port identifier within the sector.
  unsigned nof_ports;
  /// Number of slots the downlink resource grid will support.
  unsigned nof_slots_dl_rg;
  /// Number of slots the uplink resource grid will support.
  unsigned nof_slots_ul_rg;
  /// Number of PRACH buffer.
  unsigned nof_prach_buffer;
  /// Maximum number of concurrent downlink processes.
  unsigned nof_dl_processors;
  /// Maximum number of concurrent uplink processes.
  unsigned nof_ul_processors;
  /// Maximum uplink processor thread concurrency.
  unsigned max_ul_thread_concurrency;
  /// Number of RBs for downlink.
  unsigned dl_bw_rb;
  /// Number of RBs for uplink.
  unsigned ul_bw_rb;
  /// List of active subcarrier spacing, indexed by numerology.
  std::array<bool, to_numerology_value(subcarrier_spacing::invalid)> active_scs;
  /// Softbuffer pool configuration.
  rx_softbuffer_pool_config softbuffer_config;
  /// Upper PHY resource grid gateway.
  upper_phy_rg_gateway* rg_gateway;
  /// Downlink task executor.
  task_executor* dl_executor;
  /// PUCCH task executor.
  task_executor* pucch_executor;
  /// PUSCH task executor.
  task_executor* pusch_executor;
  /// PRACH task executor.
  task_executor* prach_executor;
  /// Received symbol request notifier.
  upper_phy_rx_symbol_request_notifier* rx_symbol_request_notifier;
};

/// Returns true if the given upper PHY configuration is valid, otherwise false.
inline bool is_valid_upper_phy_config(const upper_phy_config& config)
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
  virtual std::unique_ptr<upper_phy> create(const upper_phy_config& config) = 0;
};

/// Creates and returns an upper PHY factory.
std::unique_ptr<upper_phy_factory>
create_upper_phy_factory(std::shared_ptr<downlink_processor_factory> downlink_proc_factory);

} // namespace srsgnb
