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

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/upper/codeblock_metadata.h"
#include "srsgnb/phy/upper/rx_softbuffer_pool.h"
#include "srsgnb/support/math_utils.h"
#include <mutex>

namespace srsgnb {

/// Describes a receiver softbuffer pool parameters.
struct rx_softbuffer_pool_description {
  /// Indicates the maximum codeblock size.
  unsigned max_codeblock_size;
  /// Indicates the number of softbuffers available in the pool.
  unsigned max_softbuffers;
  /// Indicates the number of codeblocks available in the pool for all the softbuffers.
  unsigned max_nof_codeblocks;
  /// Indicates the softbuffer lifetime as a number of slots.
  unsigned expire_timeout_slots;
};

/// Manages a codeblock buffer pool.
class rx_softbuffer_codeblock_pool
{
private:
  /// Describes a codeblock buffer entry.
  struct entry {
    /// Indicates if the entry is reserved.
    bool reserved;
    /// Contains the codeblock soft bits.
    std::vector<log_likelihood_ratio> soft_bits;
    /// Contains the codeblock data bits.
    std::vector<uint8_t> data_bits;
  };

  /// Stores all codeblock entries.
  std::vector<entry> entries;

public:
  /// Default CB identifier for unreserved codeblocks.
  static constexpr unsigned UNRESERVED_CB_ID = UINT32_MAX;

  /// \brief Creates a receive buffer codeblock pool.
  /// \param[in] nof_codeblocks Indicates the maximum number of codeblocks.
  /// \param[in] max_codeblock_size Indicates the maximum codeblock size.
  rx_softbuffer_codeblock_pool(unsigned nof_codeblocks, unsigned max_codeblock_size)
  {
    entries.resize(nof_codeblocks);
    for (entry& e : entries) {
      e.reserved = false;
      e.soft_bits.resize(max_codeblock_size);
      // The maximum number of data bits is
      // max_codeblock_size * max(BG coding rate) = max_codeblock_size * (1/3)
      e.data_bits.resize(divide_ceil(max_codeblock_size, 3));
    }
  }

  /// \brief Reserves a codeblock softbuffer.
  /// \return The codeblock identifier in the pool if it is reserved successfully. Otherwise, \c UNRESERVED_CB_ID
  unsigned reserve()
  {
    // Find the first available codeblock.
    for (unsigned cb_id = 0; cb_id != entries.size(); ++cb_id) {
      if (!entries[cb_id].reserved) {
        entries[cb_id].reserved = true;
        return cb_id;
      }
    }
    return UNRESERVED_CB_ID;
  }

  /// \brief Frees a codeblock softbuffer.
  /// \param[in] cb_id Indicates the codeblock identifier in the pool.
  void free(unsigned cb_id)
  {
    // Skip if the codeblock identifier is equal to the unreserved identifier.
    if (cb_id == UNRESERVED_CB_ID) {
      return;
    }

    SRSGNB_ALWAYS_ASSERT__(cb_id < entries.size(), "Codeblock index ({}) is out-of-range ({}).", cb_id, entries.size());
    SRSGNB_ALWAYS_ASSERT__(entries[cb_id].reserved, "Codeblock index ({}) is not reserved.", cb_id);
    entries[cb_id].reserved = false;
  }

  /// \brief Gets a codeblock soft-bit buffer.
  /// \param[in] cb_id Indicates the codeblock identifier.
  /// \return A view to the codeblock soft-bit buffer.
  span<log_likelihood_ratio> get_soft_bits(unsigned cb_id)
  {
    SRSGNB_ALWAYS_ASSERT__(cb_id < entries.size(), "Codeblock index ({}) is out-of-range ({}).", cb_id, entries.size());
    SRSGNB_ALWAYS_ASSERT__(entries[cb_id].reserved, "Codeblock index ({}) is not reserved.", cb_id);
    return entries[cb_id].soft_bits;
  }

