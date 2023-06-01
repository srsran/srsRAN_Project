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

#pragma once

#include "srsran/adt/expected.h"
#include "srsran/fapi/validator_report.h"
#include <gtest/gtest.h>

namespace unittest {
/// Builds and returns a valid CRC.indication message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.8.
srsran::fapi::crc_indication_message build_valid_crc_indication();

/// Builds and returns a valid DL CSI-RS pdu. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.3.
srsran::fapi::dl_csi_rs_pdu build_valid_dl_csi_pdu();

/// Builds and returns a valid DL PDCCH PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.1.
srsran::fapi::dl_pdcch_pdu build_valid_dl_pdcch_pdu();

/// Builds and returns a valid DL PDSCH PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.2.
srsran::fapi::dl_pdsch_pdu build_valid_dl_pdsch_pdu();

/// Builds and returns a valid DL SSB pdu. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.4.
srsran::fapi::dl_ssb_pdu build_valid_dl_ssb_pdu();

/// Builds and returns a valid DL TTI request message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.
srsran::fapi::dl_tti_request_message build_valid_dl_tti_request();

/// Builds and returns a valid ERROR.indication message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.3.6.1.
srsran::fapi::error_indication_message build_valid_error_indication();

/// Builds and returns a valid OUT_OF_SYNC ERROR.indication message. Every parameter is within the range defined in
/// SCF-222 v4.0 Section 3.3.6.1.
srsran::fapi::error_indication_message build_valid_out_of_sync_error_indication();

/// Builds and returns a RACH.indication message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.11.
srsran::fapi::rach_indication_message build_valid_rach_indication();

/// Builds and returns a valid Rx_Data.indication message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.7.
srsran::fapi::rx_data_indication_message build_valid_rx_data_indication();

/// Builds and returns a valid SLOT.indication message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.1.
srsran::fapi::slot_indication_message build_valid_slot_indication();

/// Builds and returns a valid Tx_Data.request message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.6.
/// \note Payload pointer is set to \c nullptr.
srsran::fapi::tx_data_request_message build_valid_tx_data_request();

/// Builds and returns a valid transmission precoding and beamforming PDU. Every parameter is within the range defined
/// in SCF-222 v4.0 Section 3.4.2.5.
srsran::fapi::tx_precoding_and_beamforming_pdu build_valid_tx_precoding_and_beamforming_pdu();

/// Builds and returns a valid UCI.indication message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.9.
srsran::fapi::uci_indication_message build_valid_uci_indication();

/// Builds and returns a valid UCI PUSCH PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.9.1.
srsran::fapi::uci_pusch_pdu build_valid_uci_pusch_pdu();

/// Builds and returns a valid UCI PUCCH format 0/1 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.9.2.
srsran::fapi::uci_pucch_pdu_format_0_1 build_valid_uci_pucch_format01_pdu();

/// Builds and returns a valid UCI PUCCH format 2/3/4 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.9.3.
srsran::fapi::uci_pucch_pdu_format_2_3_4 build_valid_uci_pucch_format234_pdu();

/// Builds and returns a valid UL_DCI.request message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.4.
srsran::fapi::ul_dci_request_message build_valid_ul_dci_request();

/// Builds and returns a valid UL PRACH PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.1.
srsran::fapi::ul_prach_pdu build_valid_ul_prach_pdu();

/// Builds and returns a valid UL PUCCH format 0 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.3.
srsran::fapi::ul_pucch_pdu build_valid_ul_pucch_f0_pdu();

/// Builds and returns a valid UL PUCCH format 1 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.3.
srsran::fapi::ul_pucch_pdu build_valid_ul_pucch_f1_pdu();

/// Builds and returns a valid UL PUCCH format 2 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.3.
srsran::fapi::ul_pucch_pdu build_valid_ul_pucch_f2_pdu();

/// Builds and returns a valid UL PUCCH format 3 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.3.
srsran::fapi::ul_pucch_pdu build_valid_ul_pucch_f3_pdu();

/// Builds and returns a valid UL PUCCH format 4 PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.3.
srsran::fapi::ul_pucch_pdu build_valid_ul_pucch_f4_pdu();

/// Builds and returns a valid UL PUSCH PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.2.
srsran::fapi::ul_pusch_pdu build_valid_ul_pusch_pdu();

/// Builds and returns a valid UL_TTI.request. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.
srsran::fapi::ul_tti_request_message build_valid_ul_tti_request();

/// Google test helpers.
template <typename T>
struct pdu_field_data {
  std::string                            property;
  std::function<void(T& pdu, int value)> fun;
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const pdu_field_data<T>& arg)
{
  os << arg.property;
  return os;
};

struct test_case_data {
  unsigned value;
  bool     result;
};

using validator_report = srsran::fapi::validator_report;
using message_type_id  = srsran::fapi::message_type_id;
using error_report     = srsran::fapi::validator_report::error_report;

inline std::ostream& operator<<(std::ostream& os, const test_case_data& arg)
{
  os << "Value = " << std::to_string(arg.value) << ", Result = " << std::to_string(arg.result);
  return os;
};

template <typename T, typename F>
class validate_fapi_field
{
public:
  virtual ~validate_fapi_field() = default;

protected:
  void execute_test(pdu_field_data<T> property, test_case_data params, std::function<T()> builder, F validator)
  {
    T    pdu    = given_the_pdu(property, params, builder);
    bool result = when_executing_the_validation(pdu, validator);
    then_check_the_report(result, property.property, params.result);
  }

