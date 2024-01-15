/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "../../support/resource_grid_mapper_test_doubles.h"
#include "pdsch_processor_test_data.h"
#include "pdsch_processor_test_doubles.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/phy/upper/tx_buffer_pool.h"
#include "srsran/phy/upper/unique_tx_buffer.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/support/executors/task_worker_pool.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os, "{}", test_case.context.pdu);
  return os;
}

namespace {

using PdschProcessorParams = std::tuple<std::string, test_case_t>;

// Number of concurrent threads.
static constexpr unsigned NOF_CONCURRENT_THREADS = 16;

class PdschProcessorFixture : public ::testing::TestWithParam<PdschProcessorParams>
{
private:
  std::shared_ptr<pdsch_processor_factory> create_pdsch_processor_factory(const std::string& type)
  {
    std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
    if (!crc_calc_factory) {
      return nullptr;
    }

    std::shared_ptr<ldpc_encoder_factory> ldpc_encoder_factory = create_ldpc_encoder_factory_sw("auto");
    if (!ldpc_encoder_factory) {
      return nullptr;
    }

    std::shared_ptr<ldpc_rate_matcher_factory> ldpc_rate_matcher_factory = create_ldpc_rate_matcher_factory_sw();
    if (!ldpc_rate_matcher_factory) {
      return nullptr;
    }

    std::shared_ptr<ldpc_segmenter_tx_factory> ldpc_segmenter_tx_factory =
        create_ldpc_segmenter_tx_factory_sw(crc_calc_factory);
    if (!ldpc_segmenter_tx_factory) {
      return nullptr;
    }

    pdsch_encoder_factory_sw_configuration pdsch_encoder_factory_config = {};
    pdsch_encoder_factory_config.encoder_factory                        = ldpc_encoder_factory;
    pdsch_encoder_factory_config.rate_matcher_factory                   = ldpc_rate_matcher_factory;
    pdsch_encoder_factory_config.segmenter_factory                      = ldpc_segmenter_tx_factory;
    std::shared_ptr<pdsch_encoder_factory> pdsch_encoder_factory =
        create_pdsch_encoder_factory_sw(pdsch_encoder_factory_config);
    if (!pdsch_encoder_factory) {
      return nullptr;
    }

    std::shared_ptr<channel_modulation_factory> modulator_factory = create_channel_modulation_sw_factory();
    if (!modulator_factory) {
      return nullptr;
    }

    std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
    if (!prg_factory) {
      return nullptr;
    }

    std::shared_ptr<pdsch_modulator_factory> pdsch_modulator_factory =
        create_pdsch_modulator_factory_sw(modulator_factory, prg_factory);
    if (!pdsch_modulator_factory) {
      return nullptr;
    }

    std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_pdsch_factory =
        create_dmrs_pdsch_processor_factory_sw(prg_factory);
    if (!dmrs_pdsch_factory) {
      return nullptr;
    }

    if (type == "generic") {
      return create_pdsch_processor_factory_sw(pdsch_encoder_factory, pdsch_modulator_factory, dmrs_pdsch_factory);
    }

    if (type == "concurrent") {
      worker_pool = std::make_unique<task_worker_pool<concurrent_queue_policy::locking_mpmc>>(
          NOF_CONCURRENT_THREADS, 128, "pdsch_proc");
      executor = std::make_unique<task_worker_pool_executor<concurrent_queue_policy::locking_mpmc>>(*worker_pool);

      return create_pdsch_concurrent_processor_factory_sw(crc_calc_factory,
                                                          ldpc_encoder_factory,
                                                          ldpc_rate_matcher_factory,
                                                          prg_factory,
                                                          modulator_factory,
                                                          dmrs_pdsch_factory,
                                                          *executor,
                                                          NOF_CONCURRENT_THREADS);
    }

    if (type == "lite") {
      return create_pdsch_lite_processor_factory_sw(ldpc_segmenter_tx_factory,
                                                    ldpc_encoder_factory,
                                                    ldpc_rate_matcher_factory,
                                                    prg_factory,
                                                    modulator_factory,
                                                    dmrs_pdsch_factory);
    }

    return nullptr;
  }

protected:
  // PDSCH processor.
  std::unique_ptr<pdsch_processor> pdsch_proc;
  // PDSCH validator.
  std::unique_ptr<pdsch_pdu_validator> pdu_validator;
  // PDSCH rate matcher buffer pool.
  std::unique_ptr<tx_buffer_pool> rm_buffer_pool;
  // Worker pool.
  std::unique_ptr<task_worker_pool<concurrent_queue_policy::locking_mpmc>>          worker_pool;
  std::unique_ptr<task_worker_pool_executor<concurrent_queue_policy::locking_mpmc>> executor;