  /// \brief Gets a codeblock data-bit buffer.
  /// \param[in] cb_id Indicates the codeblock identifier.
  /// \return A view to the codeblock data-bit buffer.
  span<uint8_t> get_data_bits(unsigned cb_id)
  {
    SRSGNB_ALWAYS_ASSERT__(cb_id < entries.size(), "Codeblock index ({}) is out-of-range ({}).", cb_id, entries.size());
    SRSGNB_ALWAYS_ASSERT__(entries[cb_id].reserved, "Codeblock index ({}) is not reserved.", cb_id);
    return entries[cb_id].data_bits;
  }
};

/// Implements a receiver softbuffer interface.
class rx_softbuffer_impl : public rx_softbuffer
{
private:
  /// Reservation identifier.
  rx_softbuffer_identifier reservation_id;
  /// Indicates the slot the softbuffer will expire at.
  slot_point reservation_expire_slot;
  /// Reference to the codeblock pool.
  rx_softbuffer_codeblock_pool& codeblock_pool;
  /// Stores codeblocks CRCs.
  static_vector<bool, MAX_NOF_SEGMENTS> crc;
  /// Stores codeblock identifiers.
  static_vector<unsigned, MAX_NOF_SEGMENTS> codeblock_ids;

public:
  /// \brief Creates a receive softbuffer.
  /// \param[in] pool Provides the codeblock softbuffer pool.
  rx_softbuffer_impl(rx_softbuffer_codeblock_pool& pool) : reservation_id(), codeblock_pool(pool)
  {
    // Do nothing.
  }

  /// \brief Reserves the buffer.
  /// \param[in] id Indicates the reservation identifier.
  /// \param[in] expire_slot Indicates the slot at which the reservation expires.
  /// \param[in] nof_codeblocks Indicates the number of codeblocks to reserve.
  void reserve(const rx_softbuffer_identifier& id, const slot_point& expire_slot, unsigned int nof_codeblocks)
  {
    // Update reservation information.
    reservation_id          = id;
    reservation_expire_slot = expire_slot;

    // If the number of codeblocks match, skip the rest.
    if (nof_codeblocks == codeblock_ids.size()) {
      return;
    }

    // Make sure there are no buffers before reserving.
    free();

    // Resize CRCs.
    crc.resize(nof_codeblocks);

    // Resize codeblocks with codeblock unreserved identifier.
    codeblock_ids.resize(nof_codeblocks, static_cast<unsigned>(rx_softbuffer_codeblock_pool::UNRESERVED_CB_ID));

    // Reserve codeblocks.
    for (unsigned& cb_id : codeblock_ids) {
      // Reserve codeblock.
      cb_id = codeblock_pool.reserve();

      // Make sure the CB identifier is valid.
      if (cb_id == rx_softbuffer_codeblock_pool::UNRESERVED_CB_ID) {
        // Free the rest of the softbuffer.
        free();
        return;
      }
    }
  }

  /// Returns the reservation to the pool.
  void free()
  {
    // Free codeblocks.
    for (unsigned cb_id : codeblock_ids) {
      codeblock_pool.free(cb_id);
    }

    // Indicate the buffer is available by clearing the codeblocks identifiers.
    codeblock_ids.clear();
  }

  /// \brief Releases buffers that expire at the current slot.
  /// \param[in] slot Indicates the current slot.
  void run_slot(const slot_point& slot)
  {
    // Skip if it is not reseved.
    if (!is_reserved()) {
      return;
    }

    // Check the expiration of the buffer reservation.
    if (reservation_expire_slot <= slot) {
      free();
    }
  }

  /// Determines if the buffer matches the given identifier.
  bool match_id(const rx_softbuffer_identifier& identifier) const
  {
    return is_reserved() && (reservation_id == identifier);
  }

  /// Check if the softbuffer is reserved.
  bool is_reserved() const { return !codeblock_ids.empty(); }

