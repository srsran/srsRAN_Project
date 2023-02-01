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

#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/rrc/rrc_ue.h"

namespace srsgnb {
namespace srs_cu_cp {

class du_ue
{
public:
  du_ue(ue_index_t ue_index_, rnti_t c_rnti_)

  {
    ue_ctxt.ue_index = ue_index_;
    ue_ctxt.c_rnti   = c_rnti_;
  }

  slotted_id_vector<srb_id_t, cu_srb_context>&  get_srbs() { return srbs; }
  rrc_ue_task_scheduler&                        get_task_sched() { return *task_sched; }
  du_processor_rrc_ue_control_message_notifier& get_rrc_ue_notifier() { return *rrc_ue_notifier; }

  ue_index_t      get_ue_index() { return ue_ctxt.ue_index; }
  rnti_t          get_c_rnti() { return ue_ctxt.c_rnti; }
  du_cell_index_t get_pcell_index() { return ue_ctxt.pcell_index; }

  void set_pcell_index(du_cell_index_t pcell_index) { ue_ctxt.pcell_index = pcell_index; }

  void set_task_sched(rrc_ue_task_scheduler* task_sched_) { task_sched = task_sched_; }

  void set_rrc_ue_notifier(du_processor_rrc_ue_control_message_notifier* rrc_ue_notifier_)
  {
    rrc_ue_notifier = rrc_ue_notifier_;
  }

private:
  du_ue_context ue_ctxt;

  slotted_id_vector<srb_id_t, cu_srb_context>   srbs;
  rrc_ue_task_scheduler*                        task_sched      = nullptr;
  du_processor_rrc_ue_control_message_notifier* rrc_ue_notifier = nullptr;
};

class du_processor_ue_manager
{
public:
  virtual ~du_processor_ue_manager() = default;

  /// \brief Allocate new UE context for the given RNTI. A UE index is allocated internally. If a new UE can't be
  /// allocated or if a UE with the same RNTI already exists, nulltpr is returned.
  /// \param[in] rnti RNTI of the UE to be added.
  virtual du_ue*     add_du_ue(rnti_t rnti)            = 0;
  virtual void       remove_du_ue(ue_index_t ue_index) = 0;
  virtual du_ue*     find_du_ue(ue_index_t ue_index)   = 0;
  virtual ue_index_t get_ue_index(rnti_t rnti)         = 0;
  virtual size_t     get_nof_du_ues()                  = 0;
};

class ngap_ue
{
public:
  ngap_ue(ue_index_t                         ue_index,
          ran_ue_id_t                        ran_ue_id,
          ngc_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier_,
          ngc_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier_,
          ngc_du_processor_control_notifier& du_processor_ctrl_notifier_) :
    rrc_ue_pdu_notifier(rrc_ue_pdu_notifier_),
    rrc_ue_ctrl_notifier(rrc_ue_ctrl_notifier_),
    du_processor_ctrl_notifier(du_processor_ctrl_notifier_)
  {
    ue_ctxt.ue_index  = ue_index;
    ue_ctxt.ran_ue_id = ran_ue_id;
  }

  ngc_rrc_ue_pdu_notifier&           get_rrc_ue_pdu_notifier() { return rrc_ue_pdu_notifier; }
  ngc_rrc_ue_control_notifier&       get_rrc_ue_control_notifier() { return rrc_ue_ctrl_notifier; }
  ngc_du_processor_control_notifier& get_du_processor_control_notifier() { return du_processor_ctrl_notifier; }

  ue_index_t  get_ue_index() { return ue_ctxt.ue_index; }
  amf_ue_id_t get_amf_ue_id() { return ue_ctxt.amf_ue_id; }
  ran_ue_id_t get_ran_ue_id() { return ue_ctxt.ran_ue_id; }

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

class ngap_ue_manager
{
public:
  virtual ~ngap_ue_manager() = default;

  virtual ngap_ue*   add_ngap_ue(ue_index_t                         ue_index,
                                 ngc_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier,
                                 ngc_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier,
                                 ngc_du_processor_control_notifier& du_processor_ctrl_notifier) = 0;
  virtual void       remove_ngap_ue(ue_index_t ue_index)                                        = 0;
  virtual ngap_ue*   find_ngap_ue(ue_index_t ue_index)                                          = 0;
  virtual size_t     get_nof_ngap_ues()                                                         = 0;
  virtual void       set_amf_ue_id(ue_index_t ue_index, amf_ue_id_t amf_ue_id)                  = 0;
  virtual ue_index_t get_ue_index(ran_ue_id_t ran_ue_id)                                        = 0;
  virtual ue_index_t get_ue_index(amf_ue_id_t amf_ue_id)                                        = 0;
  virtual bool       contains(ue_index_t ue_index)                                              = 0;
};

} // namespace srs_cu_cp
} // namespace srsgnb
