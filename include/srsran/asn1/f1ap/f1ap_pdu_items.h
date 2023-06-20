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

/*******************************************************************************
 *
 *                     3GPP TS ASN1 F1AP v17.4.1 (2023-04)
 *
 ******************************************************************************/

#pragma once

#include "f1ap_ies.h"

namespace asn1 {
namespace f1ap {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// Associated-SCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct associated_scell_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { associated_scell_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::associated_scell_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    associated_scell_item_s&       associated_scell_item() { return c; }
    const associated_scell_item_s& associated_scell_item() const { return c; }

  private:
    associated_scell_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Associated-SCell-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using associated_scell_list_l = bounded_array<protocol_ie_single_container_s<associated_scell_item_ies_o>, 32>;

// BAP-Header-Rewriting-Added-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bap_hdr_rewriting_added_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bap_hdr_rewriting_added_list_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::bap_hdr_rewriting_added_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bap_hdr_rewriting_added_list_item_s&       bap_hdr_rewriting_added_list_item() { return c; }
    const bap_hdr_rewriting_added_list_item_s& bap_hdr_rewriting_added_list_item() const { return c; }

  private:
    bap_hdr_rewriting_added_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BAP-Header-Rewriting-Added-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bap_hdr_rewriting_added_list_l =
    dyn_array<protocol_ie_single_container_s<bap_hdr_rewriting_added_list_item_ies_o>>;

// BAP-Header-Rewriting-Removed-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bap_hdr_rewriting_remd_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bap_hdr_rewriting_remd_list_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::bap_hdr_rewriting_remd_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bap_hdr_rewriting_remd_list_item_s&       bap_hdr_rewriting_remd_list_item() { return c; }
    const bap_hdr_rewriting_remd_list_item_s& bap_hdr_rewriting_remd_list_item() const { return c; }

  private:
    bap_hdr_rewriting_remd_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BAP-Header-Rewriting-Removed-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bap_hdr_rewriting_remd_list_l = dyn_array<protocol_ie_single_container_s<bap_hdr_rewriting_remd_list_item_ies_o>>;

// BH-Routing-Information-Added-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_routing_info_added_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_routing_info_added_list_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::bh_routing_info_added_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_routing_info_added_list_item_s&       bh_routing_info_added_list_item() { return c; }
    const bh_routing_info_added_list_item_s& bh_routing_info_added_list_item() const { return c; }

  private:
    bh_routing_info_added_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BH-Routing-Information-Removed-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_routing_info_remd_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_routing_info_remd_list_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::bh_routing_info_remd_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_routing_info_remd_list_item_s&       bh_routing_info_remd_list_item() { return c; }
    const bh_routing_info_remd_list_item_s& bh_routing_info_remd_list_item() const { return c; }

  private:
    bh_routing_info_remd_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BH-Routing-Information-Added-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_routing_info_added_list_l = dyn_array<protocol_ie_single_container_s<bh_routing_info_added_list_item_ies_o>>;

// BH-Routing-Information-Removed-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_routing_info_remd_list_l = dyn_array<protocol_ie_single_container_s<bh_routing_info_remd_list_item_ies_o>>;

// BHChannels-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_failed_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_failed_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::bh_chs_failed_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_failed_to_be_modified_item_s&       bh_chs_failed_to_be_modified_item() { return c; }
    const bh_chs_failed_to_be_modified_item_s& bh_chs_failed_to_be_modified_item() const { return c; }

  private:
    bh_chs_failed_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-FailedToBeModified-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_chs_failed_to_be_modified_list_l =
    dyn_array<protocol_ie_single_container_s<bh_chs_failed_to_be_modified_item_ies_o>>;

// BHChannels-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_failed_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_failed_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::bh_chs_failed_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_failed_to_be_setup_item_s&       bh_chs_failed_to_be_setup_item() { return c; }
    const bh_chs_failed_to_be_setup_item_s& bh_chs_failed_to_be_setup_item() const { return c; }

  private:
    bh_chs_failed_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-FailedToBeSetup-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_chs_failed_to_be_setup_list_l =
    dyn_array<protocol_ie_single_container_s<bh_chs_failed_to_be_setup_item_ies_o>>;

// BHChannels-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_failed_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_failed_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::bh_chs_failed_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_failed_to_be_setup_mod_item_s&       bh_chs_failed_to_be_setup_mod_item() { return c; }
    const bh_chs_failed_to_be_setup_mod_item_s& bh_chs_failed_to_be_setup_mod_item() const { return c; }

  private:
    bh_chs_failed_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-FailedToBeSetupMod-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_chs_failed_to_be_setup_mod_list_l =
    dyn_array<protocol_ie_single_container_s<bh_chs_failed_to_be_setup_mod_item_ies_o>>;

// BHChannels-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::bh_chs_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_modified_item_s&       bh_chs_modified_item() { return c; }
    const bh_chs_modified_item_s& bh_chs_modified_item() const { return c; }

  private:
    bh_chs_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-Modified-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_chs_modified_list_l = dyn_array<protocol_ie_single_container_s<bh_chs_modified_item_ies_o>>;

// BHChannels-Required-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_required_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_required_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::bh_chs_required_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_required_to_be_released_item_s&       bh_chs_required_to_be_released_item() { return c; }
    const bh_chs_required_to_be_released_item_s& bh_chs_required_to_be_released_item() const { return c; }

  private:
    bh_chs_required_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-Required-ToBeReleased-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES
// : IEsSetParam}
using bh_chs_required_to_be_released_list_l =
    dyn_array<protocol_ie_single_container_s<bh_chs_required_to_be_released_item_ies_o>>;

// BHChannels-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::bh_chs_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_setup_item_s&       bh_chs_setup_item() { return c; }
    const bh_chs_setup_item_s& bh_chs_setup_item() const { return c; }

  private:
    bh_chs_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-Setup-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using bh_chs_setup_list_l = dyn_array<protocol_ie_single_container_s<bh_chs_setup_item_ies_o>>;

// BHChannels-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::bh_chs_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_setup_mod_item_s&       bh_chs_setup_mod_item() { return c; }
    const bh_chs_setup_mod_item_s& bh_chs_setup_mod_item() const { return c; }

  private:
    bh_chs_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-SetupMod-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_chs_setup_mod_list_l = dyn_array<protocol_ie_single_container_s<bh_chs_setup_mod_item_ies_o>>;

// BHChannels-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::bh_chs_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_to_be_modified_item_s&       bh_chs_to_be_modified_item() { return c; }
    const bh_chs_to_be_modified_item_s& bh_chs_to_be_modified_item() const { return c; }

  private:
    bh_chs_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-ToBeModified-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_chs_to_be_modified_list_l = dyn_array<protocol_ie_single_container_s<bh_chs_to_be_modified_item_ies_o>>;

// BHChannels-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::bh_chs_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_to_be_released_item_s&       bh_chs_to_be_released_item() { return c; }
    const bh_chs_to_be_released_item_s& bh_chs_to_be_released_item() const { return c; }

  private:
    bh_chs_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-ToBeReleased-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_chs_to_be_released_list_l = dyn_array<protocol_ie_single_container_s<bh_chs_to_be_released_item_ies_o>>;

// BHChannels-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::bh_chs_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_to_be_setup_item_s&       bh_chs_to_be_setup_item() { return c; }
    const bh_chs_to_be_setup_item_s& bh_chs_to_be_setup_item() const { return c; }

  private:
    bh_chs_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-ToBeSetup-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_chs_to_be_setup_list_l = dyn_array<protocol_ie_single_container_s<bh_chs_to_be_setup_item_ies_o>>;

// BHChannels-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bh_chs_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { bh_chs_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::bh_chs_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    bh_chs_to_be_setup_mod_item_s&       bh_chs_to_be_setup_mod_item() { return c; }
    const bh_chs_to_be_setup_mod_item_s& bh_chs_to_be_setup_mod_item() const { return c; }

  private:
    bh_chs_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BHChannels-ToBeSetupMod-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using bh_chs_to_be_setup_mod_list_l = dyn_array<protocol_ie_single_container_s<bh_chs_to_be_setup_mod_item_ies_o>>;

// Broadcast-To-Be-Cancelled-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_to_be_cancelled_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_to_be_cancelled_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::broadcast_to_be_cancelled_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_to_be_cancelled_item_s&       broadcast_to_be_cancelled_item() { return c; }
    const broadcast_to_be_cancelled_item_s& broadcast_to_be_cancelled_item() const { return c; }

