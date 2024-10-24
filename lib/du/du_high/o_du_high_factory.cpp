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
#include "srsran/fapi/buffered_decorator_factories.h"
#include "srsran/fapi/logging_decorator_factories.h"
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
build_fapi_adaptors(const du_cell_config& du_cell, const o_du_high_sector_dependencies& dependencies, unsigned sector)
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

/// MAC-FAPI adaptor with logging decorator.
class mac_fapi_adaptor_with_logger_impl : public fapi_adaptor::mac_fapi_adaptor
{
public:
  mac_fapi_adaptor_with_logger_impl(const du_cell_config&                du_cell,
                                    const o_du_high_sector_dependencies& dependencies,
                                    unsigned                             sector) :
    logging_slot_gateway(fapi::create_logging_slot_gateway(*dependencies.gateway)),
    adaptor([](const du_cell_config&                du_cell_adapt,
               const o_du_high_sector_dependencies& dependencies_adapt,
               unsigned                             sector_id,
               fapi::slot_message_gateway&          gateway) {
      o_du_high_sector_dependencies adaptor_dependencies;
      adaptor_dependencies.last_msg_notifier = dependencies_adapt.last_msg_notifier;
      adaptor_dependencies.gateway           = &gateway;
      return build_fapi_adaptors(du_cell_adapt, adaptor_dependencies, sector_id);
    }(du_cell, dependencies, sector, *logging_slot_gateway)),
    logging_slot_time_notifier(fapi::create_logging_slot_time_notifier(adaptor->get_slot_time_notifier())),
    logging_slot_error_notifier(fapi::create_logging_slot_error_notifier(adaptor->get_slot_error_notifier())),
    logging_slot_data_notifier(fapi::create_logging_slot_data_notifier(adaptor->get_slot_data_notifier()))
  {
  }
  fapi::slot_time_message_notifier&  get_slot_time_notifier() override { return *logging_slot_time_notifier; }
  fapi::slot_error_message_notifier& get_slot_error_notifier() override { return *logging_slot_error_notifier; }
  fapi::slot_data_message_notifier&  get_slot_data_notifier() override { return *logging_slot_data_notifier; }
  mac_cell_result_notifier&          get_cell_result_notifier() override { return adaptor->get_cell_result_notifier(); }
  void                               set_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler) override
  {
    adaptor->set_cell_slot_handler(mac_slot_handler);
  }
  void set_cell_rach_handler(mac_cell_rach_handler& mac_rach_handler) override
  {
    adaptor->set_cell_rach_handler(mac_rach_handler);
  }
  void set_cell_pdu_handler(mac_pdu_handler& handler) override { adaptor->set_cell_pdu_handler(handler); }
  void set_cell_crc_handler(mac_cell_control_information_handler& handler) override
  {
    adaptor->set_cell_crc_handler(handler);
  }

private:
  std::unique_ptr<fapi::slot_message_gateway>        logging_slot_gateway;
  std::unique_ptr<fapi_adaptor::mac_fapi_adaptor>    adaptor;
  std::unique_ptr<fapi::slot_time_message_notifier>  logging_slot_time_notifier;
  std::unique_ptr<fapi::slot_error_message_notifier> logging_slot_error_notifier;
  std::unique_ptr<fapi::slot_data_message_notifier>  logging_slot_data_notifier;
};

