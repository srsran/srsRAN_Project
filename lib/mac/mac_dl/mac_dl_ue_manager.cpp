/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_dl_ue_manager.h"
#include "srsran/support/timers2.h"

using namespace srsran;

mac_dl_ue_manager::mac_dl_ue_manager(du_rnti_table& rnti_table_) : rnti_table(rnti_table_) {}

bool mac_dl_ue_manager::add_ue(const mac_ue_create_request_message& request,
                               std::vector<std::vector<uint8_t>>    dl_harq_buffers)
{
  std::lock_guard<std::mutex> lock(ue_mutex[request.ue_index]);
  return add_ue_nolock(request.ue_index, request.crnti, request.ul_ccch_msg, std::move(dl_harq_buffers)) and
         addmod_bearers_nolock(request.ue_index, request.bearers);
}

bool mac_dl_ue_manager::remove_ue(du_ue_index_t ue_index)
{
  std::lock_guard<std::mutex> lock(ue_mutex[ue_index]);
  if (not ue_db.contains(ue_index)) {
    return false;
  }
  ue_db.erase(ue_index);
  return true;
}

bool mac_dl_ue_manager::addmod_bearers(du_ue_index_t                            ue_index,
                                       span<const mac_logical_channel_to_setup> dl_logical_channels)
{
  std::lock_guard<std::mutex> lock(ue_mutex[ue_index]);
  return addmod_bearers_nolock(ue_index, dl_logical_channels);
}

bool mac_dl_ue_manager::remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids)
{
  std::lock_guard<std::mutex> lock(ue_mutex[ue_index]);
  if (not ue_db.contains(ue_index)) {
    return false;
  }
  auto& u = ue_db[ue_index];
  for (lcid_t lcid : lcids) {
    u.dl_bearers.erase(lcid);
  }
  return true;
}

bool mac_dl_ue_manager::add_ue_nolock(du_ue_index_t                     ue_index,
                                      rnti_t                            crnti,
                                      const byte_buffer*                ul_ccch_msg,
                                      std::vector<std::vector<uint8_t>> dl_harq_buffers)
{
  if (ue_db.contains(ue_index)) {
    return false;
  }
  ue_db.emplace(ue_index);
  auto& u        = ue_db[ue_index];
  u.ue_index     = ue_index;
  u.rnti         = crnti;
  u.harq_buffers = std::move(dl_harq_buffers);
  if (ul_ccch_msg != nullptr) {
    // Store Msg3.
    srsran_assert(
        ul_ccch_msg->length() >= UE_CON_RES_ID_LEN, "Invalid UL-CCCH message length ({} < 6)", ul_ccch_msg->length());
    std::copy(ul_ccch_msg->begin(), ul_ccch_msg->begin() + UE_CON_RES_ID_LEN, u.msg3_subpdu.begin());
  }
  return true;
}

bool mac_dl_ue_manager::addmod_bearers_nolock(du_ue_index_t                            ue_index,
                                              span<const mac_logical_channel_to_setup> dl_logical_channels)
{
  if (not ue_db.contains(ue_index)) {
    return false;
  }
  auto& u = ue_db[ue_index];
  for (const mac_logical_channel_to_setup& lc : dl_logical_channels) {
    u.dl_bearers.insert(lc.lcid, lc.dl_bearer);
  }
  return true;
}

ue_con_res_id_t mac_dl_ue_manager::get_con_res_id(rnti_t rnti)
{
  du_ue_index_t               ue_index = rnti_table[rnti];
  std::lock_guard<std::mutex> lock(ue_mutex[ue_index]);
  if (not ue_db.contains(ue_index)) {
    return {};
  }
  return ue_db[ue_index].msg3_subpdu;
}