  private:
    broadcast_to_be_cancelled_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Broadcast-To-Be-Cancelled-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_to_be_cancelled_list_l =
    dyn_array<protocol_ie_single_container_s<broadcast_to_be_cancelled_list_item_ies_o>>;

// BroadcastMRBs-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_to_be_modified_item_s&       broadcast_m_rbs_to_be_modified_item() { return c; }
    const broadcast_m_rbs_to_be_modified_item_s& broadcast_m_rbs_to_be_modified_item() const { return c; }

  private:
    broadcast_m_rbs_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_to_be_released_item_s&       broadcast_m_rbs_to_be_released_item() { return c; }
    const broadcast_m_rbs_to_be_released_item_s& broadcast_m_rbs_to_be_released_item() const { return c; }

  private:
    broadcast_m_rbs_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_to_be_setup_mod_item_s&       broadcast_m_rbs_to_be_setup_mod_item() { return c; }
    const broadcast_m_rbs_to_be_setup_mod_item_s& broadcast_m_rbs_to_be_setup_mod_item() const { return c; }

  private:
    broadcast_m_rbs_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-ToBeModified-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_m_rbs_to_be_modified_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_to_be_modified_item_ies_o>, 32>;

// BroadcastMRBs-ToBeReleased-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_m_rbs_to_be_released_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_to_be_released_item_ies_o>, 32>;

// BroadcastMRBs-ToBeSetupMod-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_m_rbs_to_be_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_to_be_setup_mod_item_ies_o>, 32>;

// BroadcastMRBs-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_failed_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_failed_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_failed_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_failed_to_be_modified_item_s&       broadcast_m_rbs_failed_to_be_modified_item() { return c; }
    const broadcast_m_rbs_failed_to_be_modified_item_s& broadcast_m_rbs_failed_to_be_modified_item() const { return c; }

  private:
    broadcast_m_rbs_failed_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_failed_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_failed_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_failed_to_be_setup_mod_item_s&       broadcast_m_rbs_failed_to_be_setup_mod_item() { return c; }
    const broadcast_m_rbs_failed_to_be_setup_mod_item_s& broadcast_m_rbs_failed_to_be_setup_mod_item() const
    {
      return c;
    }

  private:
    broadcast_m_rbs_failed_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_modified_item_s&       broadcast_m_rbs_modified_item() { return c; }
    const broadcast_m_rbs_modified_item_s& broadcast_m_rbs_modified_item() const { return c; }

  private:
    broadcast_m_rbs_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_setup_mod_item_s&       broadcast_m_rbs_setup_mod_item() { return c; }
    const broadcast_m_rbs_setup_mod_item_s& broadcast_m_rbs_setup_mod_item() const { return c; }

  private:
    broadcast_m_rbs_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-FailedToBeModified-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_m_rbs_failed_to_be_modified_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_failed_to_be_modified_item_ies_o>, 32>;

// BroadcastMRBs-FailedToBeSetupMod-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_m_rbs_failed_to_be_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o>, 32>;

// BroadcastMRBs-Modified-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_m_rbs_modified_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_modified_item_ies_o>, 32>;

// BroadcastMRBs-SetupMod-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_m_rbs_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_setup_mod_item_ies_o>, 32>;

// BroadcastMRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_to_be_setup_item_s&       broadcast_m_rbs_to_be_setup_item() { return c; }
    const broadcast_m_rbs_to_be_setup_item_s& broadcast_m_rbs_to_be_setup_item() const { return c; }

  private:
    broadcast_m_rbs_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-ToBeSetup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_m_rbs_to_be_setup_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_to_be_setup_item_ies_o>, 32>;

// BroadcastMRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_failed_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_failed_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_failed_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_failed_to_be_setup_item_s&       broadcast_m_rbs_failed_to_be_setup_item() { return c; }
    const broadcast_m_rbs_failed_to_be_setup_item_s& broadcast_m_rbs_failed_to_be_setup_item() const { return c; }

