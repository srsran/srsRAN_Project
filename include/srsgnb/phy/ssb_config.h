
#ifndef SRSGNB_PHY_SSB_CONFIG_H_
#define SRSGNB_PHY_SSB_CONFIG_H_

namespace srsgnb {

enum class ssb_mapping_case { A, B, C, D, E };

class ssb_config
{
public:
  ssb_mapping_case mapping_case;
};

} // namespace srsgnb
#endif // SRSGNB_PHY_SSB_CONFIG_H_
