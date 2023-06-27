/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "cell_config_builder_params.h"
#include "serving_cell_config.h"
#include "srsran/ran/csi_rs/csi_meas_config.h"
#include "srsran/ran/pdcch/aggregation_level.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"

namespace srsran {
namespace config_helpers {

static_vector<uint8_t, 8> generate_k1_candidates(const tdd_ul_dl_config_common& tdd_cfg);

carrier_configuration make_default_dl_carrier_configuration(const cell_config_builder_params& params = {});

carrier_configuration make_default_ul_carrier_configuration(const cell_config_builder_params& params = {});

tdd_ul_dl_config_common make_default_tdd_ul_dl_config_common(const cell_config_builder_params& params = {});

coreset_configuration make_default_coreset_config(const cell_config_builder_params& params = {});

coreset_configuration make_default_coreset0_config(const cell_config_builder_params& params = {});

search_space_configuration make_default_search_space_zero_config();

search_space_configuration make_default_common_search_space_config();

search_space_configuration make_default_ue_search_space_config();

bwp_configuration make_default_init_bwp(const cell_config_builder_params& params = {});

dl_config_common make_default_dl_config_common(const cell_config_builder_params& params = {});

ul_config_common make_default_ul_config_common(const cell_config_builder_params& params = {});

ssb_configuration make_default_ssb_config(const cell_config_builder_params& params = {});

uplink_config make_default_ue_uplink_config(const cell_config_builder_params& params = {});

pusch_config make_default_pusch_config();

srs_config make_default_srs_config(const cell_config_builder_params& params);

pdsch_serving_cell_config make_default_pdsch_serving_cell_config();

pdsch_config make_default_pdsch_config(const cell_config_builder_params& params = {});

/// \brief Creates a default UE Serving Cell configuration.
serving_cell_config create_default_initial_ue_serving_cell_config(const cell_config_builder_params& params = {});

/// \brief Creates a default UE PSCell configuration.
cell_config_dedicated create_default_initial_ue_spcell_cell_config(const cell_config_builder_params& params = {});

/// \brief Computes maximum nof. candidates that can be accommodated in a CORESET for a given aggregation level.
/// \return Maximum nof. candidates for a aggregation level.
uint8_t compute_max_nof_candidates(aggregation_level aggr_lvl, const coreset_configuration& cs_cfg);

/// \brief Creates PDSCH Time Domain Resource configuration given the PDSCH OFDM symbol start and stop index.
/// \param[in] ofdm_symbols PDSCH OFDM symbols.
/// \return PDSCH Time Domain Resource configuration.
pdsch_time_domain_resource_allocation make_pdsch_time_domain_resource(ofdm_symbol_range ofdm_symbols);

/// \brief Generates PDSCH OFDM symbol ranges based on CORESET and SearchSpace configuration.
///
/// Function generates a list of OFDM symbol ranges over which PDSCH can be scheduled. The list is generated considering
/// the nof. DL symbols configured in a particular slot, CORESET duration and first PDCCH monitoring symbol index in
/// SearchSpace configuration. This is mainly needed in TDD usecase where TDD pattern configured contains special slots
/// where not all OFDM symbols are reserved for Downlink transmission.
///
/// \param[in] cs0_duration CORESET#0 duration in nof. OFDM symbols.
/// \param[in] cs1_duration CORESET#1 duration in nof. OFDM symbols.
/// \param[in] ss0_idx SearchSpace#0 index.
/// \param[in] ss1_first_monitoring_symb_idx First monitoring occasion OFDM symbol index in SearchSpace#1.
/// \param[in] ss2_first_monitoring_symb_idx First monitoring occasion OFDM symbol index in SearchSpace#2.
/// \param[in] nof_dl_symbols_in_slot Number of DL OFDM symbols configured for in a particular slot.
/// \return List of PDSCH OFDM symbol range.
std::vector<ofdm_symbol_range> generate_pdsch_ofdm_symbol_ranges(unsigned           cs0_duration,
                                                                 uint8_t            ss0_idx,
                                                                 unsigned           ss1_first_monitoring_symb_idx,
                                                                 unsigned           nof_dl_symbols_in_slot,
                                                                 optional<unsigned> cs1_duration                  = {},
                                                                 optional<unsigned> ss2_first_monitoring_symb_idx = {});

} // namespace config_helpers
} // namespace srsran
