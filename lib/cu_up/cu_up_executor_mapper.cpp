/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/cu_up/cu_up_executor_pool.h"

using namespace srsran;

class cu_up_executor_mapper_impl final : public cu_up_executor_pool
{
  struct session_executor_context {
    task_executor& ctrl_exec;
    task_executor& ul_exec;
    task_executor& dl_exec;

    session_executor_context(task_executor& ctrl_exec_, task_executor& ul_exec_, task_executor& dl_exec_) :
      ctrl_exec(ctrl_exec_), ul_exec(ul_exec_), dl_exec(dl_exec_)
    {
    }
  };

public:
  class pdu_session_executor_mapper_impl final : public pdu_session_executor_mapper
  {
  public:
    pdu_session_executor_mapper_impl(cu_up_executor_mapper_impl& parent_, session_executor_context& ctxt_) :
      parent(parent_), ctxt(ctxt_)
    {
    }

    ~pdu_session_executor_mapper_impl() override { parent.deregister_pdu_session(ctxt); }

    task_executor& ctrl_executor() override { return ctxt.ctrl_exec; }
    task_executor& ul_pdu_executor() override { return ctxt.ul_exec; }
    task_executor& dl_pdu_executor() override { return ctxt.dl_exec; }

  private:
    cu_up_executor_mapper_impl& parent;
    session_executor_context&   ctxt;
  };

  cu_up_executor_mapper_impl(span<task_executor*> dl_executors,
                             span<task_executor*> ul_executors,
                             span<task_executor*> ctrl_executors)
  {
    srsran_assert(ctrl_executors.size() > 0, "At least one DL executor must be specified");
    if (dl_executors.empty()) {
      dl_executors = ctrl_executors;
    } else {
      srsran_assert(dl_executors.size() == ctrl_executors.size(),
                    "If specified, the number of DL executors must be equal to the number of control executors");
    }
    if (ul_executors.empty()) {
      ul_executors = ctrl_executors;
    } else {
      srsran_assert(ul_executors.size() == ctrl_executors.size(),
                    "If specified, the number of UL executors must be equal to the number of control executors");
    }

    for (unsigned i = 0; i != ctrl_executors.size(); ++i) {
      sessions.emplace_back(*ctrl_executors[i], *ul_executors[i], *dl_executors[i]);
    }
  }

  std::unique_ptr<pdu_session_executor_mapper> create_pdu_session() override
  {
    return std::make_unique<pdu_session_executor_mapper_impl>(
        *this, sessions[round_robin_index.fetch_add(1, std::memory_order_relaxed) % sessions.size()]);
  }

private:
  void deregister_pdu_session(session_executor_context& session_ctxt)
  {
    // do nothing.
  }

  std::vector<session_executor_context> sessions;

  std::atomic<uint32_t> round_robin_index{0};
};

std::unique_ptr<cu_up_executor_pool> srsran::make_cu_up_executor_mapper(span<task_executor*> dl_executors,
                                                                        span<task_executor*> ul_executors,
                                                                        span<task_executor*> ctrl_executors)
{
  return std::make_unique<cu_up_executor_mapper_impl>(dl_executors, ul_executors, ctrl_executors);
}
