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
};

/// Describes all uplink processors in a pool.
struct uplink_processor_pool_config {
  /// Uplink processors for a given sector and numerology.
  struct sector_ul_processors {
    /// Base station sector identifie.
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
};

/// Factory that allows to create downlink processors.
class downlink_processor_factory
{
public:
  virtual ~downlink_processor_factory() = default;

  /// \brief Creates a return a downlink processor.
  virtual std::unique_ptr<downlink_processor> create(const downlink_processor_config& config) = 0;
};

/// Describes all downlink processors in a pool.
struct downlink_processor_pool_config {
  /// Downlink processors for a given sector and numerology.
  struct sector_dl_processor {
    /// Base station sector identifie.
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
};

/// \brief Creates and returns a downlink processor pool.
std::unique_ptr<downlink_processor_pool> create_dl_processor_pool(downlink_processor_pool_config config);

/// Upper PHY configuration parameters used to create a new upper PHY object.
struct upper_phy_config {
  /// Radio sector identifier.
  unsigned sector_id;
  /// Port identifier within the sector.
  unsigned nof_ports;
  /// Number of slots the downlink resource grid will support.
  unsigned nof_slots_dl_rg;
  /// Number of downlink processors.
  unsigned nof_dl_processors;
  /// Number of RBs for downlink.
  unsigned dl_bw_rb;
  /// Upper PHY resource grid gateway.
  upper_phy_rg_gateway* gateway;
  /// Downlink task executor.
  task_executor* dl_executor;
  /// Number of uplink processors.
  unsigned nof_ul_processors;
  /// Uplink task executor.
  task_executor* ul_executor;
  /// DFT size for a 15kHz subcarrier spacing.
  unsigned dft_size_15kHz;
  /// Number of RBs for uplink.
  unsigned ul_bw_rb;
  /// Received symbol request notifier.
  upper_phy_rx_symbol_request_notifier* symbol_request_notifier;
};

/// \brief Factory that builds upper PHY objects.
class upper_phy_factory
{
public:
  virtual ~upper_phy_factory() = default;

  /// \brief Creates and returns an upper PHY object.
  virtual std::unique_ptr<upper_phy> create(const upper_phy_config& config) = 0;
};

/// Creates and returns an upper PHY factory.
std::unique_ptr<upper_phy_factory> create_upper_phy_factory();

} // namespace srsgnb
