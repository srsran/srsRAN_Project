
#ifndef SRSGNB_DU_MANAGER_H
#define SRSGNB_DU_MANAGER_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/rnti.h"
#include <string>

namespace srsgnb {

struct ul_ccch_indication_message;

struct du_ue_delete_message {
  du_ue_index_t ue_index;
};

/// Interface used to handle external events (e.g. UL CCCH).
class du_manager_ccch_handler
{
public:
  virtual ~du_manager_ccch_handler()                                            = default;
  virtual void handle_ul_ccch_indication(const ul_ccch_indication_message& msg) = 0;
};

class du_manager_interface_query
{
public:
  virtual ~du_manager_interface_query() = default;
  virtual size_t nof_ues()              = 0;
};

class du_manager_controller
{
public:
  virtual ~du_manager_controller() = default;
  virtual void start()             = 0;
  virtual void stop()              = 0;
};

class du_manager_interface : public du_manager_interface_query,
                             public du_manager_ccch_handler,
                             public du_manager_controller
{
public:
  virtual ~du_manager_interface() = default;
};

} // namespace srsgnb

#endif // SRSGNB_DU_MANAGER_H
