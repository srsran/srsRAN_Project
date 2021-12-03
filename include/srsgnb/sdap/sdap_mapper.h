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
