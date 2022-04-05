
#ifndef SRSGNB_LIB_RADIO_UHD_RADIO_UHD_LOGGER_H
#define SRSGNB_LIB_RADIO_UHD_RADIO_UHD_LOGGER_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-override"
#include <uhd/utils/log.hpp>
#pragma GCC diagnostic pop

#ifdef UHD_LOG_INFO
#define Error(message) UHD_LOG_ERROR("UHD RF", message)
#define Warning(message) UHD_LOG_WARNING("UHD RF", message)
#define Info(message) UHD_LOG_INFO("UHD RF", message)
#define Debug(message) UHD_LOG_DEBUG("UHD RF", message)
#define Trace(message) UHD_LOG_TRACE("UHD RF", message)
#else
#define Error(message) UHD_LOG << message << std::endl
#define Warning(message) UHD_LOG << message << std::endl
#define Info(message) UHD_LOG << message << std::endl
#define Debug(message) UHD_LOG << message << std::endl
#define Trace(message) UHD_LOG << message << std::endl
#endif

#endif // SRSGNB_LIB_RADIO_UHD_RADIO_UHD_LOGGER_H
