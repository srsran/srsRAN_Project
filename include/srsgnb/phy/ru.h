#ifndef SRSGNB_PHY_RU_H
#define SRSGNB_PHY_RU_H

#include "srsgnb/adt/slot_context.h"
#include "srsgnb/phy/resource_grid.h"

namespace srsgnb {

/**
 * @brief Describes the DU interface that the RU uses to perform asynchronous callbacks
 */
class du_callback_interface
{
public:
  virtual ~du_callback_interface()                                    = default;
  virtual void ru_rx_callback()                                       = 0;
  virtual void ru_rx_prach_callback()                                 = 0;
  virtual void ru_rx_srs_callback()                                   = 0;
  virtual void ru_dl_tti_callback(const slot_context_t& slot_context) = 0;
  virtual void ru_ul_half_slot_callback()                             = 0;
  virtual void ru_ul_full_slot_callback()                             = 0;
};

/**
 * @brief Public interface that the PHY layer (in the DU) uses to retrieve the resource grids
 */
class ru_interface
{
public:
  struct context_t {
    slot_context_t slot;
    unsigned       sector;
    unsigned       antenna;
  };
  virtual ~ru_interface() = default;

  /**
   * @brief Get the DL resource grid for the given context
   * @param context The given context
   * @return The reference of the resource grid
   */
  virtual resource_grid_writer& get_dl_resource_grid(const context_t& context) = 0;

  /**
   * @brief Get the UL resource grid for the given context
   * @param context The given context
   * @return The reference of the resource grid
   */
  virtual const resource_grid_reader& get_ul_resource_grid(const context_t& context) = 0;
};

class ru : public ru_interface
{
protected:
  du_callback_interface& du;

public:
  /**
   * @brief It is compulsory that the RU is instanced with the DU instance
   * @param du_ The DU instance
   */
  explicit ru(du_callback_interface& du_) : du(du_){};

  /**
   * @brief Default destructor
   */
  virtual ~ru() = default;

  /**
   * @brief Starts RU operation
   */
  virtual void start() = 0;

  /**
   * Stops RU operation
   */
  virtual void stop() = 0;
};

};     // namespace srsgnb
#endif // SRSGNB_PHY_RU_H
