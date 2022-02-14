
#include "srsgnb/rlc/rlc_factory.h"
#include "rlc_impl.h"

namespace srsgnb {

std::unique_ptr<rlc_ul_bearer> create_rlc_ul_bearer(du_ue_index_t ue_index, lcid_t lcid, rlc_ul_sdu_notifier& notifier)
{
  std::unique_ptr<rlc_ul_bearer> instance = std::make_unique<rlc_ul_um_bearer>(ue_index, lcid, notifier);
  return instance;
}

} // namespace srsgnb