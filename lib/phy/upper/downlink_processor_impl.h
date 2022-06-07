/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_PHY_UPPER_DOWNLINK_PROCESSOR_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_DOWNLINK_PROCESSOR_IMPL_H

#include "srsgnb/adt/circular_map.h"
#include "srsgnb/phy/resource_grid_context.h"
#include "srsgnb/phy/upper/downlink_processor.h"
#include <mutex>

namespace srsgnb {

class upper_phy_rg_gateway;
class task_executor;

/// Implementation of a downlink processor.
class downlink_processor_impl : public downlink_processor
{
public:
  downlink_processor_impl(upper_phy_rg_gateway&            gateway,
                          std::unique_ptr<pdcch_processor> pdcch_proc,
                          std::unique_ptr<pdsch_processor> pdsch_proc,
                          std::unique_ptr<ssb_processor>   ssb_proc,
                          task_executor&                   executor);

  void process_pdcch(pdcch_processor::pdu_t& pdu) override;

  void process_pdsch(const static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS>& data,
                     const pdsch_processor::pdu_t&                                                        pdu) override;
  void process_ssb(const ssb_processor::pdu_t& pdu) override;
  void configure_resource_grid(const resource_grid_context& context, resource_grid& grid) override;
  void send_resource_grid() override;

private:
  void send_grid();

private:
  upper_phy_rg_gateway&            gateway;
  resource_grid_context            rg_context;
  resource_grid*                   current_grid;
  std::unique_ptr<pdcch_processor> pdcch_proc;
  std::unique_ptr<pdsch_processor> pdsch_proc;
  std::unique_ptr<ssb_processor>   ssb_proc;
  task_executor&                   executor;

  unsigned num_pdus;
  bool     is_send_allowed;
  /// Protects num_pdus and is_send_allowed.
  // :TODO: remove me later
  std::mutex mutex;
};

struct downlink_processor_pool_config_impl {
  struct info {
    unsigned                                          sector;
    unsigned                                          numerology;
    std::vector<std::unique_ptr<downlink_processor> > procs;
  };

  std::vector<info> procs;
  unsigned          num_sectors;
};

/// Implementation of a downlink processor pool.
class downlink_processor_pool_impl : public downlink_processor_pool
{
  static constexpr unsigned MAX_NUM_NUMEROLOGIES = 5U;

  using processor_pool       = std::vector<std::unique_ptr<downlink_processor> >;
  using numerology_container = circular_map<uint32_t, processor_pool, MAX_NUM_NUMEROLOGIES>;

  /// \brief Represents a sector in the container of the pool.
  class sector
  {
    const unsigned       sector_id;
    numerology_container numerologies;

  public:
    sector() : sector_id(-1) {}

    sector(unsigned sector_id, numerology_container cont) : sector_id(sector_id), numerologies(std::move(cont)) {}

    processor_pool& operator[](int numerology)
    {
      auto iter = numerologies.find(numerology);
      srsran_assert(iter != numerologies.end(), "Invalid numerology ({}) in sector ({})", numerology, sector_id);

      return iter->second;
    }

    /// \brief Insert the given key and object in the container.
    void insert(unsigned key, processor_pool&& obj) { numerologies.insert(key, std::move(obj)); }
  };

public:
  explicit downlink_processor_pool_impl(downlink_processor_pool_config_impl dl_processors) :
    processors(dl_processors.num_sectors)
  {
    for (auto& proc : dl_processors.procs) {
      srsran_assert(!proc.procs.empty(), "Cannot store an empty processor pool");
      processors[proc.sector].insert(proc.numerology, std::move(proc.procs));
    }
  }

  downlink_processor& get_processor(const slot_point& slot, unsigned sector) override
  {
    srsran_assert(sector < processors.size(), "Invalid sector ({}) when requesting a downlink processor", sector);
    srsran_assert(slot.valid(), "Invalid slot ({}) when requesting a downlink processor", slot);

    auto&    sector_proc = processors[sector];
    auto&    proc_pool   = sector_proc[slot.numerology()];
    unsigned index       = slot.system_slot() % proc_pool.size();
    return *proc_pool[index];
  }

private:
  std::vector<sector> processors;
};

} // namespace srsgnb
#endif // SRSGNB_LIB_PHY_UPPER_DOWNLINK_PROCESSOR_IMPL_H
