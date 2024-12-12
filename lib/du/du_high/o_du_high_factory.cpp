/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/du/du_high/o_du_high_factory.h"
#include "o_du_high_impl.h"
#include "srsran/du/du_high/du_high_factory.h"
#include "srsran/e2/e2_du_factory.h"
#include "srsran/fapi/decorator_factory.h"
#include "srsran/fapi/messages.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor_factory.h"
#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "srsran/fapi_adaptor/uci_part2_correspondence_generator.h"

using namespace srsran;
using namespace srs_du;

static std::unique_ptr<fapi_adaptor::mac_fapi_adaptor>
build_mac_fapi_adaptor(unsigned                                                       sector_id,
                       subcarrier_spacing                                             scs,
                       fapi::slot_message_gateway&                                    gateway,
                       fapi::slot_last_message_notifier&                              last_msg_notifier,
                       std::unique_ptr<fapi_adaptor::precoding_matrix_mapper>         pm_mapper,
                       std::unique_ptr<fapi_adaptor::uci_part2_correspondence_mapper> part2_mapper,
                       unsigned                                                       cell_nof_prbs)
{
  std::unique_ptr<fapi_adaptor::mac_fapi_adaptor_factory> adaptor_factory =
      fapi_adaptor::create_mac_fapi_adaptor_factory();
  report_error_if_not(adaptor_factory, "Invalid MAC adaptor factory.");

  fapi_adaptor::mac_fapi_adaptor_config mac_fapi_config;
  mac_fapi_config.sector_id     = sector_id;
  mac_fapi_config.cell_nof_prbs = cell_nof_prbs;
  mac_fapi_config.scs           = scs;

  fapi_adaptor::mac_fapi_adaptor_dependencies mac_fapi_deps;
  mac_fapi_deps.gateway           = &gateway;
  mac_fapi_deps.last_msg_notifier = &last_msg_notifier;
  mac_fapi_deps.pm_mapper         = std::move(pm_mapper);
  mac_fapi_deps.part2_mapper      = std::move(part2_mapper);

  return adaptor_factory->create(mac_fapi_config, std::move(mac_fapi_deps));
}

static std::unique_ptr<fapi_adaptor::mac_fapi_adaptor>
build_fapi_adaptor(const du_cell_config& du_cell, const o_du_high_sector_dependencies& dependencies, unsigned sector)
{
  srsran_assert(dependencies.gateway, "Invalid gateway");
  srsran_assert(dependencies.last_msg_notifier, "Invalid last message notifier");

  const subcarrier_spacing scs             = du_cell.scs_common;
  auto                     pm_tools        = fapi_adaptor::generate_precoding_matrix_tables(du_cell.dl_carrier.nof_ant);
  auto                     uci_part2_tools = fapi_adaptor::generate_uci_part2_correspondence(1);

  return build_mac_fapi_adaptor(
      sector,
      scs,
      *dependencies.gateway,
      *dependencies.last_msg_notifier,
      std::move(std::get<std::unique_ptr<fapi_adaptor::precoding_matrix_mapper>>(pm_tools)),
      std::move(std::get<std::unique_ptr<fapi_adaptor::uci_part2_correspondence_mapper>>(uci_part2_tools)),
      get_max_Nprb(du_cell.dl_carrier.carrier_bw_mhz, scs, frequency_range::FR1));
}

namespace {

/// MAC-FAPI adaptor wrapper that wraps an adaptor and a FAPI decorator.
class mac_fapi_adaptor_wrapper : public fapi_adaptor::mac_fapi_adaptor
{
public:
  mac_fapi_adaptor_wrapper(const du_cell_config&                 du_cell,
                           const o_du_high_sector_dependencies&  dependencies,
                           unsigned                              sector,
                           std::unique_ptr<fapi::fapi_decorator> decorator_) :
    decorator(std::move(decorator_)),
    adaptor([](const du_cell_config&             du_cell_adapt,
               unsigned                          sector_id,
               fapi::slot_message_gateway&       gateway,
               fapi::slot_last_message_notifier& last_msg_notifier) {
      o_du_high_sector_dependencies adaptor_dependencies;
      adaptor_dependencies.last_msg_notifier = &last_msg_notifier;
      adaptor_dependencies.gateway           = &gateway;
      return build_fapi_adaptor(du_cell_adapt, adaptor_dependencies, sector_id);
    }(du_cell,
            sector,
            decorator ? decorator->get_slot_message_gateway() : *dependencies.gateway,
            decorator ? decorator->get_slot_last_message_notifier() : *dependencies.last_msg_notifier))
  {
    if (decorator) {
      decorator->set_slot_data_message_notifier(adaptor->get_slot_data_message_notifier());
      decorator->set_slot_error_message_notifier(adaptor->get_slot_error_message_notifier());
      decorator->set_slot_time_message_notifier(adaptor->get_slot_time_message_notifier());
    }
  }

  // See interface for documentation.
  fapi::slot_time_message_notifier& get_slot_time_message_notifier() override
  {
    return decorator ? decorator->get_slot_time_message_notifier() : adaptor->get_slot_time_message_notifier();
  }