  void SetUp() override
  {
    const PdschProcessorParams& param        = GetParam();
    const std::string&          factory_type = std::get<0>(param);

    // Create PDSCH processor factory.
    std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory = create_pdsch_processor_factory(factory_type);
    ASSERT_NE(pdsch_proc_factory, nullptr) << "Invalid PDSCH processor factory.";

    // Create actual PDSCH processor.
    pdsch_proc = pdsch_proc_factory->create();
    ASSERT_NE(pdsch_proc, nullptr) << "Cannot create PDSCH processor";

    // Create actual PDSCH processor validator.
    pdu_validator = pdsch_proc_factory->create_validator();
    ASSERT_NE(pdu_validator, nullptr) << "Cannot create PDSCH validator";

    // Create buffer pool.
    tx_buffer_pool_config buffer_pool_config;
    buffer_pool_config.max_codeblock_size   = ldpc::MAX_CODEBLOCK_SIZE;
    buffer_pool_config.nof_buffers          = 1;
    buffer_pool_config.nof_codeblocks       = pdsch_constants::CODEWORD_MAX_SIZE.value() / ldpc::MAX_MESSAGE_SIZE;
    buffer_pool_config.expire_timeout_slots = 0;
    buffer_pool_config.external_soft_bits   = false;
    rm_buffer_pool                          = create_tx_buffer_pool(buffer_pool_config);
    ASSERT_NE(rm_buffer_pool, nullptr) << "Cannot create buffer pool";
  }

  void TearDown() override
  {
    if (worker_pool) {
      worker_pool->stop();
    }
  }
};

TEST_P(PdschProcessorFixture, PdschProcessorVectortest)
{
  pdsch_processor_notifier_spy notifier_spy;
  const PdschProcessorParams&  param     = GetParam();
  const test_case_t&           test_case = std::get<1>(param);
  const test_case_context&     context   = test_case.context;
  pdsch_processor::pdu_t       config    = context.pdu;
  config.codewords.front().new_data      = true;

  unsigned max_symb  = context.rg_nof_symb;
  unsigned max_prb   = context.rg_nof_rb;
  unsigned max_ports = config.precoding.get_nof_ports();

  // Prepare resource grid and resource grid mapper spies.
  resource_grid_writer_spy              grid(max_ports, max_symb, max_prb);
  std::unique_ptr<resource_grid_mapper> mapper = create_resource_grid_mapper(max_ports, NRE * max_prb, grid);

  // Read input data as a bit-packed transport block.
  std::vector<uint8_t> transport_block = test_case.sch_data.read();
  ASSERT_FALSE(transport_block.empty()) << "Failed to load transport block.";

  // Prepare transport blocks view.
  static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS> transport_blocks;
  transport_blocks.emplace_back(transport_block);

  // Make sure the configuration is valid.
  ASSERT_TRUE(pdu_validator->is_valid(config));

  trx_buffer_identifier buffer_id(0, 0);

  unsigned nof_codeblocks =
      ldpc::compute_nof_codeblocks(units::bits(transport_block.size() * 8), config.ldpc_base_graph);

  unique_tx_buffer rm_buffer = rm_buffer_pool->reserve(slot_point(), buffer_id, nof_codeblocks);

  // Process PDSCH.
  pdsch_proc->process(*mapper, std::move(rm_buffer), notifier_spy, transport_blocks, config);

  // Waits for the processor to finish.
  notifier_spy.wait_for_finished();

  // Assert results.
  grid.assert_entries(test_case.grid_expected.read());
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PdschProcessorVectortest,
                         PdschProcessorFixture,
                         testing::Combine(testing::Values("generic", "concurrent", "lite"),
                                          ::testing::ValuesIn(pdsch_processor_test_data)));
} // namespace

} // namespace srsran
