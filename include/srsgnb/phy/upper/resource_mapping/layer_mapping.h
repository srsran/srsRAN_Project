#ifndef  SRSGNB_PHY_UPPER_RESOURCE_MAPPING_LAYER_MAPPING
#define  SRSGNB_PHY_UPPER_RESOURCE_MAPPING_LAYER_MAPPING

#include <complex>
#include <vector>

namespace srsgnb {

// NOTE: I'm assuming the layer mapping interface is generic and can be applied to both PDSCH and PUSCH.
class layer_mapping
{
public:
  // Add missing arguments to give info how to map these symbols if they are in the middle of a codeword.
  virtual std::vector<std::vector<std::complex<float> > > map(const std::vector<std::complex<float> >& symbols,
                                                              unsigned                                 num_layers) = 0;
};

} // namespace srsgnb

#endif //  SRSGNB_PHY_UPPER_RESOURCE_MAPPING_LAYER_MAPPING
