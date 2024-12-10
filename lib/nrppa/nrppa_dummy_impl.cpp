/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "nrppa_dummy_impl.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace srs_cu_cp;

nrppa_dummy_impl::nrppa_dummy_impl() : logger(srslog::fetch_basic_logger("NRPPA")) {}

// Note: For fwd declaration of member types, dtor cannot be trivial.
nrppa_dummy_impl::~nrppa_dummy_impl() {}

void nrppa_dummy_impl::handle_ue_measurement(ue_index_t ue_index, const cell_measurement_positioning_info& meas_result)
{
  logger.info("NRPPa messages are not supported");
}

void nrppa_dummy_impl::remove_ue_context(ue_index_t ue_index)
{
  logger.info("NRPPa messages are not supported");
}

void nrppa_dummy_impl::handle_new_nrppa_pdu(const byte_buffer& nrppa_pdu, std::optional<ue_index_t> ue_index)
{
  logger.info("NRPPa messages are not supported");
}
