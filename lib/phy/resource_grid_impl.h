#ifndef SRSGNB_RESOURCE_GRID_IMPL_H
#define SRSGNB_RESOURCE_GRID_IMPL_H

#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/srsvec/aligned_vec.h"

namespace srsgnb {

class resource_grid_impl : public resource_grid
{
private:
  unsigned                  nof_symb;
  unsigned                  nof_subc;
  srsvec::aligned_vec<cf_t> buffer;

public:
  resource_grid_impl(unsigned nof_symb, unsigned nof_subc);
  void put(unsigned l, unsigned k, cf_t value) override;
  void put(unsigned l, span<const bool> mask, span<const cf_t>& symbol_buffer) override;
  void put(unsigned l, unsigned k_init, span<const cf_t> symbols) override;

  cf_t get(unsigned l, unsigned k) const override;
  void get(unsigned l, span<const bool> mask, span<cf_t>& symbol_buffer) override;
  void get(unsigned l, unsigned k_init, span<cf_t> symbols) override;

  void all_zero() override;
};

} // namespace srsgnb

#endif // SRSGNB_RESOURCE_GRID_IMPL_H
