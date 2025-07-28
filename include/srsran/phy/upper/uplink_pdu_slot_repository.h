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

#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_processor.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_configuration.h"
#include "srsran/phy/upper/uplink_processor_context.h"
#include <variant>

namespace srsran {

/// \brief Uplink slot PDU repository.
///
/// This class registers PUSCH, PUCCH and SRS PDUs to be processed in the same slot context.
class uplink_pdu_slot_repository
{
public:
  /// PUSCH PDU configuration.
  struct pusch_pdu {
    /// HARQ process number.
    unsigned harq_id;
    /// Transport block size.
    units::bytes tb_size;
    /// PUSCH processor PDU.
    pusch_processor::pdu_t pdu;
  };

  /// PUCCH PDU configuration.
  struct pucch_pdu {
    /// PUCCH context.
    ul_pucch_context context;
    /// PUCCH configuration.
    std::variant<pucch_processor::format0_configuration,
                 pucch_processor::format1_configuration,
                 pucch_processor::format2_configuration,
                 pucch_processor::format3_configuration,
                 pucch_processor::format4_configuration>
        config;
  };

  /// Sounding Reference Signals PDU configuration.
  struct srs_pdu {
    /// SRS context.
    ul_srs_context context;
    /// Actual SRS channel estimator configuration.
    srs_estimator_configuration config;
  };

  /// Default destructor.
  virtual ~uplink_pdu_slot_repository() = default;

  /// Adds the given PUSCH PDU to the repository.
  virtual void add_pusch_pdu(const pusch_pdu& pdu) = 0;

  /// Adds the given PUCCH PDU to the repository.
  virtual void add_pucch_pdu(const pucch_pdu& pdu) = 0;

  /// Adds the given SRS PDU to the repository.
  virtual void add_srs_pdu(const srs_pdu& pdu) = 0;
};

/// \brief Unique uplink PDU slot repository.
///
/// Keeps the uplink PDU slot repository interface available for enqueuing uplink transmissions as long as it is
/// available in a scope.
///
/// The uplink PDU slot repository closes the window for accepting new PDUs when the unique instance is either destroyed
/// or released.
class unique_uplink_pdu_slot_repository
{
public:
  /// Uplink PDU repository underlying interface.
  class uplink_pdu_slot_repository_callback : public uplink_pdu_slot_repository
  {
  public:
    /// Default destructor.
    ~uplink_pdu_slot_repository_callback() override = default;

    /// Stops adding PDUs.
    virtual shared_resource_grid finish_adding_pdus() = 0;
  };

  /// Default constructor - creates an invalid uplink repository.
  unique_uplink_pdu_slot_repository() = default;

  /// Builds a unique uplink repository from an underlying instance.
  explicit unique_uplink_pdu_slot_repository(uplink_pdu_slot_repository_callback& repository_) :
    repository(&repository_)
  {
  }

  /// Disabled copy constructor.
  unique_uplink_pdu_slot_repository(const unique_uplink_pdu_slot_repository&) = delete;

  /// Disabled copy assignment operator.
  unique_uplink_pdu_slot_repository& operator=(const unique_uplink_pdu_slot_repository& other) = delete;

  /// Move constructor.
  unique_uplink_pdu_slot_repository(unique_uplink_pdu_slot_repository&& other) noexcept
  {
    release();
    repository       = other.repository;
    other.repository = nullptr;
  }

  /// Move assignment operator.
  unique_uplink_pdu_slot_repository& operator=(unique_uplink_pdu_slot_repository&& other) noexcept
  {
    release();
    repository       = other.repository;
    other.repository = nullptr;
    return *this;
  }

  /// Returns \c true if the unique repository is valid, false otherwise.
  bool is_valid() const { return repository != nullptr; }

  /// Default destructor - notifies the end of processing PDUs.
  ~unique_uplink_pdu_slot_repository() { release(); }

  /// \brief Releases the unique uplink PDU slot repository and obtain the resource grid.
  ///
  /// The unique PDU slot repository will stop being valid after this call and it will not accept for uplink PDU until a
  /// new slot is configured.
  ///
  /// \return A shared resource grid if the repository is valid, otherwise an invalid grid.
  shared_resource_grid release()
  {
    shared_resource_grid grid;

    // Notify the repository that no more PDU will be queued and obtain the resource grid for the configured slot.
    if (is_valid()) {
      grid = repository->finish_adding_pdus();
    }

    // Invalidate pointer to the instance. It will no longer be valid.
    repository = nullptr;

    return grid;
  }

  /// Gets the underlying uplink PDU slot repository.
  uplink_pdu_slot_repository& get()
  {
    srsran_assert(is_valid(), "Invalid repository.");
    return *repository;
  }

  /// Gets the underlying uplink PDU slot repository.
  uplink_pdu_slot_repository* operator->()
  {
    srsran_assert(is_valid(), "Invalid repository.");
    return repository;
  }

private:
  /// Reference to the underlying uplink repository. Set to \c nullptr for an invalid repository.
  uplink_pdu_slot_repository_callback* repository = nullptr;
};

/// Uplink slot PDU repository pool.
class uplink_pdu_slot_repository_pool
{
public:
  /// Default destructor.
  virtual ~uplink_pdu_slot_repository_pool() = default;

  /// Gets the uplink slot PDU repository associated to a given slot.
  virtual unique_uplink_pdu_slot_repository get_pdu_slot_repository(slot_point slot) = 0;
};
} // namespace srsran
