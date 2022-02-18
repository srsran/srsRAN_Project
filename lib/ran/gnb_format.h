
#ifndef SRSGNB_GNB_FORMAT_H
#define SRSGNB_GNB_FORMAT_H

#include "srsgnb/ran/rnti.h"
#include "srsgnb/srslog/srslog.h"

// clang-format off
#define FMT_CTRL_PREFIX "CTRL"
#define FMT_DL_PREFIX   "DL  "
#define FMT_UL_PREFIX   "UL  "
#define FMT_UE_INDEX    "ueId={}"
#define FMT_RNTI        "{:#x}"
#define FMT_CC          "cc={}"
#define FMT_CH          "{}"
// clang-format on

namespace srsgnb {

template <size_t N>
const char* to_c_str(fmt::basic_memory_buffer<char, N>& mem_buffer)
{
  mem_buffer.push_back('\0');
  return mem_buffer.data();
}

template <typename... Args>
void log_proc(srslog::basic_logger& logger, du_ue_index_t ue_index, const char* fmt, Args&&... args)
{
  logger.info(FMT_CTRL_PREFIX " " FMT_UE_INDEX ": {}", ue_index, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void log_proc(srslog::basic_logger& logger, du_ue_index_t ue_index, rnti_t rnti, const char* fmt, Args&&... args)
{
  logger.info(FMT_CTRL_PREFIX " " FMT_UE_INDEX " " FMT_RNTI ": {}", ue_index, rnti, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void log_proc_started(srslog::basic_logger& logger, du_ue_index_t ue_index, rnti_t rnti, const char* proc_name)
{
  logger.info(FMT_CTRL_PREFIX " " FMT_UE_INDEX " " FMT_RNTI ": \"{}\" started.", ue_index, rnti, proc_name);
}

inline void log_proc_completed(srslog::basic_logger& logger, du_ue_index_t ue_index, rnti_t rnti, const char* proc_name)
{
  logger.info(FMT_CTRL_PREFIX " " FMT_UE_INDEX " " FMT_RNTI ": \"{}\" completed.", ue_index, rnti, proc_name);
}

template <typename... Args>
void log_proc_failure(srslog::basic_logger& logger,
                      du_ue_index_t         ue_index,
                      rnti_t                rnti,
                      const char*           proc_name,
                      const char*           cause_fmt,
                      Args&&... args)
{
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  logger.error(FMT_CTRL_PREFIX " " FMT_UE_INDEX " " FMT_RNTI "\"{}\" failed. Cause: {}",
               ue_index,
               rnti,
               proc_name,
               to_c_str(fmtbuf));
}

template <typename... Args>
void log_proc_event(srslog::basic_logger& logger,
                    du_ue_index_t         ue_index,
                    const char*           proc_name,
                    const char*           cause_fmt,
                    Args&&... args)
{
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  logger.info(FMT_CTRL_PREFIX " " FMT_UE_INDEX ": \"{}\" - {}", ue_index, proc_name, to_c_str(fmtbuf));
}

template <typename... Args>
void log_ul_pdu(srslog::basic_logger& logger,
                rnti_t                rnti,
                uint32_t              cc,
                const char*           ch,
                const char*           cause_fmt,
                Args&&... args)
{
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  logger.info(FMT_UL_PREFIX " " FMT_RNTI " " FMT_CC " " FMT_CH ": {}", rnti, cc, ch, to_c_str(fmtbuf));
}

template <typename... Args>
void log_ul_pdu(srslog::basic_logger& logger, rnti_t rnti, uint32_t cc, const char* cause_fmt, Args&&... args)
{
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  logger.info(FMT_UL_PREFIX " " FMT_RNTI " " FMT_CC ": {}", rnti, cc, to_c_str(fmtbuf));
}

template <typename... Args>
void log_ul_pdu(srslog::basic_logger& logger,
                du_ue_index_t         ue_index,
                rnti_t                rnti,
                uint32_t              cc,
                const char*           ch,
                const char*           cause_fmt,
                Args&&... args)
{
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  logger.info(FMT_UL_PREFIX " " FMT_UE_INDEX " " FMT_RNTI " " FMT_CC " " FMT_CH ": {}",
              ue_index,
              rnti,
              cc,
              ch,
              to_c_str(fmtbuf));
}

} // namespace srsgnb

#endif // SRSGNB_GNB_FORMAT_H
