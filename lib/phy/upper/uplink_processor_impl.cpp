/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "uplink_processor_impl.h"
#include "srsran/phy/support/prach_buffer.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/upper/unique_rx_softbuffer.h"
#include "srsran/phy/upper/upper_phy_rx_results_notifier.h"

using namespace srsran;

namespace {

/// \brief Adapts the PUSCH processor result notifier to the upper PHY receive results notifier.
///
/// It collects the Channel State Information (CSI), control and data decoding information from a PUSCH processor object
/// and notifies them through the upper PHY notification interface.
///
/// \remark The order of the calls matters. Method on_csi() must be called before on_uci() or on_sch(). Otherwise, an
/// assertion is triggered.
class pusch_processor_result_notifier_adaptor : public pusch_processor_result_notifier
{
public:
  /// \brief Creates a PUSCH processor result notifier adaptor.
  /// \param[in] notifier_   Upper physical layer result notifier.
  /// \param[in] rnti_       User RNTI.
  /// \param[in] slot_       Current slot.
  /// \param[in] harq_id_    User HARQ process identifier.
  /// \param[in] payload_    View to the data payload.
  pusch_processor_result_notifier_adaptor(upper_phy_rx_results_notifier& notifier_,
                                          uint16_t                       rnti_,
                                          slot_point                     slot_,
                                          unsigned                       harq_id_,
                                          span<const uint8_t>            payload_) :
    notifier(notifier_), rnti(to_rnti(rnti_)), slot(slot_), harq_id(to_harq_id(harq_id_)), payload(payload_)
  {
  }

  // See interface for documentation.
  void on_csi(const channel_state_information& csi_) override { csi.emplace(csi_); }

  // See interface for documentation.
  void on_uci(const pusch_processor_result_control& uci) override
  {
    srsran_assert(csi.has_value(), "Channel State Information is missing.");
    ul_pusch_results_control result;
    result.rnti = rnti;
    result.slot = slot;
    result.csi  = csi.value();

    if (!uci.harq_ack.payload.empty()) {
      result.harq_ack.emplace(uci.harq_ack);
    }

    if (!uci.csi_part1.payload.empty()) {
      result.csi1.emplace(uci.csi_part1);
    }

    if (!uci.csi_part2.payload.empty()) {
      result.csi2.emplace(uci.csi_part2);
    }

    notifier.on_new_pusch_results_control(result);
  }

  // See interface for documentation.
  void on_sch(const pusch_processor_result_data& sch) override
  {
    srsran_assert(csi.has_value(), "Channel State Information is missing.");
    ul_pusch_results_data result;
    result.rnti           = rnti;
    result.slot           = slot;
    result.csi            = csi.value();
    result.harq_id        = harq_id;
    result.decoder_result = sch.data;
    result.payload        = (sch.data.tb_crc_ok) ? payload : span<const uint8_t>();
    notifier.on_new_pusch_results_data(result);

    // Store the TB CRC okay flag.
    tb_crc_ok = sch.data.tb_crc_ok;
  }

  /// \brief Gets the TB CRC okay flag.
  /// \return True if the transport block CRC passed, otherwise False.
  bool get_tb_crc_ok() const { return tb_crc_ok; }

private:
  upper_phy_rx_results_notifier&      notifier;
  rnti_t                              rnti;
  slot_point                          slot;
  harq_id_t                           harq_id;
  span<const uint8_t>                 payload;
  optional<channel_state_information> csi;
  bool                                tb_crc_ok = false;
};

} // namespace

/// \brief Returns a PRACH detector slot configuration using the given PRACH buffer context.
static prach_detector::configuration get_prach_dectector_config_from_prach_context(const prach_buffer_context& context)
{
  prach_detector::configuration config;
  config.root_sequence_index   = context.root_sequence_index;
  config.format                = context.format;
  config.restricted_set        = context.restricted_set;
  config.zero_correlation_zone = context.zero_correlation_zone;
  config.start_preamble_index  = context.start_preamble_index;
  config.nof_preamble_indices  = context.nof_preamble_indices;
  config.ra_scs                = to_ra_subcarrier_spacing(context.pusch_scs);
  config.nof_rx_ports          = 1;

  return config;
}

uplink_processor_impl::uplink_processor_impl(std::unique_ptr<prach_detector>  prach_,
                                             std::unique_ptr<pusch_processor> pusch_proc_,
                                             std::unique_ptr<pucch_processor> pucch_proc_) :
  prach(std::move(prach_)), pusch_proc(std::move(pusch_proc_)), pucch_proc(std::move(pucch_proc_))
{
  srsran_assert(prach, "A valid PRACH detector must be provided");
  srsran_assert(pusch_proc, "A valid PUSCH processor must be provided");
  srsran_assert(pucch_proc, "A valid PUCCH processor must be provided");
}

void uplink_processor_impl::process_prach(upper_phy_rx_results_notifier& notifier,
                                          const prach_buffer&            buffer,
                                          const prach_buffer_context&    context)
{
  ul_prach_results ul_results;
  ul_results.context = context;
  ul_results.result  = prach->detect(buffer, get_prach_dectector_config_from_prach_context(context));

  // Notify the PRACH results.
  notifier.on_new_prach_results(ul_results);
}

void uplink_processor_impl::process_pusch(span<uint8_t>                      data,
                                          unique_rx_softbuffer               softbuffer,
                                          upper_phy_rx_results_notifier&     notifier,
                                          const resource_grid_reader&        grid,
                                          const uplink_processor::pusch_pdu& pdu)
{
  // Creates a PUSCH processor result notifier adaptor.
  pusch_processor_result_notifier_adaptor processor_notifier(notifier, pdu.pdu.rnti, pdu.pdu.slot, pdu.harq_id, data);

  // Processes PUSCH.
  pusch_proc->process(data, softbuffer.get(), processor_notifier, grid, pdu.pdu);

  // Release softbuffer if the TB CRC passed.
  if (processor_notifier.get_tb_crc_ok()) {
    softbuffer.release();
  }
}

void uplink_processor_impl::process_pucch(upper_phy_rx_results_notifier&     notifier,
                                          const resource_grid_reader&        grid,
                                          const uplink_processor::pucch_pdu& pdu)
{
  srsran_assert(pdu.context.format == pucch_format::FORMAT_1 || pdu.context.format == pucch_format::FORMAT_2,
                "Currently supporting PUCCH Format 1 and 2 only.");

  pucch_processor_result proc_result;
  // Process the PUCCH.
  switch (pdu.context.format) {
    case pucch_format::FORMAT_0:
      proc_result = pucch_proc->process(grid, pdu.format0);
      break;
    case pucch_format::FORMAT_1:
      proc_result = pucch_proc->process(grid, pdu.format1);
      break;
    case pucch_format::FORMAT_2:
      proc_result = pucch_proc->process(grid, pdu.format2);
      break;
    case pucch_format::FORMAT_3:
      proc_result = pucch_proc->process(grid, pdu.format3);
      break;
    case pucch_format::FORMAT_4:
      proc_result = pucch_proc->process(grid, pdu.format4);
      break;
    default:
      srsran_assert(0, "Invalid PUCCH format={}", pdu.context.format);
  }

  // Write the results.
  ul_pucch_results result;
  result.context          = pdu.context;
  result.processor_result = proc_result;

  // Notify the PUCCH results.
  notifier.on_new_pucch_results(result);
}
