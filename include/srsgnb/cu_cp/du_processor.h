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

#include "cu_cp_types.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/ran/rnti.h"
#include <string>

namespace srsgnb {
namespace srs_cu_cp {

/// Forward declared messages.
struct f1_setup_request_message;

/// Interface used to handle cells
class du_processor_cell_handler
{
public:
  virtual ~du_processor_cell_handler() = default;

  /// \brief Lookup the cell based on a given NR cell ID.
  /// \param[in] packed_nr_cell_id The packed NR cell ID received over F1AP.
  virtual du_cell_index_t find_cell(uint64_t packed_nr_cell_id) = 0;

  /// \brief Get the DU index.
  /// \return The DU index.
  virtual du_index_t get_du_index() = 0;
};

class du_processor_f1ap_setup_handler
{
public:
  virtual ~du_processor_f1ap_setup_handler()                                = default;
  virtual void handle_f1_setup_request(const f1_setup_request_message& msg) = 0;
};

struct initial_ul_rrc_message {
  du_cell_index_t                           pcell_index;
  unsigned int                              tmp_ue_id;
  asn1::unbounded_octstring<true>           rrc_container;
  asn1::unbounded_octstring<true>           du_to_cu_rrc_container;
  rnti_t                                    c_rnti;
  optional<asn1::unbounded_octstring<true>> rrc_container_rrc_setup_complete;
};

struct ul_rrc_message {
  ue_index_t                      ue_index;
  uint8_t                         srbid;
  asn1::unbounded_octstring<true> rrc_container;
};

struct dl_rrc_message {
  asn1::unbounded_octstring<true> rrc_container;
};

/// Interface to forward RRC messages at the DU processor
class du_processor_rrc_message_handler
{
public:
  virtual ~du_processor_rrc_message_handler() = default;

  virtual void handle_initial_ul_rrc_message_transfer(const initial_ul_rrc_message& msg) = 0;
  virtual void handle_ul_rrc_message_transfer(const ul_rrc_message& msg)                 = 0;
};

/// Interface to receive RRC messages at the DU processor
class du_processor_dl_rrc_message_handler
{
public:
  virtual ~du_processor_dl_rrc_message_handler() = default;

  virtual void handle_dl_rrc_message_transfer(const dl_rrc_message& msg) = 0;
};

/// Interface to notify the reception of an new RRC message.
class du_processor_rrc_message_notifier
{
public:
  virtual ~du_processor_rrc_message_notifier() = default;

  /// This callback is invoked on each received RRC message.
  virtual void on_new_rrc_message(asn1::unbounded_octstring<true> rrc_container) = 0;
};

/// Methods used by F1AP to notify about RRC messages.
class du_processor_f1c_message_notifier
{
public:
  virtual ~du_processor_f1c_message_notifier() = default;

  /// \brief Notifies about the creation of an UE.
  /// \param[in] tmp_ue_id The CU UE ID, that was received with the Initial UL RRC message.
  /// \param[in] ue_index The index of the created UE.
  virtual void on_ue_creation(unsigned int tmp_ue_id, ue_index_t ue_index) = 0;
};

/// Combined interface for all DU processor handlers
class du_processor_f1c_interface : public du_processor_f1ap_setup_handler,
                                   public du_processor_cell_handler,
                                   public du_processor_rrc_message_handler,
                                   public du_processor_dl_rrc_message_handler
{
public:
  virtual ~du_processor_f1c_interface() = default;
};

} // namespace srs_cu_cp
} // namespace srsgnb