/// MAC-FAPI adaptor with message buffering decorator.
class mac_fapi_adaptor_with_message_buffering_impl : public fapi_adaptor::mac_fapi_adaptor
{
public:
  mac_fapi_adaptor_with_message_buffering_impl(const du_cell_config&                du_cell,
                                               const o_du_high_sector_dependencies& dependencies,
                                               unsigned                             sector,
                                               unsigned                             l2_nof_slots_ahead) :
    buffered_modules(fapi::create_buffered_decorator(l2_nof_slots_ahead,
                                                     du_cell.scs_common,
                                                     *dependencies.gateway,
                                                     *dependencies.fapi_executor.value())),
    adaptor([](const du_cell_config&                du_cell_adapt,
               const o_du_high_sector_dependencies& dependencies_adapt,
               unsigned                             sector_id,
               fapi::slot_message_gateway&          gateway) {
      o_du_high_sector_dependencies adaptor_dependencies;
      adaptor_dependencies.last_msg_notifier = dependencies_adapt.last_msg_notifier;
      adaptor_dependencies.gateway           = &gateway;
      return build_fapi_adaptors(du_cell_adapt, adaptor_dependencies, sector_id);
    }(du_cell, dependencies, sector, buffered_modules->get_slot_message_gateway()))
  {
    buffered_modules->set_slot_time_notifier(adaptor->get_slot_time_notifier());
  }
  fapi::slot_time_message_notifier& get_slot_time_notifier() override
  {
    return buffered_modules->get_slot_time_message_notifier();
  }
  fapi::slot_error_message_notifier& get_slot_error_notifier() override { return adaptor->get_slot_error_notifier(); }
  fapi::slot_data_message_notifier&  get_slot_data_notifier() override { return adaptor->get_slot_data_notifier(); }
  mac_cell_result_notifier&          get_cell_result_notifier() override { return adaptor->get_cell_result_notifier(); }
  void                               set_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler) override
  {
    adaptor->set_cell_slot_handler(mac_slot_handler);
  }
  void set_cell_rach_handler(mac_cell_rach_handler& mac_rach_handler) override
  {
    adaptor->set_cell_rach_handler(mac_rach_handler);
  }
  void set_cell_pdu_handler(mac_pdu_handler& handler) override { adaptor->set_cell_pdu_handler(handler); }
  void set_cell_crc_handler(mac_cell_control_information_handler& handler) override
  {
    adaptor->set_cell_crc_handler(handler);
  }

private:
  std::unique_ptr<fapi::buffered_decorator>       buffered_modules;
  std::unique_ptr<fapi_adaptor::mac_fapi_adaptor> adaptor;
};

/// MAC-FAPI adaptor with logging and message buffering decorators.
class mac_fapi_adaptor_with_logger_and_message_buffering_impl : public fapi_adaptor::mac_fapi_adaptor
{
public:
  mac_fapi_adaptor_with_logger_and_message_buffering_impl(const du_cell_config&                du_cell,
                                                          const o_du_high_sector_dependencies& dependencies,
                                                          unsigned                             sector,
                                                          unsigned                             l2_nof_slots_ahead) :
    logging_slot_gateway(fapi::create_logging_slot_gateway(*dependencies.gateway)),
    buffered_modules(fapi::create_buffered_decorator(l2_nof_slots_ahead,
                                                     du_cell.scs_common,
                                                     *logging_slot_gateway,
                                                     *dependencies.fapi_executor.value())),
    adaptor([](const du_cell_config&                du_cell_adapt,
               const o_du_high_sector_dependencies& dependencies_adapt,
               unsigned                             sector_id,
               fapi::slot_message_gateway&          gateway) {
      o_du_high_sector_dependencies adaptor_dependencies;
      adaptor_dependencies.last_msg_notifier = dependencies_adapt.last_msg_notifier;
      adaptor_dependencies.gateway           = &gateway;
      return build_fapi_adaptors(du_cell_adapt, adaptor_dependencies, sector_id);
    }(du_cell, dependencies, sector, buffered_modules->get_slot_message_gateway())),
    logging_slot_time_notifier(
        fapi::create_logging_slot_time_notifier(buffered_modules->get_slot_time_message_notifier())),
    logging_slot_error_notifier(fapi::create_logging_slot_error_notifier(adaptor->get_slot_error_notifier())),
    logging_slot_data_notifier(fapi::create_logging_slot_data_notifier(adaptor->get_slot_data_notifier()))
  {
    buffered_modules->set_slot_time_notifier(adaptor->get_slot_time_notifier());
  }
  fapi::slot_time_message_notifier&  get_slot_time_notifier() override { return *logging_slot_time_notifier; }
  fapi::slot_error_message_notifier& get_slot_error_notifier() override { return *logging_slot_error_notifier; }
  fapi::slot_data_message_notifier&  get_slot_data_notifier() override { return *logging_slot_data_notifier; }
  mac_cell_result_notifier&          get_cell_result_notifier() override { return adaptor->get_cell_result_notifier(); }
  void                               set_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler) override
  {
    adaptor->set_cell_slot_handler(mac_slot_handler);
  }
  void set_cell_rach_handler(mac_cell_rach_handler& mac_rach_handler) override
  {
    adaptor->set_cell_rach_handler(mac_rach_handler);
  }
  void set_cell_pdu_handler(mac_pdu_handler& handler) override { adaptor->set_cell_pdu_handler(handler); }
  void set_cell_crc_handler(mac_cell_control_information_handler& handler) override
  {
    adaptor->set_cell_crc_handler(handler);
  }