  private:
    broadcast_m_rbs_failed_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_m_rbs_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { broadcast_m_rbs_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::broadcast_m_rbs_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    broadcast_m_rbs_setup_item_s&       broadcast_m_rbs_setup_item() { return c; }
    const broadcast_m_rbs_setup_item_s& broadcast_m_rbs_setup_item() const { return c; }

  private:
    broadcast_m_rbs_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// BroadcastMRBs-FailedToBeSetup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using broadcast_m_rbs_failed_to_be_setup_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_failed_to_be_setup_item_ies_o>, 32>;

// BroadcastMRBs-Setup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using broadcast_m_rbs_setup_list_l =
    bounded_array<protocol_ie_single_container_s<broadcast_m_rbs_setup_item_ies_o>, 32>;

// Candidate-SpCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct candidate_sp_cell_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { candidate_sp_cell_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::candidate_sp_cell_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    candidate_sp_cell_item_s&       candidate_sp_cell_item() { return c; }
    const candidate_sp_cell_item_s& candidate_sp_cell_item() const { return c; }

  private:
    candidate_sp_cell_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Candidate-SpCell-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using candidate_sp_cell_list_l = dyn_array<protocol_ie_single_container_s<candidate_sp_cell_item_ies_o>>;

// Cells-Broadcast-Cancelled-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_broadcast_cancelled_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_broadcast_cancelled_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::cells_broadcast_cancelled_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_broadcast_cancelled_item_s&       cells_broadcast_cancelled_item() { return c; }
    const cells_broadcast_cancelled_item_s& cells_broadcast_cancelled_item() const { return c; }

  private:
    cells_broadcast_cancelled_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-Broadcast-Cancelled-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using cells_broadcast_cancelled_list_l =
    dyn_array<protocol_ie_single_container_s<cells_broadcast_cancelled_list_item_ies_o>>;

// Cells-Broadcast-Completed-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_broadcast_completed_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_broadcast_completed_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::cells_broadcast_completed_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_broadcast_completed_item_s&       cells_broadcast_completed_item() { return c; }
    const cells_broadcast_completed_item_s& cells_broadcast_completed_item() const { return c; }

  private:
    cells_broadcast_completed_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-Broadcast-Completed-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using cells_broadcast_completed_list_l =
    dyn_array<protocol_ie_single_container_s<cells_broadcast_completed_list_item_ies_o>>;

// Cells-Failed-to-be-Activated-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_failed_to_be_activ_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_failed_to_be_activ_list_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::cells_failed_to_be_activ_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_failed_to_be_activ_list_item_s&       cells_failed_to_be_activ_list_item() { return c; }
    const cells_failed_to_be_activ_list_item_s& cells_failed_to_be_activ_list_item() const { return c; }

  private:
    cells_failed_to_be_activ_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-Failed-to-be-Activated-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using cells_failed_to_be_activ_list_l =
    dyn_array<protocol_ie_single_container_s<cells_failed_to_be_activ_list_item_ies_o>>;

// Cells-Status-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_status_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_status_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::cells_status_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_status_item_s&       cells_status_item() { return c; }
    const cells_status_item_s& cells_status_item() const { return c; }

  private:
    cells_status_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-Status-List ::= SEQUENCE (SIZE (0..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using cells_status_list_l = dyn_array<protocol_ie_single_container_s<cells_status_item_ies_o>>;

// Cells-To-Be-Broadcast-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_to_be_broadcast_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_to_be_broadcast_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::cells_to_be_broadcast_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_to_be_broadcast_item_s&       cells_to_be_broadcast_item() { return c; }
    const cells_to_be_broadcast_item_s& cells_to_be_broadcast_item() const { return c; }

  private:
    cells_to_be_broadcast_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-To-Be-Broadcast-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using cells_to_be_broadcast_list_l = dyn_array<protocol_ie_single_container_s<cells_to_be_broadcast_list_item_ies_o>>;

// Cells-to-be-Activated-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_to_be_activ_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_to_be_activ_list_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::cells_to_be_activ_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_to_be_activ_list_item_s&       cells_to_be_activ_list_item() { return c; }
    const cells_to_be_activ_list_item_s& cells_to_be_activ_list_item() const { return c; }

  private:
    cells_to_be_activ_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-to-be-Activated-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using cells_to_be_activ_list_l = dyn_array<protocol_ie_single_container_s<cells_to_be_activ_list_item_ies_o>>;

// Cells-to-be-Barred-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_to_be_barred_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_to_be_barred_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::cells_to_be_barred_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_to_be_barred_item_s&       cells_to_be_barred_item() { return c; }
    const cells_to_be_barred_item_s& cells_to_be_barred_item() const { return c; }

  private:
    cells_to_be_barred_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-to-be-Barred-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using cells_to_be_barred_list_l = dyn_array<protocol_ie_single_container_s<cells_to_be_barred_item_ies_o>>;

// Cells-to-be-Deactivated-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cells_to_be_deactiv_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { cells_to_be_deactiv_list_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::cells_to_be_deactiv_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cells_to_be_deactiv_list_item_s&       cells_to_be_deactiv_list_item() { return c; }
    const cells_to_be_deactiv_list_item_s& cells_to_be_deactiv_list_item() const { return c; }

  private:
    cells_to_be_deactiv_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Cells-to-be-Deactivated-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using cells_to_be_deactiv_list_l = dyn_array<protocol_ie_single_container_s<cells_to_be_deactiv_list_item_ies_o>>;

// DL-UP-TNL-Address-to-Update-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct dl_up_tnl_address_to_upd_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { dl_up_tnl_address_to_upd_list_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::dl_up_tnl_address_to_upd_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dl_up_tnl_address_to_upd_list_item_s&       dl_up_tnl_address_to_upd_list_item() { return c; }
    const dl_up_tnl_address_to_upd_list_item_s& dl_up_tnl_address_to_upd_list_item() const { return c; }

  private:
    dl_up_tnl_address_to_upd_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DL-UP-TNL-Address-to-Update-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using dl_up_tnl_address_to_upd_list_l =
    bounded_array<protocol_ie_single_container_s<dl_up_tnl_address_to_upd_list_item_ies_o>, 8>;

// DRB-Activity-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drb_activity_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drb_activity_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::drb_activity_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drb_activity_item_s&       drb_activity_item() { return c; }
    const drb_activity_item_s& drb_activity_item() const { return c; }

  private:
    drb_activity_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRB-Activity-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drb_activity_list_l = dyn_array<protocol_ie_single_container_s<drb_activity_item_ies_o>>;

// DRB-Notify-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drb_notify_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drb_notify_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::drb_notify_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drb_notify_item_s&       drb_notify_item() { return c; }
    const drb_notify_item_s& drb_notify_item() const { return c; }

  private:
    drb_notify_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRB-Notify-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drb_notify_list_l = dyn_array<protocol_ie_single_container_s<drb_notify_item_ies_o>>;

// DRBs-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_failed_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_failed_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::drbs_failed_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_failed_to_be_modified_item_s&       drbs_failed_to_be_modified_item() { return c; }
    const drbs_failed_to_be_modified_item_s& drbs_failed_to_be_modified_item() const { return c; }

  private:
    drbs_failed_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-FailedToBeModified-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using drbs_failed_to_be_modified_list_l =
    dyn_array<protocol_ie_single_container_s<drbs_failed_to_be_modified_item_ies_o>>;

// DRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_failed_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_failed_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::drbs_failed_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_failed_to_be_setup_item_s&       drbs_failed_to_be_setup_item() { return c; }
    const drbs_failed_to_be_setup_item_s& drbs_failed_to_be_setup_item() const { return c; }

  private:
    drbs_failed_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-FailedToBeSetup-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_failed_to_be_setup_list_l = dyn_array<protocol_ie_single_container_s<drbs_failed_to_be_setup_item_ies_o>>;

// DRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_failed_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_failed_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::drbs_failed_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_failed_to_be_setup_mod_item_s&       drbs_failed_to_be_setup_mod_item() { return c; }
    const drbs_failed_to_be_setup_mod_item_s& drbs_failed_to_be_setup_mod_item() const { return c; }

  private:
    drbs_failed_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-FailedToBeSetupMod-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using drbs_failed_to_be_setup_mod_list_l =
    dyn_array<protocol_ie_single_container_s<drbs_failed_to_be_setup_mod_item_ies_o>>;

// DRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::drbs_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_modified_item_s&       drbs_modified_item() { return c; }
    const drbs_modified_item_s& drbs_modified_item() const { return c; }

  private:
    drbs_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-Modified-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_modified_list_l = dyn_array<protocol_ie_single_container_s<drbs_modified_item_ies_o>>;

// DRBs-ModifiedConf-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_modified_conf_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_modified_conf_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::drbs_modified_conf_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_modified_conf_item_s&       drbs_modified_conf_item() { return c; }
    const drbs_modified_conf_item_s& drbs_modified_conf_item() const { return c; }

  private:
    drbs_modified_conf_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-ModifiedConf-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_modified_conf_list_l = dyn_array<protocol_ie_single_container_s<drbs_modified_conf_item_ies_o>>;

// DRBs-Required-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_required_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_required_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::drbs_required_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_required_to_be_modified_item_s&       drbs_required_to_be_modified_item() { return c; }
    const drbs_required_to_be_modified_item_s& drbs_required_to_be_modified_item() const { return c; }

  private:
    drbs_required_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-Required-ToBeModified-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using drbs_required_to_be_modified_list_l =
    dyn_array<protocol_ie_single_container_s<drbs_required_to_be_modified_item_ies_o>>;

// DRBs-Required-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_required_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_required_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::drbs_required_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_required_to_be_released_item_s&       drbs_required_to_be_released_item() { return c; }
    const drbs_required_to_be_released_item_s& drbs_required_to_be_released_item() const { return c; }

  private:
    drbs_required_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-Required-ToBeReleased-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using drbs_required_to_be_released_list_l =
    dyn_array<protocol_ie_single_container_s<drbs_required_to_be_released_item_ies_o>>;

// DRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::drbs_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_setup_item_s&       drbs_setup_item() { return c; }
    const drbs_setup_item_s& drbs_setup_item() const { return c; }

  private:
    drbs_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-Setup-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_setup_list_l = dyn_array<protocol_ie_single_container_s<drbs_setup_item_ies_o>>;

// DRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::drbs_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_setup_mod_item_s&       drbs_setup_mod_item() { return c; }
    const drbs_setup_mod_item_s& drbs_setup_mod_item() const { return c; }

  private:
    drbs_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-SetupMod-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_setup_mod_list_l = dyn_array<protocol_ie_single_container_s<drbs_setup_mod_item_ies_o>>;

// DRBs-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::drbs_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_to_be_modified_item_s&       drbs_to_be_modified_item() { return c; }
    const drbs_to_be_modified_item_s& drbs_to_be_modified_item() const { return c; }

  private:
    drbs_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-ToBeModified-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_to_be_modified_list_l = dyn_array<protocol_ie_single_container_s<drbs_to_be_modified_item_ies_o>>;

// DRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::drbs_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_to_be_released_item_s&       drbs_to_be_released_item() { return c; }
    const drbs_to_be_released_item_s& drbs_to_be_released_item() const { return c; }

  private:
    drbs_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-ToBeReleased-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_to_be_released_list_l = dyn_array<protocol_ie_single_container_s<drbs_to_be_released_item_ies_o>>;

// DRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::drbs_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_to_be_setup_item_s&       drbs_to_be_setup_item() { return c; }
    const drbs_to_be_setup_item_s& drbs_to_be_setup_item() const { return c; }

  private:
    drbs_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-ToBeSetup-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_to_be_setup_list_l = dyn_array<protocol_ie_single_container_s<drbs_to_be_setup_item_ies_o>>;

// DRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct drbs_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { drbs_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::drbs_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    drbs_to_be_setup_mod_item_s&       drbs_to_be_setup_mod_item() { return c; }
    const drbs_to_be_setup_mod_item_s& drbs_to_be_setup_mod_item() const { return c; }

  private:
    drbs_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// DRBs-ToBeSetupMod-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using drbs_to_be_setup_mod_list_l = dyn_array<protocol_ie_single_container_s<drbs_to_be_setup_mod_item_ies_o>>;

// Dedicated-SIDelivery-NeededUE-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ded_si_delivery_needed_ue_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ded_si_delivery_needed_ue_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ded_si_delivery_needed_ue_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ded_si_delivery_needed_ue_item_s&       ded_si_delivery_needed_ue_item() { return c; }
    const ded_si_delivery_needed_ue_item_s& ded_si_delivery_needed_ue_item() const { return c; }

  private:
    ded_si_delivery_needed_ue_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Dedicated-SIDelivery-NeededUE-List ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using ded_si_delivery_needed_ue_list_l =
    dyn_array<protocol_ie_single_container_s<ded_si_delivery_needed_ue_item_ies_o>>;

// GNB-CU-TNL-Association-Failed-To-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_tnl_assoc_failed_to_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_tnl_assoc_failed_to_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::gnb_cu_tnl_assoc_failed_to_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_cu_tnl_assoc_failed_to_setup_item_s&       gnb_cu_tnl_assoc_failed_to_setup_item() { return c; }
    const gnb_cu_tnl_assoc_failed_to_setup_item_s& gnb_cu_tnl_assoc_failed_to_setup_item() const { return c; }

  private:
    gnb_cu_tnl_assoc_failed_to_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-TNL-Association-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_tnl_assoc_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_tnl_assoc_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::gnb_cu_tnl_assoc_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_cu_tnl_assoc_setup_item_s&       gnb_cu_tnl_assoc_setup_item() { return c; }
    const gnb_cu_tnl_assoc_setup_item_s& gnb_cu_tnl_assoc_setup_item() const { return c; }

  private:
    gnb_cu_tnl_assoc_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-TNL-Association-To-Add-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_tnl_assoc_to_add_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_tnl_assoc_to_add_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::gnb_cu_tnl_assoc_to_add_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_cu_tnl_assoc_to_add_item_s&       gnb_cu_tnl_assoc_to_add_item() { return c; }
    const gnb_cu_tnl_assoc_to_add_item_s& gnb_cu_tnl_assoc_to_add_item() const { return c; }

  private:
    gnb_cu_tnl_assoc_to_add_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-TNL-Association-To-Remove-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_tnl_assoc_to_rem_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_tnl_assoc_to_rem_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::gnb_cu_tnl_assoc_to_rem_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_cu_tnl_assoc_to_rem_item_s&       gnb_cu_tnl_assoc_to_rem_item() { return c; }
    const gnb_cu_tnl_assoc_to_rem_item_s& gnb_cu_tnl_assoc_to_rem_item() const { return c; }

  private:
    gnb_cu_tnl_assoc_to_rem_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-TNL-Association-To-Update-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_tnl_assoc_to_upd_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_cu_tnl_assoc_to_upd_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::gnb_cu_tnl_assoc_to_upd_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_cu_tnl_assoc_to_upd_item_s&       gnb_cu_tnl_assoc_to_upd_item() { return c; }
    const gnb_cu_tnl_assoc_to_upd_item_s& gnb_cu_tnl_assoc_to_upd_item() const { return c; }

  private:
    gnb_cu_tnl_assoc_to_upd_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-DU-Served-Cells-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_served_cells_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_du_served_cells_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::gnb_du_served_cells_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_du_served_cells_item_s&       gnb_du_served_cells_item() { return c; }
    const gnb_du_served_cells_item_s& gnb_du_served_cells_item() const { return c; }

  private:
    gnb_du_served_cells_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-DU-TNL-Association-To-Remove-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_tnl_assoc_to_rem_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { gnb_du_tnl_assoc_to_rem_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::gnb_du_tnl_assoc_to_rem_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    gnb_du_tnl_assoc_to_rem_item_s&       gnb_du_tnl_assoc_to_rem_item() { return c; }
    const gnb_du_tnl_assoc_to_rem_item_s& gnb_du_tnl_assoc_to_rem_item() const { return c; }

  private:
    gnb_du_tnl_assoc_to_rem_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// IAB-Allocated-TNL-Address-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct iab_allocated_tnl_address_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { iab_allocated_tnl_address_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::iab_allocated_tnl_address_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    iab_allocated_tnl_address_item_s&       iab_allocated_tnl_address_item() { return c; }
    const iab_allocated_tnl_address_item_s& iab_allocated_tnl_address_item() const { return c; }

  private:
    iab_allocated_tnl_address_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// IAB-TNL-Addresses-To-Remove-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct iab_tnl_addresses_to_rem_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { iab_tnl_addresses_to_rem_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::iab_tnl_addresses_to_rem_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    iab_tnl_addresses_to_rem_item_s&       iab_tnl_addresses_to_rem_item() { return c; }
    const iab_tnl_addresses_to_rem_item_s& iab_tnl_addresses_to_rem_item() const { return c; }

  private:
    iab_tnl_addresses_to_rem_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MC-PagingCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct mc_paging_cell_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { mc_paging_cell_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::mc_paging_cell_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mc_paging_cell_item_s&       mc_paging_cell_item() { return c; }
    const mc_paging_cell_item_s& mc_paging_cell_item() const { return c; }

  private:
    mc_paging_cell_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastF1UContext-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_f1_u_context_failed_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_f1_u_context_failed_to_be_setup_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::multicast_f1_u_context_failed_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_f1_u_context_failed_to_be_setup_item_s& multicast_f1_u_context_failed_to_be_setup_item() { return c; }
    const multicast_f1_u_context_failed_to_be_setup_item_s& multicast_f1_u_context_failed_to_be_setup_item() const
    {
      return c;
    }

  private:
    multicast_f1_u_context_failed_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastF1UContext-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_f1_u_context_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_f1_u_context_setup_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::multicast_f1_u_context_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_f1_u_context_setup_item_s&       multicast_f1_u_context_setup_item() { return c; }
    const multicast_f1_u_context_setup_item_s& multicast_f1_u_context_setup_item() const { return c; }

  private:
    multicast_f1_u_context_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastF1UContext-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_f1_u_context_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_f1_u_context_to_be_setup_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::multicast_f1_u_context_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_f1_u_context_to_be_setup_item_s&       multicast_f1_u_context_to_be_setup_item() { return c; }
    const multicast_f1_u_context_to_be_setup_item_s& multicast_f1_u_context_to_be_setup_item() const { return c; }

  private:
    multicast_f1_u_context_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_failed_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_failed_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::multicast_m_rbs_failed_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_failed_to_be_modified_item_s&       multicast_m_rbs_failed_to_be_modified_item() { return c; }
    const multicast_m_rbs_failed_to_be_modified_item_s& multicast_m_rbs_failed_to_be_modified_item() const { return c; }

  private:
    multicast_m_rbs_failed_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_failed_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_failed_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::multicast_m_rbs_failed_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_failed_to_be_setup_item_s&       multicast_m_rbs_failed_to_be_setup_item() { return c; }
    const multicast_m_rbs_failed_to_be_setup_item_s& multicast_m_rbs_failed_to_be_setup_item() const { return c; }

  private:
    multicast_m_rbs_failed_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_failed_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_failed_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::multicast_m_rbs_failed_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_failed_to_be_setup_mod_item_s&       multicast_m_rbs_failed_to_be_setup_mod_item() { return c; }
    const multicast_m_rbs_failed_to_be_setup_mod_item_s& multicast_m_rbs_failed_to_be_setup_mod_item() const
    {
      return c;
    }

  private:
    multicast_m_rbs_failed_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::multicast_m_rbs_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_modified_item_s&       multicast_m_rbs_modified_item() { return c; }
    const multicast_m_rbs_modified_item_s& multicast_m_rbs_modified_item() const { return c; }

  private:
    multicast_m_rbs_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::multicast_m_rbs_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_setup_item_s&       multicast_m_rbs_setup_item() { return c; }
    const multicast_m_rbs_setup_item_s& multicast_m_rbs_setup_item() const { return c; }

  private:
    multicast_m_rbs_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::multicast_m_rbs_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_setup_mod_item_s&       multicast_m_rbs_setup_mod_item() { return c; }
    const multicast_m_rbs_setup_mod_item_s& multicast_m_rbs_setup_mod_item() const { return c; }

  private:
    multicast_m_rbs_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::multicast_m_rbs_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_to_be_modified_item_s&       multicast_m_rbs_to_be_modified_item() { return c; }
    const multicast_m_rbs_to_be_modified_item_s& multicast_m_rbs_to_be_modified_item() const { return c; }

  private:
    multicast_m_rbs_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::multicast_m_rbs_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_to_be_released_item_s&       multicast_m_rbs_to_be_released_item() { return c; }
    const multicast_m_rbs_to_be_released_item_s& multicast_m_rbs_to_be_released_item() const { return c; }

  private:
    multicast_m_rbs_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::multicast_m_rbs_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_to_be_setup_item_s&       multicast_m_rbs_to_be_setup_item() { return c; }
    const multicast_m_rbs_to_be_setup_item_s& multicast_m_rbs_to_be_setup_item() const { return c; }

  private:
    multicast_m_rbs_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// MulticastMRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_m_rbs_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { multicast_m_rbs_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::multicast_m_rbs_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    multicast_m_rbs_to_be_setup_mod_item_s&       multicast_m_rbs_to_be_setup_mod_item() { return c; }
    const multicast_m_rbs_to_be_setup_mod_item_s& multicast_m_rbs_to_be_setup_mod_item() const { return c; }

  private:
    multicast_m_rbs_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// NR-CGI-List-For-Restart-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct nr_cgi_list_for_restart_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { nr_cgi_list_for_restart_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::nr_cgi_list_for_restart_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    nr_cgi_list_for_restart_item_s&       nr_cgi_list_for_restart_item() { return c; }
    const nr_cgi_list_for_restart_item_s& nr_cgi_list_for_restart_item() const { return c; }

  private:
    nr_cgi_list_for_restart_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Neighbour-Cell-Information-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct neighbour_cell_info_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { neighbour_cell_info_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::neighbour_cell_info_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    neighbour_cell_info_item_s&       neighbour_cell_info_item() { return c; }
    const neighbour_cell_info_item_s& neighbour_cell_info_item() const { return c; }

  private:
    neighbour_cell_info_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PWS-Failed-NR-CGI-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pws_failed_nr_cgi_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { pws_failed_nr_cgi_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::pws_failed_nr_cgi_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pws_failed_nr_cgi_item_s&       pws_failed_nr_cgi_item() { return c; }
    const pws_failed_nr_cgi_item_s& pws_failed_nr_cgi_item() const { return c; }

  private:
    pws_failed_nr_cgi_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// PagingCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct paging_cell_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { paging_cell_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::paging_cell_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    paging_cell_item_s&       paging_cell_item() { return c; }
    const paging_cell_item_s& paging_cell_item() const { return c; }

  private:
    paging_cell_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Potential-SpCell-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct potential_sp_cell_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { potential_sp_cell_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::potential_sp_cell_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    potential_sp_cell_item_s&       potential_sp_cell_item() { return c; }
    const potential_sp_cell_item_s& potential_sp_cell_item() const { return c; }

  private:
    potential_sp_cell_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Protected-EUTRA-Resources-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct protected_eutra_res_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { protected_eutra_res_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::protected_eutra_res_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    protected_eutra_res_item_s&       protected_eutra_res_item() { return c; }
    const protected_eutra_res_item_s& protected_eutra_res_item() const { return c; }

  private:
    protected_eutra_res_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SCell-FailedtoSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct scell_failedto_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { scell_failedto_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::scell_failedto_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scell_failedto_setup_item_s&       scell_failedto_setup_item() { return c; }
    const scell_failedto_setup_item_s& scell_failedto_setup_item() const { return c; }

  private:
    scell_failedto_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SCell-FailedtoSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct scell_failedto_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { scell_failedto_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::scell_failedto_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scell_failedto_setup_mod_item_s&       scell_failedto_setup_mod_item() { return c; }
    const scell_failedto_setup_mod_item_s& scell_failedto_setup_mod_item() const { return c; }

  private:
    scell_failedto_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SCell-ToBeRemoved-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct scell_to_be_remd_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { scell_to_be_remd_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::scell_to_be_remd_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scell_to_be_remd_item_s&       scell_to_be_remd_item() { return c; }
    const scell_to_be_remd_item_s& scell_to_be_remd_item() const { return c; }

  private:
    scell_to_be_remd_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SCell-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct scell_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { scell_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::scell_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scell_to_be_setup_item_s&       scell_to_be_setup_item() { return c; }
    const scell_to_be_setup_item_s& scell_to_be_setup_item() const { return c; }

  private:
    scell_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SCell-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct scell_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { scell_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::scell_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scell_to_be_setup_mod_item_s&       scell_to_be_setup_mod_item() { return c; }
    const scell_to_be_setup_mod_item_s& scell_to_be_setup_mod_item() const { return c; }

  private:
    scell_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-FailedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_failed_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_failed_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::sl_drbs_failed_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_failed_to_be_modified_item_s&       sl_drbs_failed_to_be_modified_item() { return c; }
    const sl_drbs_failed_to_be_modified_item_s& sl_drbs_failed_to_be_modified_item() const { return c; }

  private:
    sl_drbs_failed_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_failed_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_failed_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::sl_drbs_failed_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_failed_to_be_setup_item_s&       sl_drbs_failed_to_be_setup_item() { return c; }
    const sl_drbs_failed_to_be_setup_item_s& sl_drbs_failed_to_be_setup_item() const { return c; }

  private:
    sl_drbs_failed_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_failed_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_failed_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::sl_drbs_failed_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_failed_to_be_setup_mod_item_s&       sl_drbs_failed_to_be_setup_mod_item() { return c; }
    const sl_drbs_failed_to_be_setup_mod_item_s& sl_drbs_failed_to_be_setup_mod_item() const { return c; }

  private:
    sl_drbs_failed_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::sl_drbs_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_modified_item_s&       sl_drbs_modified_item() { return c; }
    const sl_drbs_modified_item_s& sl_drbs_modified_item() const { return c; }

  private:
    sl_drbs_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-ModifiedConf-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_modified_conf_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_modified_conf_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::sl_drbs_modified_conf_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_modified_conf_item_s&       sl_drbs_modified_conf_item() { return c; }
    const sl_drbs_modified_conf_item_s& sl_drbs_modified_conf_item() const { return c; }

  private:
    sl_drbs_modified_conf_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-Required-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_required_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_required_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::sl_drbs_required_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_required_to_be_modified_item_s&       sl_drbs_required_to_be_modified_item() { return c; }
    const sl_drbs_required_to_be_modified_item_s& sl_drbs_required_to_be_modified_item() const { return c; }

  private:
    sl_drbs_required_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-Required-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_required_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_required_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::sl_drbs_required_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_required_to_be_released_item_s&       sl_drbs_required_to_be_released_item() { return c; }
    const sl_drbs_required_to_be_released_item_s& sl_drbs_required_to_be_released_item() const { return c; }

  private:
    sl_drbs_required_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::sl_drbs_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_setup_item_s&       sl_drbs_setup_item() { return c; }
    const sl_drbs_setup_item_s& sl_drbs_setup_item() const { return c; }

  private:
    sl_drbs_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::sl_drbs_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_setup_mod_item_s&       sl_drbs_setup_mod_item() { return c; }
    const sl_drbs_setup_mod_item_s& sl_drbs_setup_mod_item() const { return c; }

  private:
    sl_drbs_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-ToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::sl_drbs_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_to_be_modified_item_s&       sl_drbs_to_be_modified_item() { return c; }
    const sl_drbs_to_be_modified_item_s& sl_drbs_to_be_modified_item() const { return c; }

  private:
    sl_drbs_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::sl_drbs_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_to_be_released_item_s&       sl_drbs_to_be_released_item() { return c; }
    const sl_drbs_to_be_released_item_s& sl_drbs_to_be_released_item() const { return c; }

  private:
    sl_drbs_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::sl_drbs_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_to_be_setup_item_s&       sl_drbs_to_be_setup_item() { return c; }
    const sl_drbs_to_be_setup_item_s& sl_drbs_to_be_setup_item() const { return c; }

  private:
    sl_drbs_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SLDRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sl_drbs_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { sl_drbs_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::sl_drbs_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sl_drbs_to_be_setup_mod_item_s&       sl_drbs_to_be_setup_mod_item() { return c; }
    const sl_drbs_to_be_setup_mod_item_s& sl_drbs_to_be_setup_mod_item() const { return c; }

  private:
    sl_drbs_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-FailedToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_failed_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_failed_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::srbs_failed_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_failed_to_be_setup_item_s&       srbs_failed_to_be_setup_item() { return c; }
    const srbs_failed_to_be_setup_item_s& srbs_failed_to_be_setup_item() const { return c; }

  private:
    srbs_failed_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-FailedToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_failed_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_failed_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::srbs_failed_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_failed_to_be_setup_mod_item_s&       srbs_failed_to_be_setup_mod_item() { return c; }
    const srbs_failed_to_be_setup_mod_item_s& srbs_failed_to_be_setup_mod_item() const { return c; }

  private:
    srbs_failed_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-Modified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::srbs_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_modified_item_s&       srbs_modified_item() { return c; }
    const srbs_modified_item_s& srbs_modified_item() const { return c; }

  private:
    srbs_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-Required-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_required_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_required_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::srbs_required_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_required_to_be_released_item_s&       srbs_required_to_be_released_item() { return c; }
    const srbs_required_to_be_released_item_s& srbs_required_to_be_released_item() const { return c; }

  private:
    srbs_required_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::srbs_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_setup_item_s&       srbs_setup_item() { return c; }
    const srbs_setup_item_s& srbs_setup_item() const { return c; }

  private:
    srbs_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-SetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::srbs_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_setup_mod_item_s&       srbs_setup_mod_item() { return c; }
    const srbs_setup_mod_item_s& srbs_setup_mod_item() const { return c; }

  private:
    srbs_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::srbs_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_to_be_released_item_s&       srbs_to_be_released_item() { return c; }
    const srbs_to_be_released_item_s& srbs_to_be_released_item() const { return c; }

  private:
    srbs_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::srbs_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_to_be_setup_item_s&       srbs_to_be_setup_item() { return c; }
    const srbs_to_be_setup_item_s& srbs_to_be_setup_item() const { return c; }

  private:
    srbs_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// SRBs-ToBeSetupMod-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srbs_to_be_setup_mod_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { srbs_to_be_setup_mod_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::srbs_to_be_setup_mod_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    srbs_to_be_setup_mod_item_s&       srbs_to_be_setup_mod_item() { return c; }
    const srbs_to_be_setup_mod_item_s& srbs_to_be_setup_mod_item() const { return c; }

  private:
    srbs_to_be_setup_mod_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Served-Cells-To-Add-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct served_cells_to_add_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { served_cells_to_add_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::served_cells_to_add_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    served_cells_to_add_item_s&       served_cells_to_add_item() { return c; }
    const served_cells_to_add_item_s& served_cells_to_add_item() const { return c; }

  private:
    served_cells_to_add_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Served-Cells-To-Delete-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct served_cells_to_delete_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { served_cells_to_delete_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::served_cells_to_delete_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    served_cells_to_delete_item_s&       served_cells_to_delete_item() { return c; }
    const served_cells_to_delete_item_s& served_cells_to_delete_item() const { return c; }

  private:
    served_cells_to_delete_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// Served-Cells-To-Modify-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct served_cells_to_modify_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { served_cells_to_modify_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::served_cells_to_modify_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    served_cells_to_modify_item_s&       served_cells_to_modify_item() { return c; }
    const served_cells_to_modify_item_s& served_cells_to_modify_item() const { return c; }

  private:
    served_cells_to_modify_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ServingCellMO-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct serving_cell_mo_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { serving_cell_mo_list_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::serving_cell_mo_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    serving_cell_mo_list_item_s&       serving_cell_mo_list_item() { return c; }
    const serving_cell_mo_list_item_s& serving_cell_mo_list_item() const { return c; }

  private:
    serving_cell_mo_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-MulticastMRBs-ConfirmedToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_multicast_m_rbs_confirmed_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ue_multicast_m_rbs_confirmed_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_multicast_m_rbs_confirmed_to_be_modified_item_s& ue_multicast_m_rbs_confirmed_to_be_modified_item() { return c; }
    const ue_multicast_m_rbs_confirmed_to_be_modified_item_s& ue_multicast_m_rbs_confirmed_to_be_modified_item() const
    {
      return c;
    }

  private:
    ue_multicast_m_rbs_confirmed_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-MulticastMRBs-RequiredToBeModified-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_multicast_m_rbs_required_to_be_modified_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_multicast_m_rbs_required_to_be_modified_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ue_multicast_m_rbs_required_to_be_modified_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_multicast_m_rbs_required_to_be_modified_item_s& ue_multicast_m_rbs_required_to_be_modified_item() { return c; }
    const ue_multicast_m_rbs_required_to_be_modified_item_s& ue_multicast_m_rbs_required_to_be_modified_item() const
    {
      return c;
    }

  private:
    ue_multicast_m_rbs_required_to_be_modified_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-MulticastMRBs-RequiredToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_multicast_m_rbs_required_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_multicast_m_rbs_required_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ue_multicast_m_rbs_required_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_multicast_m_rbs_required_to_be_released_item_s& ue_multicast_m_rbs_required_to_be_released_item() { return c; }
    const ue_multicast_m_rbs_required_to_be_released_item_s& ue_multicast_m_rbs_required_to_be_released_item() const
    {
      return c;
    }

  private:
    ue_multicast_m_rbs_required_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-MulticastMRBs-Setup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_multicast_m_rbs_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_multicast_m_rbs_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ue_multicast_m_rbs_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_multicast_m_rbs_setup_item_s&       ue_multicast_m_rbs_setup_item() { return c; }
    const ue_multicast_m_rbs_setup_item_s& ue_multicast_m_rbs_setup_item() const { return c; }

  private:
    ue_multicast_m_rbs_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-MulticastMRBs-ToBeReleased-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_multicast_m_rbs_to_be_released_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_multicast_m_rbs_to_be_released_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ue_multicast_m_rbs_to_be_released_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_multicast_m_rbs_to_be_released_item_s&       ue_multicast_m_rbs_to_be_released_item() { return c; }
    const ue_multicast_m_rbs_to_be_released_item_s& ue_multicast_m_rbs_to_be_released_item() const { return c; }

  private:
    ue_multicast_m_rbs_to_be_released_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-MulticastMRBs-ToBeSetup-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_multicast_m_rbs_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_multicast_m_rbs_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ue_multicast_m_rbs_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_multicast_m_rbs_to_be_setup_item_s&       ue_multicast_m_rbs_to_be_setup_item() { return c; }
    const ue_multicast_m_rbs_to_be_setup_item_s& ue_multicast_m_rbs_to_be_setup_item() const { return c; }

  private:
    ue_multicast_m_rbs_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UE-MulticastMRBs-ToBeSetup-atModify-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_multicast_m_rbs_to_be_setup_at_modify_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ue_multicast_m_rbs_to_be_setup_at_modify_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_multicast_m_rbs_to_be_setup_at_modify_item_s&       ue_multicast_m_rbs_to_be_setup_at_modify_item() { return c; }
    const ue_multicast_m_rbs_to_be_setup_at_modify_item_s& ue_multicast_m_rbs_to_be_setup_at_modify_item() const
    {
      return c;
    }

  private:
    ue_multicast_m_rbs_to_be_setup_at_modify_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UEIdentity-List-For-Paging-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_id_list_for_paging_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_id_list_for_paging_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ue_id_list_for_paging_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ue_id_list_for_paging_item_s&       ue_id_list_for_paging_item() { return c; }
    const ue_id_list_for_paging_item_s& ue_id_list_for_paging_item() const { return c; }

  private:
    ue_id_list_for_paging_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UL-UP-TNL-Address-to-Update-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ul_up_tnl_address_to_upd_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ul_up_tnl_address_to_upd_list_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ul_up_tnl_address_to_upd_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ul_up_tnl_address_to_upd_list_item_s&       ul_up_tnl_address_to_upd_list_item() { return c; }
    const ul_up_tnl_address_to_upd_list_item_s& ul_up_tnl_address_to_upd_list_item() const { return c; }

  private:
    ul_up_tnl_address_to_upd_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// UL-UP-TNL-Information-to-Update-List-ItemIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ul_up_tnl_info_to_upd_list_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ul_up_tnl_info_to_upd_list_item, nulltype } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    types       type() const { return types::ul_up_tnl_info_to_upd_list_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ul_up_tnl_info_to_upd_list_item_s&       ul_up_tnl_info_to_upd_list_item() { return c; }
    const ul_up_tnl_info_to_upd_list_item_s& ul_up_tnl_info_to_upd_list_item() const { return c; }

  private:
    ul_up_tnl_info_to_upd_list_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GNB-CU-TNL-Association-Failed-To-Setup-List ::= SEQUENCE (SIZE (1..32)) OF
// ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using gnb_cu_tnl_assoc_failed_to_setup_list_l =
    bounded_array<protocol_ie_single_container_s<gnb_cu_tnl_assoc_failed_to_setup_item_ies_o>, 32>;

// GNB-CU-TNL-Association-Setup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using gnb_cu_tnl_assoc_setup_list_l =
    bounded_array<protocol_ie_single_container_s<gnb_cu_tnl_assoc_setup_item_ies_o>, 32>;

// GNB-CU-TNL-Association-To-Add-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using gnb_cu_tnl_assoc_to_add_list_l =
    bounded_array<protocol_ie_single_container_s<gnb_cu_tnl_assoc_to_add_item_ies_o>, 32>;

// GNB-CU-TNL-Association-To-Remove-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using gnb_cu_tnl_assoc_to_rem_list_l =
    bounded_array<protocol_ie_single_container_s<gnb_cu_tnl_assoc_to_rem_item_ies_o>, 32>;

// GNB-CU-TNL-Association-To-Update-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using gnb_cu_tnl_assoc_to_upd_list_l =
    bounded_array<protocol_ie_single_container_s<gnb_cu_tnl_assoc_to_upd_item_ies_o>, 32>;

// GNB-DU-Served-Cells-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using gnb_du_served_cells_list_l = dyn_array<protocol_ie_single_container_s<gnb_du_served_cells_item_ies_o>>;

// GNB-DU-TNL-Association-To-Remove-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using gnb_du_tnl_assoc_to_rem_list_l =
    bounded_array<protocol_ie_single_container_s<gnb_du_tnl_assoc_to_rem_item_ies_o>, 32>;

// IAB-Allocated-TNL-Address-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using iab_allocated_tnl_address_list_l =
    dyn_array<protocol_ie_single_container_s<iab_allocated_tnl_address_list_item_ies_o>>;

// IAB-TNL-Addresses-To-Remove-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using iab_tnl_addresses_to_rem_list_l = dyn_array<protocol_ie_single_container_s<iab_tnl_addresses_to_rem_item_ies_o>>;

// MulticastF1UContext-FailedToBeSetup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES
// : IEsSetParam}
using multicast_f1_u_context_failed_to_be_setup_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_f1_u_context_failed_to_be_setup_item_ies_o>, 32>;

// MulticastF1UContext-Setup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_f1_u_context_setup_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_f1_u_context_setup_item_ies_o>, 32>;

// MulticastF1UContext-ToBeSetup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_f1_u_context_to_be_setup_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_f1_u_context_to_be_setup_item_ies_o>, 32>;

// MulticastMRBs-FailedToBeModified-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_m_rbs_failed_to_be_modified_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_failed_to_be_modified_item_ies_o>, 32>;

// MulticastMRBs-FailedToBeSetup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_m_rbs_failed_to_be_setup_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_failed_to_be_setup_item_ies_o>, 32>;

// MulticastMRBs-FailedToBeSetupMod-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_m_rbs_failed_to_be_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_failed_to_be_setup_mod_item_ies_o>, 32>;

// MulticastMRBs-Modified-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_m_rbs_modified_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_modified_item_ies_o>, 32>;

// MulticastMRBs-Setup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using multicast_m_rbs_setup_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_setup_item_ies_o>, 32>;

// MulticastMRBs-SetupMod-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_m_rbs_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_setup_mod_item_ies_o>, 32>;

// MulticastMRBs-ToBeModified-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_m_rbs_to_be_modified_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_to_be_modified_item_ies_o>, 32>;

// MulticastMRBs-ToBeReleased-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_m_rbs_to_be_released_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_to_be_released_item_ies_o>, 32>;

// MulticastMRBs-ToBeSetup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_m_rbs_to_be_setup_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_to_be_setup_item_ies_o>, 32>;

// MulticastMRBs-ToBeSetupMod-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using multicast_m_rbs_to_be_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<multicast_m_rbs_to_be_setup_mod_item_ies_o>, 32>;

// NR-CGI-List-For-Restart-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using nr_cgi_list_for_restart_list_l =
    dyn_array<protocol_ie_single_container_s<nr_cgi_list_for_restart_list_item_ies_o>>;

// Neighbour-Cell-Information-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using neighbour_cell_info_list_l = dyn_array<protocol_ie_single_container_s<neighbour_cell_info_item_ies_o>>;

// PWS-Failed-NR-CGI-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using pws_failed_nr_cgi_list_l = dyn_array<protocol_ie_single_container_s<pws_failed_nr_cgi_list_item_ies_o>>;

// Potential-SpCell-List ::= SEQUENCE (SIZE (0..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using potential_sp_cell_list_l = dyn_array<protocol_ie_single_container_s<potential_sp_cell_item_ies_o>>;

// Protected-EUTRA-Resources-List ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using protected_eutra_res_list_l = dyn_array<protocol_ie_single_container_s<protected_eutra_res_item_ies_o>>;

// SCell-FailedtoSetup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using scell_failedto_setup_list_l = bounded_array<protocol_ie_single_container_s<scell_failedto_setup_item_ies_o>, 32>;

// SCell-FailedtoSetupMod-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using scell_failedto_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<scell_failedto_setup_mod_item_ies_o>, 32>;

// SCell-ToBeRemoved-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using scell_to_be_remd_list_l = bounded_array<protocol_ie_single_container_s<scell_to_be_remd_item_ies_o>, 32>;

// SCell-ToBeSetup-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using scell_to_be_setup_list_l = bounded_array<protocol_ie_single_container_s<scell_to_be_setup_item_ies_o>, 32>;

// SCell-ToBeSetupMod-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using scell_to_be_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<scell_to_be_setup_mod_item_ies_o>, 32>;

// SLDRBs-FailedToBeModified-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using sl_drbs_failed_to_be_modified_list_l =
    dyn_array<protocol_ie_single_container_s<sl_drbs_failed_to_be_modified_item_ies_o>>;

// SLDRBs-FailedToBeSetup-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using sl_drbs_failed_to_be_setup_list_l =
    dyn_array<protocol_ie_single_container_s<sl_drbs_failed_to_be_setup_item_ies_o>>;

// SLDRBs-FailedToBeSetupMod-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using sl_drbs_failed_to_be_setup_mod_list_l =
    dyn_array<protocol_ie_single_container_s<sl_drbs_failed_to_be_setup_mod_item_ies_o>>;

// SLDRBs-Modified-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using sl_drbs_modified_list_l = dyn_array<protocol_ie_single_container_s<sl_drbs_modified_item_ies_o>>;

// SLDRBs-ModifiedConf-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using sl_drbs_modified_conf_list_l = dyn_array<protocol_ie_single_container_s<sl_drbs_modified_conf_item_ies_o>>;

// SLDRBs-Required-ToBeModified-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using sl_drbs_required_to_be_modified_list_l =
    dyn_array<protocol_ie_single_container_s<sl_drbs_required_to_be_modified_item_ies_o>>;

// SLDRBs-Required-ToBeReleased-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using sl_drbs_required_to_be_released_list_l =
    dyn_array<protocol_ie_single_container_s<sl_drbs_required_to_be_released_item_ies_o>>;

// SLDRBs-Setup-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using sl_drbs_setup_list_l = dyn_array<protocol_ie_single_container_s<sl_drbs_setup_item_ies_o>>;

// SLDRBs-SetupMod-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using sl_drbs_setup_mod_list_l = dyn_array<protocol_ie_single_container_s<sl_drbs_setup_mod_item_ies_o>>;

// SLDRBs-ToBeModified-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using sl_drbs_to_be_modified_list_l = dyn_array<protocol_ie_single_container_s<sl_drbs_to_be_modified_item_ies_o>>;

// SLDRBs-ToBeReleased-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using sl_drbs_to_be_released_list_l = dyn_array<protocol_ie_single_container_s<sl_drbs_to_be_released_item_ies_o>>;

// SLDRBs-ToBeSetup-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using sl_drbs_to_be_setup_list_l = dyn_array<protocol_ie_single_container_s<sl_drbs_to_be_setup_item_ies_o>>;

// SLDRBs-ToBeSetupMod-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using sl_drbs_to_be_setup_mod_list_l = dyn_array<protocol_ie_single_container_s<sl_drbs_to_be_setup_mod_item_ies_o>>;

// SRBs-FailedToBeSetup-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_failed_to_be_setup_list_l =
    bounded_array<protocol_ie_single_container_s<srbs_failed_to_be_setup_item_ies_o>, 8>;

// SRBs-FailedToBeSetupMod-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using srbs_failed_to_be_setup_mod_list_l =
    bounded_array<protocol_ie_single_container_s<srbs_failed_to_be_setup_mod_item_ies_o>, 8>;

// SRBs-Modified-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_modified_list_l = bounded_array<protocol_ie_single_container_s<srbs_modified_item_ies_o>, 8>;

// SRBs-Required-ToBeReleased-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using srbs_required_to_be_released_list_l =
    bounded_array<protocol_ie_single_container_s<srbs_required_to_be_released_item_ies_o>, 8>;

// SRBs-Setup-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_setup_list_l = bounded_array<protocol_ie_single_container_s<srbs_setup_item_ies_o>, 8>;

// SRBs-SetupMod-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_setup_mod_list_l = bounded_array<protocol_ie_single_container_s<srbs_setup_mod_item_ies_o>, 8>;

// SRBs-ToBeReleased-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_to_be_released_list_l = bounded_array<protocol_ie_single_container_s<srbs_to_be_released_item_ies_o>, 8>;

// SRBs-ToBeSetup-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_to_be_setup_list_l = bounded_array<protocol_ie_single_container_s<srbs_to_be_setup_item_ies_o>, 8>;

// SRBs-ToBeSetupMod-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using srbs_to_be_setup_mod_list_l = bounded_array<protocol_ie_single_container_s<srbs_to_be_setup_mod_item_ies_o>, 8>;

// Served-Cells-To-Add-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using served_cells_to_add_list_l = dyn_array<protocol_ie_single_container_s<served_cells_to_add_item_ies_o>>;

// Served-Cells-To-Delete-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using served_cells_to_delete_list_l = dyn_array<protocol_ie_single_container_s<served_cells_to_delete_item_ies_o>>;

// Served-Cells-To-Modify-List ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using served_cells_to_modify_list_l = dyn_array<protocol_ie_single_container_s<served_cells_to_modify_item_ies_o>>;

// ServingCellMO-List ::= SEQUENCE (SIZE (1..16)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using serving_cell_mo_list_l = bounded_array<protocol_ie_single_container_s<serving_cell_mo_list_item_ies_o>, 16>;

// UE-MulticastMRBs-ConfirmedToBeModified-List ::= SEQUENCE (SIZE (1..64)) OF
// ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using ue_multicast_m_rbs_confirmed_to_be_modified_list_l =
    dyn_array<protocol_ie_single_container_s<ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o>>;

// UE-MulticastMRBs-RequiredToBeModified-List ::= SEQUENCE (SIZE (1..64)) OF
// ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using ue_multicast_m_rbs_required_to_be_modified_list_l =
    dyn_array<protocol_ie_single_container_s<ue_multicast_m_rbs_required_to_be_modified_item_ies_o>>;

// UE-MulticastMRBs-RequiredToBeReleased-List ::= SEQUENCE (SIZE (1..64)) OF
// ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES : IEsSetParam}
using ue_multicast_m_rbs_required_to_be_released_list_l =
    dyn_array<protocol_ie_single_container_s<ue_multicast_m_rbs_required_to_be_released_item_ies_o>>;

// UE-MulticastMRBs-Setup-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using ue_multicast_m_rbs_setup_list_l = dyn_array<protocol_ie_single_container_s<ue_multicast_m_rbs_setup_item_ies_o>>;

// UE-MulticastMRBs-ToBeReleased-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using ue_multicast_m_rbs_to_be_released_list_l =
    dyn_array<protocol_ie_single_container_s<ue_multicast_m_rbs_to_be_released_item_ies_o>>;

// UE-MulticastMRBs-ToBeSetup-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using ue_multicast_m_rbs_to_be_setup_list_l =
    dyn_array<protocol_ie_single_container_s<ue_multicast_m_rbs_to_be_setup_item_ies_o>>;

// UE-MulticastMRBs-ToBeSetup-atModify-List ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES
// : IEsSetParam}
using ue_multicast_m_rbs_to_be_setup_at_modify_list_l =
    dyn_array<protocol_ie_single_container_s<ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o>>;

// UEIdentity-List-For-Paging-List ::= SEQUENCE (SIZE (1..4096)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using ue_id_list_for_paging_list_l = dyn_array<protocol_ie_single_container_s<ue_id_list_for_paging_item_ies_o>>;

// UL-UP-TNL-Address-to-Update-List ::= SEQUENCE (SIZE (1..8)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using ul_up_tnl_address_to_upd_list_l =
    bounded_array<protocol_ie_single_container_s<ul_up_tnl_address_to_upd_list_item_ies_o>, 8>;

// UL-UP-TNL-Information-to-Update-List ::= SEQUENCE (SIZE (1..32678)) OF ProtocolIE-SingleContainer{F1AP-PROTOCOL-IES :
// IEsSetParam}
using ul_up_tnl_info_to_upd_list_l = dyn_array<protocol_ie_single_container_s<ul_up_tnl_info_to_upd_list_item_ies_o>>;

} // namespace f1ap
} // namespace asn1

extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::associated_scell_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bap_hdr_rewriting_added_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bap_hdr_rewriting_remd_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_routing_info_added_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_routing_info_remd_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_failed_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_failed_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_failed_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_required_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::bh_chs_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_to_be_cancelled_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_m_rbs_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_m_rbs_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_m_rbs_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::f1ap::broadcast_m_rbs_failed_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::f1ap::broadcast_m_rbs_failed_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_m_rbs_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_m_rbs_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_m_rbs_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_m_rbs_failed_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::broadcast_m_rbs_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::candidate_sp_cell_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_broadcast_cancelled_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_broadcast_completed_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_failed_to_be_activ_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_status_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_broadcast_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_activ_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_barred_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::cells_to_be_deactiv_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::dl_up_tnl_address_to_upd_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drb_activity_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drb_notify_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_failed_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_failed_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_failed_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_modified_conf_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_required_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_required_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ded_si_delivery_needed_ue_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_cu_tnl_assoc_failed_to_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_cu_tnl_assoc_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_cu_tnl_assoc_to_add_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_cu_tnl_assoc_to_rem_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_cu_tnl_assoc_to_upd_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_du_served_cells_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_du_tnl_assoc_to_rem_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::iab_allocated_tnl_address_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::iab_tnl_addresses_to_rem_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::f1ap::multicast_f1_u_context_failed_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_f1_u_context_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_f1_u_context_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::f1ap::multicast_m_rbs_failed_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_m_rbs_failed_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::f1ap::multicast_m_rbs_failed_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_m_rbs_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_m_rbs_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_m_rbs_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_m_rbs_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_m_rbs_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_m_rbs_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::multicast_m_rbs_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::nr_cgi_list_for_restart_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::neighbour_cell_info_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::pws_failed_nr_cgi_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::potential_sp_cell_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::protected_eutra_res_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::scell_failedto_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::scell_failedto_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::scell_to_be_remd_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::scell_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::scell_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_failed_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_failed_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_failed_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_modified_conf_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_required_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_required_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::sl_drbs_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_failed_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_failed_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_required_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::srbs_to_be_setup_mod_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::served_cells_to_add_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::served_cells_to_delete_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::served_cells_to_modify_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::serving_cell_mo_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::f1ap::ue_multicast_m_rbs_confirmed_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::f1ap::ue_multicast_m_rbs_required_to_be_modified_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::f1ap::ue_multicast_m_rbs_required_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ue_multicast_m_rbs_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ue_multicast_m_rbs_to_be_released_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ue_multicast_m_rbs_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<
    asn1::f1ap::ue_multicast_m_rbs_to_be_setup_at_modify_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ue_id_list_for_paging_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ul_up_tnl_address_to_upd_list_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::f1ap::ul_up_tnl_info_to_upd_list_item_ies_o>;