  // See interface for documentation.
  fapi::slot_error_message_notifier& get_slot_error_message_notifier() override
  {
    return decorator ? decorator->get_slot_error_message_notifier() : adaptor->get_slot_error_message_notifier();
  }

  // See interface for documentation.
  fapi::slot_data_message_notifier& get_slot_data_message_notifier() override
  {
    return decorator ? decorator->get_slot_data_message_notifier() : adaptor->get_slot_data_message_notifier();
  }

  // See interface for documentation.
  mac_cell_result_notifier& get_cell_result_notifier() override { return adaptor->get_cell_result_notifier(); }

  // See interface for documentation.
  void set_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler) override
  {
    adaptor->set_cell_slot_handler(mac_slot_handler);
  }

  // See interface for documentation.
  void set_cell_rach_handler(mac_cell_rach_handler& mac_rach_handler) override
  {
    adaptor->set_cell_rach_handler(mac_rach_handler);
  }

  // See interface for documentation.
  void set_cell_pdu_handler(mac_pdu_handler& handler) override { adaptor->set_cell_pdu_handler(handler); }

  // See interface for documentation.
  void set_cell_crc_handler(mac_cell_control_information_handler& handler) override
  {
    adaptor->set_cell_crc_handler(handler);
  }

private:
  std::unique_ptr<fapi::fapi_decorator>           decorator;
  std::unique_ptr<fapi_adaptor::mac_fapi_adaptor> adaptor;
};

} // namespace

static o_du_high_impl_dependencies
resolve_o_du_high_impl_dependencies(const o_du_high_config&             config,
                                    span<o_du_high_sector_dependencies> odu_dependencies)
{
  srsran_assert(config.du_hi.ran.cells.size() == odu_dependencies.size(),
                "DU high number of cells '{}' does not match number sectors '{}' in O-DU dependencies",
                config.du_hi.ran.cells.size(),
                odu_dependencies.size());

  o_du_high_impl_dependencies dependencies;
  dependencies.logger = &srslog::fetch_basic_logger("DU");

  for (unsigned i = 0, e = odu_dependencies.size(); i != e; ++i) {
    fapi::decorator_config decorator_cfg;

    if (config.fapi.log_level == srslog::basic_levels::debug) {
      decorator_cfg.logging_cfg.emplace(fapi::logging_decorator_config{srslog::fetch_basic_logger("FAPI", true),
                                                                       *odu_dependencies[i].gateway,
                                                                       *odu_dependencies[i].last_msg_notifier});
    }

    if (config.fapi.l2_nof_slots_ahead != 0) {
      srsran_assert(odu_dependencies[i].fapi_executor, "Invalid executor for the FAPI message bufferer decorator");
      decorator_cfg.bufferer_cfg.emplace(
          fapi::message_bufferer_decorator_config{config.fapi.l2_nof_slots_ahead,
                                                  config.du_hi.ran.cells[i].scs_common,
                                                  *odu_dependencies[i].fapi_executor.value(),
                                                  *odu_dependencies[i].gateway,
                                                  *odu_dependencies[i].last_msg_notifier});
    }

    dependencies.du_high_adaptor.push_back(std::make_unique<mac_fapi_adaptor_wrapper>(
        config.du_hi.ran.cells[i], odu_dependencies[i], i, fapi::create_decorators(decorator_cfg)));
  }

  return dependencies;
}

std::unique_ptr<o_du_high> srsran::srs_du::make_o_du_high(const o_du_high_config&  config,
                                                          o_du_high_dependencies&& odu_dependencies)
{
  o_du_high_impl_dependencies dependencies = resolve_o_du_high_impl_dependencies(config, odu_dependencies.sectors);

  dependencies.logger->debug("FAPI adaptors created successfully");

  srs_du::du_high_configuration du_hi_cfg = config.du_hi;

  auto odu = std::make_unique<o_du_high_impl>(std::move(dependencies));

  // Instantiate DU-high.
  odu_dependencies.du_hi.phy_adapter = &odu->get_mac_result_notifier();

  if (!odu_dependencies.e2_client) {
    odu->set_du_high(make_du_high(du_hi_cfg, odu_dependencies.du_hi));
    dependencies.logger->info("DU created successfully");

    return odu;
  }

  auto du_hi = make_du_high(du_hi_cfg, odu_dependencies.du_hi);

  auto e2agent = create_e2_du_agent(
      config.e2ap_config,
      *odu_dependencies.e2_client,
      odu_dependencies.e2_du_metric_iface,
      &du_hi->get_f1ap_du(),
      &du_hi->get_du_configurator(),
      timer_factory{*odu_dependencies.du_hi.timers, odu_dependencies.du_hi.exec_mapper->du_e2_executor()},
      odu_dependencies.du_hi.exec_mapper->du_e2_executor());

  odu->set_e2_agent(std::move(e2agent));
  odu->set_du_high(std::move(du_hi));
  dependencies.logger->info("DU created successfully");

  return odu;
}
