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

#pragma once

#include "srsran/phy/upper/downlink_processor.h"
#include "srsran/phy/upper/signal_processors/prs/prs_generator_validator.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_configuration_validator.h"
#include "srsran/phy/upper/uplink_pdu_validator.h"
#include "srsran/phy/upper/uplink_processor.h"

namespace srsran {

/// Implements the uplink PDU validator for \ref uplink_processor_single_executor_impl.
class uplink_processor_validator_impl : public uplink_pdu_validator
{
public:
  /// Constructs an uplink PDU validator containing the validators for each channel.
  uplink_processor_validator_impl(std::unique_ptr<prach_detector_validator>              prach_,
                                  std::unique_ptr<pucch_pdu_validator>                   pucch_,
                                  std::unique_ptr<pusch_pdu_validator>                   pusch_,
                                  std::unique_ptr<srs_estimator_configuration_validator> srs_) :
    prach(std::move(prach_)), pucch(std::move(pucch_)), pusch(std::move(pusch_)), srs(std::move(srs_))
  {
    srsran_assert(prach, "Invalid PRACH detector validator.");
    srsran_assert(pucch, "Invalid PUCCH processor validator.");
    srsran_assert(pusch, "Invalid PUSCH processor validator.");
    srsran_assert(srs, "Invalid SRS estimator configuration validator.");
  }

  // See interface for documentation.
  error_type<std::string> is_valid(const prach_detector::configuration& config) const override
  {
    return prach->is_valid(config);
  }
  error_type<std::string> is_valid(const pucch_processor::format0_configuration& config) const override
  {
    return pucch->is_valid(config);
  }
  error_type<std::string> is_valid(const pucch_processor::format1_configuration& config) const override
  {
    return pucch->is_valid(config);
  }
  error_type<std::string> is_valid(const pucch_processor::format2_configuration& config) const override
  {
    return pucch->is_valid(config);
  }
  error_type<std::string> is_valid(const pucch_processor::format3_configuration& config) const override
  {
    return pucch->is_valid(config);
  }
  error_type<std::string> is_valid(const pucch_processor::format4_configuration& config) const override
  {
    return pucch->is_valid(config);
  }
  error_type<std::string> is_valid(const pusch_processor::pdu_t& config) const override
  {
    return pusch->is_valid(config);
  }
  error_type<std::string> is_valid(const srs_estimator_configuration& config) const override
  {
    return srs->is_valid(config);
  }

private:
  std::unique_ptr<prach_detector_validator>              prach;
  std::unique_ptr<pucch_pdu_validator>                   pucch;
  std::unique_ptr<pusch_pdu_validator>                   pusch;
  std::unique_ptr<srs_estimator_configuration_validator> srs;
};

/// Implements the downlink PDU validator for \ref downlink_processor_multi_executor_impl.
class downlink_processor_validator_impl : public downlink_pdu_validator
{
public:
  /// Constructs an downlink PDU validator containing the validators for each channel.
  downlink_processor_validator_impl(std::unique_ptr<ssb_pdu_validator>                  ssb_,
                                    std::unique_ptr<pdcch_pdu_validator>                pdcch_,
                                    std::unique_ptr<pdsch_pdu_validator>                pdsch_,
                                    std::unique_ptr<nzp_csi_rs_configuration_validator> csi_,
                                    std::unique_ptr<prs_generator_validator>            prs_) :
    ssb(std::move(ssb_)), pdcch(std::move(pdcch_)), pdsch(std::move(pdsch_)), csi(std::move(csi_)), prs(std::move(prs_))
  {
    srsran_assert(ssb, "Invalid SSB processor validator.");
    srsran_assert(pdcch, "Invalid PDCCH processor validator.");
    srsran_assert(pdsch, "Invalid PDSCH processor validator.");
    srsran_assert(csi, "Invalid NZP-CSI-RS processor validator.");
    srsran_assert(prs, "Invalid PRS generator validator.");
  }

  // See interface for documentation.
  error_type<std::string> is_valid(const ssb_processor::pdu_t& pdu) const override { return ssb->is_valid(pdu); }
  error_type<std::string> is_valid(const pdcch_processor::pdu_t& pdu) const override { return pdcch->is_valid(pdu); }
  error_type<std::string> is_valid(const pdsch_processor::pdu_t& pdu) const override { return pdsch->is_valid(pdu); }
  error_type<std::string> is_valid(const nzp_csi_rs_generator::config_t& config) const override
  {
    return csi->is_valid(config);
  }
  error_type<std::string> is_valid(const prs_generator_configuration& config) const override
  {
    return prs->is_valid(config);
  }

private:
  std::unique_ptr<ssb_pdu_validator>                  ssb;
  std::unique_ptr<pdcch_pdu_validator>                pdcch;
  std::unique_ptr<pdsch_pdu_validator>                pdsch;
  std::unique_ptr<nzp_csi_rs_configuration_validator> csi;
  std::unique_ptr<prs_generator_validator>            prs;
};

} // namespace srsran
