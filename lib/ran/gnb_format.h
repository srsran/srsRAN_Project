
#ifndef SRSGNB_GNB_FORMAT_H
#define SRSGNB_GNB_FORMAT_H

#include "srsgnb/srslog/srslog.h"

// clang-format off
#define FMT_CTRL_PREFIX "CTRL "
#define FMT_DL_PREFIX   "DL   "
#define FMT_UL_PREFIX   "UL   "
#define FMT_UE_INDEX    "ueId={} "
#define FMT_RNTI        "rnti={:#x} "

#define LOG_START_PROC(ue_index, name) logger.info(FMT_CTRL_PREFIX UE_ID fmt);
// clang-format on

namespace srsgnb {

template <typename... Args>
void log_proc(srslog::basic_logger& logger, du_ue_index_t ue_index, const char* fmt, Args&&... args)
{
  logger.info(FMT_CTRL_PREFIX FMT_UE_INDEX ": {}", ue_index, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void log_proc(srslog::basic_logger& logger, du_ue_index_t ue_index, rnti_t rnti, const char* fmt, Args&&... args)
{
  logger.info(FMT_CTRL_PREFIX FMT_UE_INDEX FMT_RNTI ": {}", ue_index, rnti, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void log_proc_started(srslog::basic_logger& logger, du_ue_index_t ue_index, rnti_t rnti, const char* proc_name)
{
  logger.info(FMT_CTRL_PREFIX FMT_UE_INDEX FMT_RNTI ": \"{}\" started.", ue_index, rnti, proc_name);
}

template <typename... Args>
void log_proc_completed(srslog::basic_logger& logger, du_ue_index_t ue_index, rnti_t rnti, const char* proc_name)
{
  logger.info(FMT_CTRL_PREFIX FMT_UE_INDEX FMT_RNTI ": \"{}\" completed.", ue_index, rnti, proc_name);
}

} // namespace srsgnb

#endif // SRSGNB_GNB_FORMAT_H