  // See interface for documentation.
  unsigned get_nof_codeblocks() const override
  {
    SRSGNB_ALWAYS_ASSERT__(is_reserved(), "Softbuffer is not reserved.");
    return crc.size();
  }

  // See interface for documentation.
  void reset_codeblocks_crc() override
  {
    SRSGNB_ALWAYS_ASSERT__(is_reserved(), "Softbuffer is not reserved.");
    // Set all codeblock CRC to false.
    for (bool& cb_crc : crc) {
      cb_crc = false;
    }
  }

  // See interface for documentation.
  span<bool> get_codeblocks_crc() override
  {
    SRSGNB_ALWAYS_ASSERT__(is_reserved(), "Softbuffer is not reserved.");
    return crc;
  }

  // See interface for documentation.
  span<log_likelihood_ratio> get_codeblock_soft_bits(unsigned codeblock_id, unsigned codeblock_size) override
  {
    SRSGNB_ALWAYS_ASSERT__(is_reserved(), "Softbuffer is not reserved.");
    SRSGNB_ALWAYS_ASSERT__(codeblock_id < codeblock_ids.size(),
                           "Codeblock index ({}) is out of range ({}).",
                           codeblock_id,
                           codeblock_ids.size());
    unsigned cb_id       = codeblock_ids[codeblock_id];
    unsigned cb_max_size = codeblock_pool.get_soft_bits(cb_id).size();
    srsgnb_assert(
        codeblock_size <= cb_max_size, "Codeblock size {} exceeds maximum size {}.", codeblock_size, cb_max_size);
    return codeblock_pool.get_soft_bits(cb_id).first(codeblock_size);
  }

  // See interface for documentation.
  span<uint8_t> get_codeblock_data_bits(unsigned codeblock_id, unsigned data_size) override
  {
    SRSGNB_ALWAYS_ASSERT__(is_reserved(), "Softbuffer is not reserved.");
    SRSGNB_ALWAYS_ASSERT__(codeblock_id < codeblock_ids.size(),
                           "Codeblock index ({}) is out of range ({}).",
                           codeblock_id,
                           codeblock_ids.size());
    unsigned cb_id         = codeblock_ids[codeblock_id];
    unsigned data_max_size = codeblock_pool.get_data_bits(cb_id).size();
    srsgnb_assert(
        data_size <= data_max_size, "Codeblock data size {} exceeds maximum size {}.", data_size, data_max_size);
    return codeblock_pool.get_data_bits(cb_id).first(data_size);
  }
};

/// Describes a receiver softbuffer pool.
class rx_softbuffer_pool_impl : public rx_softbuffer_pool
{
private:
  /// Codeblock softbuffer pool.
  rx_softbuffer_codeblock_pool codeblock_pool;
  /// Storage of softbuffers.
  std::vector<rx_softbuffer_impl> buffers;
  /// Indicates the lifetime of a softbuffer reservation as a number of slots.
  unsigned expire_timeout_slots;
  /// Protects methods from concurrent calls.
  std::mutex mutex;

public:
  /// \brief Creates a generic receiver softbuffer pool.
  /// \param[in] config Provides the pool required parameters.
  rx_softbuffer_pool_impl(const rx_softbuffer_pool_description& config) :
    codeblock_pool(config.max_nof_codeblocks, config.max_codeblock_size),
    buffers(config.max_softbuffers, codeblock_pool),
    expire_timeout_slots(config.expire_timeout_slots)
  {
    // Do nothing.
  }

  // See interface for documentation.
  rx_softbuffer*
  reserve_softbuffer(const slot_point& slot, const rx_softbuffer_identifier& id, unsigned nof_codeblocks) override;

  // See interface for documentation.
  void free_softbuffer(const rx_softbuffer_identifier& id) override;

  // See interface for documentation.
  void run_slot(const slot_point& slot) override;
};

std::unique_ptr<rx_softbuffer_pool> create_rx_softbuffer_pool(const rx_softbuffer_pool_description& config);

} // namespace srsgnb
