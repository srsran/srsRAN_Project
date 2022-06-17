/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_DU_CONTEXT_H
#define SRSGNB_DU_CONTEXT_H

#include "srsgnb/adt/slot_array.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include <string>

namespace srsgnb {
namespace srs_cu_cp {

/// 36-bit identifying an NR Cell Id as specified in subclause 9.3.1.7 of 3GPP TS 38.413
struct nr_cell_identity {
  uint64_t packed;       /// All 36 bits combined
  uint32_t gnb_identity; /// 22 to 32 bits
  uint16_t cell_idenity; /// 4 to 14 bits
};

struct nr_cell_global_identity {
  uint16_t         mcc; /// 3 digits mobile country code
  uint16_t         mnc; /// 2 or 3 digits mobile network code
  nr_cell_identity nci; /// NR cell id
};

/// Basic cell system information provided by DU
struct du_sys_info {
  asn1::rrc_nr::mib_s mib;
  byte_buffer         packed_mib;

  asn1::rrc_nr::sib1_s sib1;
  byte_buffer          packed_sib1;

  const asn1::rrc_nr::serving_cell_cfg_common_sib_s& serv_cell_cfg_common() const
  {
    return sib1.serving_cell_cfg_common;
  }
};

struct du_cell_context {
  du_cell_index_t         cell_index; /// CU internal cell ID
  nr_cell_global_identity cgi;        /// global cell ID
  uint16_t                pci;        /// Physical cell ID
  du_sys_info             sys_info;   /// System information provided by DU
};

struct du_context {
  du_index_t                                    du_index;      /// Index assisgned by CU-CP
  uint64_t                                      id;            /// the gNB-DU-ID
  std::string                                   name = "none"; /// gNB-DU-Name
  slot_array<du_cell_context, MAX_NOF_DU_CELLS> cell_db; /// flattened version of served cells list provided by DU/F1AP
};

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_DU_CONTEXT_H
