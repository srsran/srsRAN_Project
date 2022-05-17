/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SDAP_SDAP_MAPPER_H
#define SRSGNB_SDAP_SDAP_MAPPER_H

namespace srsgnb {

/// DRB to QoS flow mapping.
class sdap_mapper
{
public:
  virtual ~sdap_mapper() = default;

  virtual bool associate(int mapping_list_number, const void* mapping_list) = 0;

  virtual bool drb_release(int drb) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_SDAP_SDAP_MAPPER_H
