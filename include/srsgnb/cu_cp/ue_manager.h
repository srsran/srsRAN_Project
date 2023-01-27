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

#include "ue_context.h"
#include "srsgnb/ngap/ngc.h"

namespace srsgnb {
namespace srs_cu_cp {

class du_processor_ue_manager
{
public:
  virtual ~du_processor_ue_manager() = default;

  /// Allocate new UE context for the given RNTI. A UE index is allocated internally.
  /// If a new UE can't be allocated or if a UE with the same RNTI already exists, nulltpr is returned.
  /// \param rnti RNTI of the UE to be added.
  virtual ue_context* add_ue(rnti_t rnti)            = 0;
  virtual void        remove_ue(ue_index_t ue_index) = 0;
  virtual ue_context* find_ue(ue_index_t ue_index)   = 0;
  virtual ue_index_t  find_ue_index(rnti_t rnti)     = 0;
  virtual size_t      get_nof_ues()                  = 0;
};

class ngc_ue
{
public:
  ngc_ue(cu_cp_ue_id_t                      cu_cp_ue_id,
         ran_ue_id_t                        ran_ue_id,
         ngc_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier_,
         ngc_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier_,
         ngc_du_processor_control_notifier& du_processor_ctrl_notifier_) :
    rrc_ue_pdu_notifier(rrc_ue_pdu_notifier_),
    rrc_ue_ctrl_notifier(rrc_ue_ctrl_notifier_),
    du_processor_ctrl_notifier(du_processor_ctrl_notifier_)
  {
    ue_ctxt.cu_cp_ue_id = cu_cp_ue_id;
    ue_ctxt.ran_ue_id   = ran_ue_id;
  }

  ngc_rrc_ue_pdu_notifier&           get_rrc_ue_pdu_notifier() { return rrc_ue_pdu_notifier; }
  ngc_rrc_ue_control_notifier&       get_rrc_ue_control_notifier() { return rrc_ue_ctrl_notifier; }
  ngc_du_processor_control_notifier& get_du_processor_control_notifier() { return du_processor_ctrl_notifier; }

  cu_cp_ue_id_t get_cu_cp_ue_id() { return ue_ctxt.cu_cp_ue_id; }
  amf_ue_id_t   get_amf_ue_id() { return ue_ctxt.amf_ue_id; }
  ran_ue_id_t   get_ran_ue_id() { return ue_ctxt.ran_ue_id; }

  uint64_t get_aggregate_maximum_bit_rate_dl() { return ue_ctxt.aggregate_maximum_bit_rate_dl; }
  void     set_aggregate_maximum_bit_rate_dl(uint64_t aggregate_maximum_bit_rate_dl)
  {
    ue_ctxt.aggregate_maximum_bit_rate_dl = aggregate_maximum_bit_rate_dl;
  }

  /// \brief Set the AMF UE ID in the NGC UE.
  /// \remark These functions should only be used by the UE manager.
  /// \param amf_ue_id The AMF UE ID to set
  void _set_amf_ue_id(amf_ue_id_t amf_ue_id) { ue_ctxt.amf_ue_id = amf_ue_id; }

private:
  ngc_ue_context                     ue_ctxt;
  ngc_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier;
  ngc_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier;
  ngc_du_processor_control_notifier& du_processor_ctrl_notifier;
};

class ngc_ue_manager
{
public:
  virtual ~ngc_ue_manager() = default;

  virtual ngc_ue&       add_ue(cu_cp_ue_id_t                      cu_cp_ue_id,
                               ngc_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier,
                               ngc_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier,
                               ngc_du_processor_control_notifier& du_processor_ctrl_notifier) = 0;
  virtual void          remove_ue(cu_cp_ue_id_t cu_cp_ue_id)                                  = 0;
  virtual ngc_ue*       find_ue(cu_cp_ue_id_t cu_cp_ue_id)                                    = 0;
  virtual size_t        get_nof_ngc_ues()                                                     = 0;
  virtual void          set_amf_ue_id(cu_cp_ue_id_t cu_cp_ue_id, amf_ue_id_t amf_ue_id)       = 0;
  virtual cu_cp_ue_id_t get_cu_cp_ue_id(ran_ue_id_t ran_ue_id)                                = 0;
  virtual cu_cp_ue_id_t get_cu_cp_ue_id(amf_ue_id_t amf_ue_id)                                = 0;
  virtual bool          contains(cu_cp_ue_id_t cu_cp_ue_id)                                   = 0;
};

} // namespace srs_cu_cp
} // namespace srsgnb
