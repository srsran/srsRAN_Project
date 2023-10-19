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

#include "srsran/phy/support/support_factories.h"
#include "interpolator/interpolator_linear_impl.h"
#include "prach_buffer_impl.h"
#include "prach_buffer_pool_impl.h"
#include "resource_grid_impl.h"
#include "resource_grid_pool_impl.h"
#include "srsran/phy/generic_functions/precoding/precoding_factories.h"
#include "srsran/ran/prach/prach_constants.h"

using namespace srsran;

namespace {

/// Factory that builds resource grid objects.
class resource_grid_factory_impl : public resource_grid_factory
{
public:
  explicit resource_grid_factory_impl(std::shared_ptr<channel_precoder_factory> precoder_factory_) :
    precoder_factory(std::move(precoder_factory_))
  {
    srsran_assert(precoder_factory, "Invalid channel precoder factory.");
  }

  /// \brief Creates a resource grid instance for a number of ports, symbols and subcarriers.
  /// \param[in] nof_ports   Number of ports.
  /// \param[in] nof_symbols Number of OFDM symbols.
  /// \param[in] nof_subc    Number of subcarriers.
  /// \return A resource grid object.
  std::unique_ptr<resource_grid> create(unsigned nof_ports, unsigned nof_symb, unsigned nof_subc) override
  {
    return std::make_unique<resource_grid_impl>(nof_ports, nof_symb, nof_subc, precoder_factory->create());
  }

private:
  // Channel precoder factory.
  std::shared_ptr<channel_precoder_factory> precoder_factory;
};

} // namespace

std::unique_ptr<resource_grid_pool>
srsran::create_resource_grid_pool(unsigned                                      nof_sectors,
                                  unsigned                                      nof_slots,
                                  std::vector<std::unique_ptr<resource_grid>>&& grids)
{
  return std::make_unique<resource_grid_pool_impl>(nof_sectors, nof_slots, std::move(grids));
}

std::unique_ptr<prach_buffer_pool>
srsran::create_prach_buffer_pool(std::vector<std::unique_ptr<prach_buffer>>&& elements)
{
  return std::make_unique<prach_buffer_pool_impl>(std::move(elements));
}

std::unique_ptr<prach_buffer> srsran::create_prach_buffer_long(unsigned max_nof_antennas, unsigned max_nof_fd_occasions)
{
  return std::make_unique<prach_buffer_impl>(max_nof_antennas,
                                             1,
                                             max_nof_fd_occasions,
                                             prach_constants::LONG_SEQUENCE_MAX_NOF_SYMBOLS,
                                             prach_constants::LONG_SEQUENCE_LENGTH);
}

std::unique_ptr<prach_buffer> srsran::create_prach_buffer_short(unsigned max_nof_antennas,
                                                                unsigned max_nof_td_occasions,
                                                                unsigned max_nof_fd_occasions)
{
  return std::make_unique<prach_buffer_impl>(max_nof_antennas,
                                             max_nof_td_occasions,
                                             max_nof_fd_occasions,
                                             prach_constants::SHORT_SEQUENCE_MAX_NOF_SYMBOLS,
                                             prach_constants::SHORT_SEQUENCE_LENGTH);
}

std::unique_ptr<srsran::interpolator> srsran::create_interpolator()
{
  return std::make_unique<interpolator_linear_impl>();
}

std::shared_ptr<resource_grid_factory>
srsran::create_resource_grid_factory(std::shared_ptr<channel_precoder_factory> precoder_factory)
{
  return std::make_shared<resource_grid_factory_impl>(std::move(precoder_factory));
}

/// Implements an ideal precoder for testing purposes.
class channel_precoder_dummy : public channel_precoder
{
public:
  // See interface for documentation.
  void apply_precoding(re_buffer_writer&              output,
                       const re_buffer_reader&        input,
                       const precoding_weight_matrix& precoding) override
  {
    unsigned nof_ports  = precoding.get_nof_ports();
    unsigned nof_layers = precoding.get_nof_layers();
    unsigned nof_re     = input.get_nof_re();

    srsran_assert(output.get_nof_slices() == nof_ports,
                  "The output number of slices (i.e., {}) must be equal to the number of ports (i.e., {}).",
                  output.get_nof_slices(),
                  nof_ports);
    srsran_assert(input.get_nof_slices() == nof_layers,
                  "The input number of slices (i.e., {}) must be equal to the number of layers (i.e., {}).",
                  input.get_nof_slices(),
                  nof_layers);
    srsran_assert(output.get_nof_re() == input.get_nof_re(),
                  "The output number of RE (i.e., {}) must be equal to the input number of RE (i.e., {}).",
                  output.get_nof_re(),
                  input.get_nof_re());

    cf_t correction = std::sqrt(static_cast<float>(nof_ports));

    for (unsigned i_re = 0; i_re != nof_re; ++i_re) {
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        cf_t sum = 0.0;
        for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
          sum += input.get_slice(i_layer)[i_re] * precoding.get_coefficient(i_layer, i_port);
        }
        output.get_slice(i_port)[i_re] = sum * correction;
      }
    }
  }

  // See interface for documentation.
  void apply_layer_map_and_precoding(re_buffer_writer&              output,
                                     span<const ci8_t>              input,
                                     const precoding_weight_matrix& precoding) override
  {
    unsigned nof_layers = precoding.get_nof_layers();
    unsigned nof_ports  = precoding.get_nof_layers();
    unsigned nof_re     = input.size() / nof_layers;

    srsran_assert(output.get_nof_slices() == nof_ports,
                  "The output number of slices (i.e., {}) must be equal to the number of ports (i.e., {}).",
                  output.get_nof_slices(),
                  nof_ports);
    srsran_assert(input.size() % nof_layers == 0,
                  "The input size (i.e., {}) must be multiple of the number of layers (i.e., {}).",
                  input.size(),
                  nof_layers);
    srsran_assert(output.get_nof_re() == nof_re,
                  "The output number of RE (i.e., {}) must be equal to the input size (i.e., {}) times the number of "
                  "layers (i.e., {}).",
                  output.get_nof_re(),
                  input.size(),
                  nof_layers);

    cf_t correction = std::sqrt(static_cast<float>(nof_ports));

    for (unsigned i_re = 0; i_re != nof_re; ++i_re) {
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        cf_t sum = 0.0;
        for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
          sum += to_cf(input[nof_layers * i_re + i_layer]) * precoding.get_coefficient(i_layer, i_port);
        }
        output.get_slice(i_port)[i_re] = sum * correction;
      }
    }
  }
};

std::unique_ptr<resource_grid_mapper>
srsran::create_resource_grid_mapper(unsigned nof_ports, unsigned nof_subc, srsran::resource_grid_writer& writer)
{
  return std::make_unique<resource_grid_mapper_impl>(
      nof_ports, nof_subc, writer, std::make_unique<channel_precoder_dummy>());
}