private:
  std::unique_ptr<fapi::slot_message_gateway>        logging_slot_gateway;
  std::unique_ptr<fapi::buffered_decorator>          buffered_modules;
  std::unique_ptr<fapi_adaptor::mac_fapi_adaptor>    adaptor;
  std::unique_ptr<fapi::slot_time_message_notifier>  logging_slot_time_notifier;
  std::unique_ptr<fapi::slot_error_message_notifier> logging_slot_error_notifier;
  std::unique_ptr<fapi::slot_data_message_notifier>  logging_slot_data_notifier;
};

} // namespace

std::unique_ptr<o_du_high> srsran::srs_du::make_o_du_high(const o_du_high_config&  config,
                                                          o_du_high_dependencies&& odu_dependencies)
{
  srsran_assert(config.du_hi.ran.cells.size() == odu_dependencies.sectors.size(),
                "DU high number of cells '{}' does not match number sectors '{}' in O-DU dependencies",
                config.du_hi.ran.cells.size(),
                odu_dependencies.sectors.size());

  o_du_high_impl_dependencies dependencies;

  for (unsigned i = 0, e = odu_dependencies.sectors.size(); i != e; ++i) {
    if (config.fapi.log_level == srslog::basic_levels::debug) {
      if (config.fapi.l2_nof_slots_ahead == 0) {
        dependencies.du_high_adaptor.push_back(std::make_unique<mac_fapi_adaptor_with_logger_impl>(
            config.du_hi.ran.cells[i], odu_dependencies.sectors[i], i));
      } else {
        srsran_assert(odu_dependencies.sectors[i].fapi_executor, "Invalid executor for the FAPI buffered decorator");
        dependencies.du_high_adaptor.push_back(
            std::make_unique<mac_fapi_adaptor_with_logger_and_message_buffering_impl>(
                config.du_hi.ran.cells[i], odu_dependencies.sectors[i], i, config.fapi.l2_nof_slots_ahead));
      }
    } else {
      if (config.fapi.l2_nof_slots_ahead == 0) {
        dependencies.du_high_adaptor.push_back(
            build_fapi_adaptors(config.du_hi.ran.cells[i], odu_dependencies.sectors[i], i));
      } else {
        srsran_assert(odu_dependencies.sectors[i].fapi_executor, "Invalid executor for the FAPI buffered decorator");
        dependencies.du_high_adaptor.push_back(std::make_unique<mac_fapi_adaptor_with_message_buffering_impl>(
            config.du_hi.ran.cells[i], odu_dependencies.sectors[i], i, config.fapi.l2_nof_slots_ahead));
      }
    }
  }

  auto& logger        = srslog::fetch_basic_logger("DU");
  dependencies.logger = &logger;

  logger.debug("FAPI adaptors created successfully");

  auto odu = std::make_unique<o_du_high_impl>(std::move(dependencies));

  // Instantiate DU-high.
  srs_du::du_high_configuration du_hi_cfg = config.du_hi;
  du_hi_cfg.phy_adapter                   = &odu->get_mac_result_notifier();
  odu->set_du_high(make_du_high(du_hi_cfg));

  logger.info("DU created successfully");

  return odu;
}
