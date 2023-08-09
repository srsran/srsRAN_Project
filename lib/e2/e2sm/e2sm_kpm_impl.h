/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2ap/e2sm_kpm.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2sm/e2sm.h"
#include <map>
namespace srsran {

class e2sm_kpm_impl : public e2sm_interface
{
public:
  // constructor takes logger as argument
  e2sm_kpm_impl(srslog::basic_logger&    logger_,
                e2sm_handler&            e2sm_packer_,
                e2_du_metrics_interface& du_metrics_interface_);

  e2sm_handler& get_e2sm_packer() override;

  bool action_supported(const asn1::e2ap::ri_caction_to_be_setup_item_s& ric_action) override;

  std::unique_ptr<e2sm_report_service> get_e2sm_report_service(const srsran::byte_buffer& action_definition) override;

  static bool supported_test_cond_type(asn1::e2sm_kpm::test_cond_type_c test_cond_type)
  {
    if (test_cond_type.type() == asn1::e2sm_kpm::test_cond_type_c::types_opts::cqi) {
      return true;
    } else if (test_cond_type.type() == asn1::e2sm_kpm::test_cond_type_c::types_opts::rsrp) {
      return true;
    } else if (test_cond_type.type() == asn1::e2sm_kpm::test_cond_type_c::types_opts::rsrq) {
      return true;
    } else {
      return false;
    }
  }

private:
  srslog::basic_logger&           logger;
  e2sm_handler&                   e2sm_packer;
  e2_du_metrics_interface&        du_metrics_interface;
};

} // namespace srsran
