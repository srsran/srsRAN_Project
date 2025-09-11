/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "du_high_config_cli11_ntn_schema.h"
#include "du_high_config.h"
#include "srsran/support/cli11_utils.h"

using namespace srsran;

#ifndef SRSRAN_HAS_ENTERPRISE_NTN

void srsran::configure_cli11_advanced_ntn_args(CLI::App& app, ntn_config& config)
{
  // Advanced NTN config parameters are not implemented.
}

void srsran::configure_cli11_cell_ntn_args(CLI::App& app, std::optional<ntn_config>& cell_ntn_params)
{
  // Per-cell NTN config parameters are not implemented.
}

#endif // SRSRAN_HAS_ENTERPRISE_NTN

static void configure_cli11_epoch_time(CLI::App& app, epoch_time_t& epoch_time)
{
  add_option(app, "--sfn", epoch_time.sfn, "SFN Part")->capture_default_str()->check(CLI::Range(0, 1023));
  add_option(app, "--subframe_number", epoch_time.subframe_number, "Sub-frame number Part")
      ->capture_default_str()
      ->check(CLI::Range(0, 9));
}

static void configure_cli11_ta_info(CLI::App& app, ta_info_t& ta_info)
{
  add_option(app, "--ta_common", ta_info.ta_common, "TA common")
      ->capture_default_str()
      ->check(CLI::Range(0.0, 270730.0));
  add_option(app, "--ta_common_drift", ta_info.ta_common_drift, "Drift rate of the common TA")
      ->capture_default_str()
      ->check(CLI::Range(-51.4606, 51.4606));
  add_option(app, "--ta_common_drift_variant", ta_info.ta_common_drift_variant, "Drift rate variation of the common TA")
      ->capture_default_str()
      ->check(CLI::Range(0.0, 0.57898));
  add_option(app, "--ta_common_offset", ta_info.ta_common_offset, "Constant offset added to TA common")
      ->capture_default_str()
      ->check(CLI::Range(0.0, 10000.0));
}

static void configure_cli11_ephemeris_info_ecef(CLI::App& app, ecef_coordinates_t& ephemeris_info)
{
  add_option(app, "--pos_x", ephemeris_info.position_x, "X Position of the satellite [m]")
      ->capture_default_str()
      ->check(CLI::Range(-43620761.6, 43620759.3));
  add_option(app, "--pos_y", ephemeris_info.position_y, "Y Position of the satellite [m]")
      ->capture_default_str()
      ->check(CLI::Range(-43620761.6, 43620759.3));
  add_option(app, "--pos_z", ephemeris_info.position_z, "Z Position of the satellite [m]")
      ->capture_default_str()
      ->check(CLI::Range(-43620761.6, 43620759.3));
  add_option(app, "--vel_x", ephemeris_info.velocity_vx, "X Velocity of the satellite [m/s]")
      ->capture_default_str()
      ->check(CLI::Range(-7864.32, 7864.26));
  add_option(app, "--vel_y", ephemeris_info.velocity_vy, "Y Velocity of the satellite [m/s]")
      ->capture_default_str()
      ->check(CLI::Range(-7864.32, 7864.26));
  add_option(app, "--vel_z", ephemeris_info.velocity_vz, "Z Velocity of the satellite [m/s]")
      ->capture_default_str()
      ->check(CLI::Range(-7864.32, 7864.26));
}

static void configure_cli11_ephemeris_info_orbital(CLI::App& app, orbital_coordinates_t& ephemeris_info)
{
  add_option(app, "--semi_major_axis", ephemeris_info.semi_major_axis, "Semi-major axis of the satellite [m]")
      ->capture_default_str()
      ->check(CLI::Range(6500000.0, 42998632.07));
  add_option(app, "--eccentricity", ephemeris_info.eccentricity, "Eccentricity of the satellite [-]")
      ->capture_default_str()
      ->check(CLI::Range(0.0, 0.01500510825));
  add_option(app, "--periapsis", ephemeris_info.periapsis, "Periapsis of the satellite [rad]")
      ->capture_default_str()
      ->check(CLI::Range(0.0, 6.28407400155));
  add_option(app, "--longitude", ephemeris_info.longitude, "Longitude of the satellites angle of ascending node [rad]")
      ->capture_default_str()
      ->check(CLI::Range(0.0, 6.28407400155));
  add_option(app, "--inclination", ephemeris_info.inclination, "Inclination of the satellite [rad]")
      ->capture_default_str()
      ->check(CLI::Range(-1.57101850624, 1.57101848283));
  add_option(app, "--mean_anomaly", ephemeris_info.mean_anomaly, "Mean anomaly of the satellite [rad]")
      ->capture_default_str()
      ->check(CLI::Range(0.0, 6.28407400155));
}

void srsran::configure_cli11_ntn_args(CLI::App& app, ntn_config& config)
{
  add_option(app, "--cell_specific_koffset", config.cell_specific_koffset, "Cell-specific k-offset to be used for NTN.")
      ->capture_default_str()
      ->check(CLI::Range(0, 1023));

  app.add_option("--ntn_ul_sync_validity_dur", config.ntn_ul_sync_validity_dur, "An UL sync validity duration")
      ->capture_default_str()
      ->check(CLI::IsMember({5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 120, 180, 240, 900}));

  // Epoch time.
  static epoch_time_t epoch_time;
  CLI::App* epoch_time_subcmd = add_subcommand(app, "epoch_time", "Epoch time for the NTN assistance information");
  configure_cli11_epoch_time(*epoch_time_subcmd, epoch_time);
  epoch_time_subcmd->parse_complete_callback([&]() {
    if (app.get_subcommand("epoch_time")->count() != 0) {
      config.epoch_time = epoch_time;
    }
  });

  // TA-info
  static ta_info_t ta_info;
  CLI::App*        ta_info_subcmd = add_subcommand(app, "ta_info", "TA Info for the NTN assistance information");
  configure_cli11_ta_info(*ta_info_subcmd, ta_info);
  ta_info_subcmd->parse_complete_callback([&]() {
    if (app.get_subcommand("ta_info")->count() != 0) {
      config.ta_info = ta_info;
    }
  });

  // Ephemeris configuration: ECEF state vector.
  static ecef_coordinates_t ecef_coordinates;
  CLI::App*                 ephem_subcmd_ecef =
      add_subcommand(app, "ephemeris_info_ecef", "Ephermeris information of the satellite in ecef coordinates");
  configure_cli11_ephemeris_info_ecef(*ephem_subcmd_ecef, ecef_coordinates);
  ephem_subcmd_ecef->parse_complete_callback([&]() {
    if (app.get_subcommand("ephemeris_info_ecef")->count() != 0) {
      config.ephemeris_info = ecef_coordinates;
    }
  });

  // Ephemeris configuration: Orbital parameters.
  static orbital_coordinates_t orbital_coordinates;
  CLI::App*                    ephem_subcmd_orbital =
      add_subcommand(app, "ephemeris_orbital", "Ephermeris information of the satellite in orbital coordinates");
  configure_cli11_ephemeris_info_orbital(*ephem_subcmd_orbital, orbital_coordinates);
  ephem_subcmd_orbital->parse_complete_callback([&]() {
    if (app.get_subcommand("ephemeris_orbital")->count() != 0) {
      config.ephemeris_info = orbital_coordinates;
    }
  });

  configure_cli11_advanced_ntn_args(app, config);
}
