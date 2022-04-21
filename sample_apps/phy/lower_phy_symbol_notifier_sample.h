
#ifndef SAMPLE_APPS_PHY_LOWER_PHY_SYMBOL_NOTIFIER_SAMPLE_H
#define SAMPLE_APPS_PHY_LOWER_PHY_SYMBOL_NOTIFIER_SAMPLE_H

#include "srsgnb/phy/lower/lower_phy_rx_symbol_notifier.h"
#include "srsgnb/phy/lower/lower_phy_timing_notifier.h"
#include <mutex>

namespace srsgnb {

class lower_phy_rx_symbol_notifier_sample : public lower_phy_rx_symbol_notifier
{
private:
  srslog::basic_logger& logger;
  std::mutex            mutex;

public:
  lower_phy_rx_symbol_notifier_sample(std::string log_level = "none") : logger(srslog::fetch_basic_logger("RxSyHan"))
  {
    logger.set_level(srslog::str_to_basic_level(log_level));
  }
  void on_rx_symbol(const lower_phy_rx_symbol_context_t& context, const resource_grid_reader& grid) override
  {
    std::unique_lock<std::mutex> lock(mutex);
    logger.set_context(context.slot.system_slot());
    logger.debug("Rx symbol {} received for sector {}", context.nof_symbols, context.sector);
  }
  void on_rx_prach_symbol(const lower_phy_rx_symbol_context_t& context) override
  {
    std::unique_lock<std::mutex> lock(mutex);
    logger.set_context(context.slot.system_slot());
    logger.debug("PRACH symbol {} received for sector {}", context.nof_symbols, context.sector);
  }
  void on_rx_srs_symbol(const lower_phy_rx_symbol_context_t& context) override
  {
    std::unique_lock<std::mutex> lock(mutex);
    logger.set_context(context.slot.system_slot());
    logger.debug("SRS symbol {} received for sector {}", context.nof_symbols, context.sector);
  }
};

} // namespace srsgnb

#endif // SAMPLE_APPS_PHY_LOWER_PHY_SYMBOL_NOTIFIER_SAMPLE_H