  void execute_test_pdu(pdu_field_data<T>         property,
                        test_case_data            params,
                        std::function<T()>        builder,
                        F                         validator,
                        srsran::fapi::dl_pdu_type pdu_type)
  {
    T    pdu    = given_the_pdu(property, params, builder);
    bool result = when_executing_the_validation(pdu, validator, pdu_type);
    then_check_the_report(result, property.property, params.result);
  }

private:
  T given_the_pdu(pdu_field_data<T> property, test_case_data params, std::function<T()> builder)
  {
    T pdu = builder();
    property.fun(pdu, params.value);
    return pdu;
  };

  virtual bool when_executing_the_validation(T pdu, F validator) { return true; };

  virtual bool when_executing_the_validation(T pdu, F validator, srsran::fapi::dl_pdu_type pdu_type) { return true; };

  void then_check_the_report(bool result, const std::string& property, bool expected_result)
  {
    EXPECT_EQ(result, expected_result);
    if (!result) {
      EXPECT_EQ(1U, report.reports.size());
      const auto& rep = report.reports.back();
      EXPECT_EQ(property, rep.property_name);
    } else {
      EXPECT_TRUE(report.reports.empty());
    }
  };

protected:
  validator_report report = {0, 0};
};

using fapi_error = srsran::error_type<validator_report>;

template <typename T>
class validate_fapi_message : public validate_fapi_field<T, std::function<fapi_error(T& pdu)>>
{
  using base = validate_fapi_field<T, std::function<fapi_error(T& pdu)>>;

public:
  void execute_test(pdu_field_data<T>                 property,
                    test_case_data                    params,
                    std::function<T()>                builder,
                    std::function<fapi_error(T& pdu)> validator,
                    message_type_id                   msg_type_id)
  {
    base::execute_test(property, params, builder, validator);

    check_message_params(params.result, msg_type_id);
  }

private:
  bool when_executing_the_validation(T pdu, std::function<fapi_error(T& pdu)> validator) override
  {
    fapi_error error = validator(pdu);
    if (error.is_error()) {
      base::report = error.error();
    }

    return !error.is_error();
  };

  void check_message_params(bool result, message_type_id msg_type_id) const
  {
    // In case of error, check the PDU type.
    if (base::report.reports.size()) {
      // Base class checks all the parameters but pdu_type, so it gets checked here. Base also checks that only one
      // error report exists, so the last (or the first) of them can be used for the check.
      const error_report& rep = base::report.reports.back();
      EXPECT_EQ(static_cast<unsigned>(msg_type_id), static_cast<unsigned>(rep.message_type));
    }
  }
};

template <typename T, typename U>
class validate_fapi_pdu
  : public validate_fapi_field<T, std::function<bool(T& pdu, srsran::fapi::validator_report& report)>>
{
  using base = validate_fapi_field<T, std::function<bool(T& pdu, srsran::fapi::validator_report& report)>>;

public:
  void execute_test(pdu_field_data<T>                                                   property,
                    test_case_data                                                      params,
                    std::function<T()>                                                  builder,
                    std::function<bool(T& pdu, srsran::fapi::validator_report& report)> validator,
                    message_type_id                                                     msg_type_id,
                    U                                                                   pdu_type)
  {
    base::execute_test(property, params, builder, validator);

    check_pdu_params(params.result, msg_type_id, pdu_type);
  }

private:
  bool when_executing_the_validation(T pdu, std::function<bool(T& pdu, validator_report& report)> validator) override
  {
    return validator(pdu, base::report);
  };

  void check_pdu_params(bool result, message_type_id msg_type_id, U pdu_type) const
  {
    // In case of error, check the PDU type.
    if (base::report.reports.size()) {
      // Base class checks all the parameters but pdu_type, so it gets checked here. Base also checks that only one
      // error report exists, so the last (or the first) of them can be used for the check.
      const auto& rep = base::report.reports.back();
      EXPECT_EQ(static_cast<unsigned>(pdu_type), static_cast<unsigned>(rep.pdu_type.value()));
      EXPECT_EQ(static_cast<unsigned>(msg_type_id), static_cast<unsigned>(rep.message_type));
    }
  }
};

template <typename T, typename U>
class validate_fapi_sub_pdu
  : public validate_fapi_field<T, std::function<bool(T& pdu, srsran::fapi::validator_report& report, U pdu_type)>>
{
  using base = validate_fapi_field<T, std::function<bool(T& pdu, srsran::fapi::validator_report& report, U pdu_type)>>;

public:
  void execute_test(pdu_field_data<T>                                                               property,
                    test_case_data                                                                  params,
                    std::function<T()>                                                              builder,
                    std::function<bool(T& pdu, srsran::fapi::validator_report& report, U pdu_type)> validator,
                    message_type_id                                                                 msg_type_id,
                    U                                                                               pdu_type)
  {
    base::execute_test_pdu(property, params, builder, validator, pdu_type);

    check_pdu_params(params.result, msg_type_id, pdu_type);
  }

private:
  virtual bool
  when_executing_the_validation(T                                                                 pdu,
                                std::function<bool(T& pdu, validator_report& report, U pdu_type)> validator,
                                srsran::fapi::dl_pdu_type                                         pdu_type) override
  {
    return validator(pdu, base::report, pdu_type);
  };

  void check_pdu_params(bool result, message_type_id msg_type_id, U pdu_type) const
  {
    // In case of error, check the PDU type.
    if (base::report.reports.size()) {
      // Base class checks all the parameters but pdu_type, so it gets checked here. Base also checks that only one
      // error report exists, so the last (or the first) of them can be used for the check.
      const auto& rep = base::report.reports.back();
      EXPECT_EQ(static_cast<unsigned>(pdu_type), static_cast<unsigned>(rep.pdu_type.value()));
      EXPECT_EQ(static_cast<unsigned>(msg_type_id), static_cast<unsigned>(rep.message_type));
    }
  }
};

} // namespace unittest
