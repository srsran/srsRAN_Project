
#ifndef SRSGNB_PHY_UPPER_RX_SOFTBUFFER_POOL_IMPL_H
#define SRSGNB_PHY_UPPER_RX_SOFTBUFFER_POOL_IMPL_H

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/upper/rx_softbuffer_pool.h"

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

class rx_softbuffer_codeblock_pool
{
private:
  /// Describes a codeblock soft-bit buffer entry.
  struct entry {
    /// Indicates if the entry is reserved.
    bool reserved;
    /// Contains the codeblock soft bits.
    std::vector<int8_t> soft_bits;
  };

  /// Stores all codeblock entries.
  std::vector<entry> entries;

public:
  /// \brief Creates a receive buffer codeblock pool.
  /// \param[in] nof_codeblocks Indicates the maximum number of codeblocks.
  /// \param[in] max_codeblock_size Indicates the maximum codeblock size.
  rx_softbuffer_codeblock_pool(unsigned nof_codeblocks, unsigned max_codeblock_size)
  {
    entries.resize(nof_codeblocks);
    for (entry& e : entries) {
      e.reserved = false;
      e.soft_bits.resize(max_codeblock_size);
    }
  }

  /// \brief Reserves a codeblock softbuffer.
  /// \return The codeblock identifier in the pool.
  unsigned reserve()
  {
    // Find the first available codeblock.
    for (unsigned cb_id = 0; cb_id != entries.size(); ++cb_id) {
      if (!entries[cb_id].reserved) {
        entries[cb_id].reserved = true;
        return cb_id;
      }
    }
    srsran_terminate("Failed to reserve codeblock.");
  }

  /// \brief Frees a codeblock softbuffer.
  /// \param[in] cb_id Indicates the codeblock identifier in the pool.
  void free(unsigned cb_id)
  {
    srsran_always_assert(cb_id < entries.size(), "Codeblock index ({}) is out-of-range ({}).", cb_id, entries.size());
    srsran_always_assert(entries[cb_id].reserved, "Codeblock index ({}) is not reserved.", cb_id);
    entries[cb_id].reserved = false;
  }

  /// \brief Gets a codeblock soft-bit buffer.
  /// \param[in] cb_id Indicates the codeblock identifier.
  /// \return A view to the codeblock soft-bits buffer.
  span<int8_t> get_soft_bits(unsigned cb_id)
  {
    srsran_always_assert(cb_id < entries.size(), "Codeblock index ({}) is out-of-range ({}).", cb_id, entries.size());
    srsran_always_assert(entries[cb_id].reserved, "Codeblock index ({}) is not reserved.", cb_id);
    return entries[cb_id].soft_bits;
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
  /// Maximum number of codeblocks.
  static constexpr unsigned MAX_CODEBLOCKS = 52;
  /// Reference to the codeblock pool.
  rx_softbuffer_codeblock_pool& codeblock_pool;
  /// Stores codeblocks CRCs.
  static_vector<bool, MAX_CODEBLOCKS> crc;
  /// Stores codeblock identifiers.
  static_vector<unsigned, MAX_CODEBLOCKS> codeblock_ids;

public:
  /// \brief Creates a receive softbuffer.
  /// \param[in] pool Provides the codeblock softbuffer pool.
  rx_softbuffer_impl(rx_softbuffer_codeblock_pool& pool) : codeblock_pool(pool)
  {
    // Do nothing.
  }

  /// \brief Reserves the buffer.
  /// \param id Indicates the reservation identifier.
  /// \param expire_slot Indicates the slot at which the reservation expires.
  /// \param nof_codeblocks Indicates the number of codeblocks to reserve.
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

    // Resize storage.
    crc.resize(nof_codeblocks);
    codeblock_ids.resize(nof_codeblocks);

    // Reserve codeblocks.
    for (unsigned& cb_id : codeblock_ids) {
      cb_id = codeblock_pool.reserve();
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
    srsran_always_assert(is_reserved(), "Softbuffer is not reserved.");
    return crc.size();
  }

  // See interface for documentation.
  void reset_codeblocks_crc() override
  {
    srsran_always_assert(is_reserved(), "Softbuffer is not reserved.");
    // Set all codeblock CRC to false.
    for (bool& cb_crc : crc) {
      cb_crc = false;
    }
  }

  // See interface for documentation.
  span<bool> get_codeblocks_crc() override
  {
    srsran_always_assert(is_reserved(), "Softbuffer is not reserved.");
    return crc;
  }

  // See interface for documentation.
  span<int8_t> get_codeblock_soft_bits(unsigned codeblock_id, unsigned codeblock_size) override
  {
    srsran_always_assert(is_reserved(), "Softbuffer is not reserved.");
    srsran_always_assert(codeblock_id < codeblock_ids.size(),
                         "Codeblock index ({}) is out-of-range ({}).",
                         codeblock_id,
                         codeblock_ids.size());
    unsigned cb_id = codeblock_ids[codeblock_id];
    return codeblock_pool.get_soft_bits(cb_id).first(codeblock_size);
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

public:
  /// \brief Creates a generic receiver softbuffer pool.
  /// \param[in] config Provides the pool required parameters.
  rx_softbuffer_pool_impl(rx_softbuffer_pool_description& config) :
    codeblock_pool(config.max_nof_codeblocks, config.max_codeblock_size),
    buffers(config.max_softbuffers, codeblock_pool),
    expire_timeout_slots(config.expire_timeout_slots)
  {
    // Do nothing.
  }

  // See interface for documentation.
  rx_softbuffer*
  reserve_softbuffer(const slot_point& slot, const rx_softbuffer_identifier& id, unsigned int nof_codeblocks) override;

  // See interface for documentation.
  void free_softbuffer(const rx_softbuffer_identifier& id) override;

  // See interface for documentation.
  void run_slot(const slot_point& slot) override;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_RX_SOFTBUFFER_POOL_IMPL_H
