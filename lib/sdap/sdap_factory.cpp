/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/sdap/sdap_factory.h"
#include "sdap_impl.h"
#include "sdap_packet_procedures_impl.h"
#include "srsgnb/sdap/sdap_entity_factory.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;
using namespace srs_cu_up;

/// Factories are at a low level point of abstraction, as such, they are the "only" (best effort) objects that depend on
/// concrete class implementations instead of interfaces, intrinsically giving them tight coupling to the objects being
/// created. Keeping this coupling in a single file, is the best, as the rest of the code can be kept decoupled.

std::unique_ptr<sdap_pdu_handler> srsgnb::srs_cu_up::create_sdap(sdap_sdu_rx_notifier& listener)
{
  auto ul_procedure =
      std::unique_ptr<sdap_packet_procedures>(new sdap_ul_packet_procedure(create_sdap_entity(), listener));

  return std::unique_ptr<sdap_pdu_handler>(new sdap_procedure_dispatcher(std::move(ul_procedure)));
}
