/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "ue_ul_bearer_manager.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"

using namespace srsran;
using namespace srs_cu_cp;

namespace {

/// Handles uplink PDUs that are transported in the CCCH.
class f1c_srb0_ul_handler : public f1c_ul_bearer_handler
{
public:
  explicit f1c_srb0_ul_handler(f1ap_ul_ccch_notifier& ul_ccch_notifier_) : ul_ccch_notifier(ul_ccch_notifier_) {}

  // See interface for the documentation.
  void handle_ul_rrc_message(byte_buffer pdu) override { ul_ccch_notifier.on_ul_ccch_pdu(std::move(pdu)); }

private:
  f1ap_ul_ccch_notifier& ul_ccch_notifier;
};

/// Handles uplink PDUs that are transported in the DCCH.
class f1c_other_cu_bearer : public f1c_ul_bearer_handler
{
public:
  explicit f1c_other_cu_bearer(f1ap_ul_dcch_notifier& ul_dcch_notifier_) : ul_dcch_notifier(ul_dcch_notifier_) {}

  // See interface for the documentation.
  void handle_ul_rrc_message(byte_buffer pdu) override { ul_dcch_notifier.on_ul_dcch_pdu(std::move(pdu)); }

private:
  f1ap_ul_dcch_notifier& ul_dcch_notifier;
};

} // namespace

void ue_ul_bearer_manager::activate_srb0(f1ap_ul_ccch_notifier& f1ap_srb0_notifier)
{
  f1c_ul_bearers.emplace(srb_id_to_uint(srb_id_t::srb0), std::make_unique<f1c_srb0_ul_handler>(f1ap_srb0_notifier));
}

void ue_ul_bearer_manager::activate_srb1(f1ap_ul_dcch_notifier& f1ap_srb1_notifier)
{
  f1c_ul_bearers.emplace(srb_id_to_uint(srb_id_t::srb1), std::make_unique<f1c_other_cu_bearer>(f1ap_srb1_notifier));
}

void ue_ul_bearer_manager::activate_srb2(f1ap_ul_dcch_notifier& f1ap_srb2_notifier)
{
  f1c_ul_bearers.emplace(srb_id_to_uint(srb_id_t::srb2), std::make_unique<f1c_other_cu_bearer>(f1ap_srb2_notifier));
}
