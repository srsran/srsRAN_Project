#include "e2sm_kpm_impl.h"
#include "e2sm_kpm_report_service_impl.h"
#include "srsran/asn1/asn1_utils.h"

using namespace asn1::e2ap;
using namespace asn1::e2sm_kpm;
using namespace srsran;

e2sm_kpm_impl::e2sm_kpm_impl(srslog::basic_logger&    logger_,
                             e2sm_handler&            e2sm_packer_,
                             e2_du_metrics_interface& du_metrics_interface_) :
  logger(logger_), e2sm_packer(e2sm_packer_), du_metrics_interface(du_metrics_interface_)
{
}

bool e2sm_kpm_impl::action_supported(const asn1::e2ap::ri_caction_to_be_setup_item_s& ric_action)
{
  bool admit_action = false;
  auto action_def   = e2sm_packer.handle_packed_e2sm_kpm_action_definition(ric_action.ric_action_definition);

  switch (action_def.ric_style_type) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      admit_action = true;
      break;
    default:
      logger.info("Unknown RIC style type %i -> do not admit action %i (type %i)",
                  action_def.ric_style_type,
                  ric_action.ric_action_id,
                  ric_action.ric_action_type);
  }

  return admit_action;
}

std::unique_ptr<e2sm_report_service>
e2sm_kpm_impl::get_e2sm_report_service(const srsran::byte_buffer& action_definition)
{
  e2_sm_kpm_action_definition_s action_def = e2sm_packer.handle_packed_e2sm_kpm_action_definition(action_definition);
  uint32_t                      ric_style_type = action_def.ric_style_type;
  switch (ric_style_type) {
    case 1:
      return std::make_unique<e2sm_kpm_report_service_style1>(std::move(action_def), du_metrics_interface);
      break;
    case 2:
      return nullptr;
      break;
    case 3:
      return std::make_unique<e2sm_kpm_report_service_style3>(std::move(action_def), du_metrics_interface);
      break;
    case 4:
      return nullptr;
      break;
    case 5:
      return nullptr;
      break;
    default:
      logger.info("Unknown RIC style type %i", ric_style_type);
      return nullptr;
  }
}

e2sm_handler& e2sm_kpm_impl::get_e2sm_packer()
{
  return e2sm_packer;
}