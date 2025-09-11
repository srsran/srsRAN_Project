#
# Copyright 2021-2025 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

"""
Configuration related steps
"""

import logging
import socket
from collections import defaultdict
from pprint import pformat
from typing import List, NamedTuple, Optional, Tuple, Union

from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.public import MetricServerInfo
from retina.protocol.channel_emulator_pb2 import EphemerisInfoType, NtnScenarioConfig, NtnScenarioType


def configure_ntn_parameters(
    *, retina_data: RetinaTestData, ntn_config: NtnScenarioConfig  # The "*" enforces keyword-only arguments
):
    """
    Configure test NTN parameters
    """
    retina_data.test_config["gnb"]["parameters"]["ntn_enable"] = True
    # CU NTN parameters.
    if ntn_config.scenario_type == NtnScenarioType.GEO:
        retina_data.test_config["gnb"]["parameters"]["cu_cp_inactivity_timer"] = 120
        retina_data.test_config["gnb"]["parameters"]["request_pdu_session_timeout"] = 12
        retina_data.test_config["gnb"]["parameters"]["rrc_procedure_guard_time_ms"] = 12800
    elif ntn_config.scenario_type == NtnScenarioType.MEO:
        retina_data.test_config["gnb"]["parameters"]["cu_cp_inactivity_timer"] = 90
        retina_data.test_config["gnb"]["parameters"]["request_pdu_session_timeout"] = 9
        retina_data.test_config["gnb"]["parameters"]["rrc_procedure_guard_time_ms"] = 12800
    else:  # LEO
        retina_data.test_config["gnb"]["parameters"]["cu_cp_inactivity_timer"] = 60
        retina_data.test_config["gnb"]["parameters"]["request_pdu_session_timeout"] = 6
        retina_data.test_config["gnb"]["parameters"]["rrc_procedure_guard_time_ms"] = 10000

    # DU NTN parameters.
    retina_data.test_config["gnb"]["parameters"]["sib19"] = {}
    retina_data.test_config["gnb"]["parameters"]["sib19"][
        "ntn_ul_sync_validity_dur"
    ] = ntn_config.sib19_cfg.ntn_ul_sync_validity_dur
    retina_data.test_config["gnb"]["parameters"]["sib19"][
        "cell_specific_koffset"
    ] = ntn_config.sib19_cfg.cell_specific_koffset
    retina_data.test_config["gnb"]["parameters"]["sib19"]["ta_common"] = ntn_config.sib19_cfg.ta_common
    retina_data.test_config["gnb"]["parameters"]["sib19"]["ta_common_drift"] = ntn_config.sib19_cfg.ta_common_drift
    retina_data.test_config["gnb"]["parameters"]["sib19"][
        "ta_common_drift_variant"
    ] = ntn_config.sib19_cfg.ta_common_drift_variant
    if ntn_config.sib19_cfg.ephemeris_info_type == EphemerisInfoType.ORBITAL:
        retina_data.test_config["gnb"]["parameters"]["sib19"]["use_ephemeris_orbital"] = True
        retina_data.test_config["gnb"]["parameters"]["sib19"][
            "semi_major_axis"
        ] = ntn_config.sib19_cfg.ephemeris_orbital.semi_major_axis
        retina_data.test_config["gnb"]["parameters"]["sib19"][
            "eccentricity"
        ] = ntn_config.sib19_cfg.ephemeris_orbital.eccentricity
        retina_data.test_config["gnb"]["parameters"]["sib19"][
            "periapsis"
        ] = ntn_config.sib19_cfg.ephemeris_orbital.periapsis
        retina_data.test_config["gnb"]["parameters"]["sib19"][
            "longitude"
        ] = ntn_config.sib19_cfg.ephemeris_orbital.longitude
        retina_data.test_config["gnb"]["parameters"]["sib19"][
            "inclination"
        ] = ntn_config.sib19_cfg.ephemeris_orbital.inclination
        retina_data.test_config["gnb"]["parameters"]["sib19"][
            "mean_anomaly"
        ] = ntn_config.sib19_cfg.ephemeris_orbital.mean_anomaly
    else:
        retina_data.test_config["gnb"]["parameters"]["sib19"]["use_ephemeris_orbital"] = False
        retina_data.test_config["gnb"]["parameters"]["sib19"]["pos_x"] = ntn_config.sib19_cfg.ephemeris_info_ecef.pos_x
        retina_data.test_config["gnb"]["parameters"]["sib19"]["pos_y"] = ntn_config.sib19_cfg.ephemeris_info_ecef.pos_y
        retina_data.test_config["gnb"]["parameters"]["sib19"]["pos_z"] = ntn_config.sib19_cfg.ephemeris_info_ecef.pos_z
        retina_data.test_config["gnb"]["parameters"]["sib19"]["vel_x"] = ntn_config.sib19_cfg.ephemeris_info_ecef.vel_x
        retina_data.test_config["gnb"]["parameters"]["sib19"]["vel_y"] = ntn_config.sib19_cfg.ephemeris_info_ecef.vel_y
        retina_data.test_config["gnb"]["parameters"]["sib19"]["vel_z"] = ntn_config.sib19_cfg.ephemeris_info_ecef.vel_z

    retina_data.test_config["gnb"]["parameters"]["ta_target"] = ntn_config.ta_cfg.ta_target
    retina_data.test_config["gnb"]["parameters"][
        "ta_meas_slot_prohibit_period"
    ] = ntn_config.ta_cfg.slot_prohibit_period
    retina_data.test_config["gnb"]["parameters"]["ta_meas_slot_period"] = ntn_config.ta_cfg.slot_meas_period
    retina_data.test_config["gnb"]["parameters"]["ta_cmd_offset_threshold"] = ntn_config.ta_cfg.ta_cmd_offset_threshold

    # UE NTN parameters.
    retina_data.test_config["ue"]["parameters"]["ntn_enable"] = True
    retina_data.test_config["ue"]["parameters"]["latitude"] = ntn_config.ue_position.latitude
    retina_data.test_config["ue"]["parameters"]["longitude"] = ntn_config.ue_position.longitude
    retina_data.test_config["ue"]["parameters"]["altitude"] = ntn_config.ue_position.altitude


# pylint: disable=too-many-arguments,too-many-positional-arguments
# pylint: disable=too-many-locals
def configure_test_parameters(
    *,  # This enforces keyword-only arguments
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    band: int,
    common_scs: int,
    bandwidth: int,
    sample_rate: Optional[int],
    global_timing_advance: int,
    time_alignment_calibration: Union[int, str],
    n3_enable: Optional[bool] = None,
    common_search_space_enable: bool = False,
    prach_config_index: int = -1,
    log_ip_level: str = "",
    ul_noise_spd: int = 0,
    noise_spd: int = 0,
    enable_qos_reestablishment: bool = False,
    num_cells: int = 1,
    cell_position_offset: Tuple[float, float, float] = (1000, 0, 0),
    enable_security_mode: bool = False,
    rx_to_tx_latency: int = -1,
    enable_dddsu: bool = False,
    nof_antennas_dl: int = 1,
    nof_antennas_ul: int = 1,
    ims_mode: str = "",
    enable_drx: bool = False,
    pdsch_mcs_table: str = "qam256",
    pusch_mcs_table: str = "qam256",
    cu_cp_inactivity_timer: int = -1,
    use_format_0: bool = False,
    pucch_set1_format: int = 2,
    pdsch_interleaving_bundle_size: int = 0,
    ntn_config: Optional[NtnScenarioConfig] = None,
    pdcch_log: bool = False,
    slices: Optional[List[dict]] = None,
    ue_sds: Optional[List[str]] = None,
    warning_allowlist: Optional[List[str]] = None,
):
    """
    Configure test parameters
    """
    retina_data.test_config = {
        "ue": {
            "parameters": {
                "band": band,
                "dl_arfcn": _get_dl_arfcn(band),
                "ssb_arfcn": _get_ssb_arfcn(band, bandwidth),
                "common_scs": common_scs,
                "ssb_scs": common_scs,
                "bandwidth": bandwidth,
                "global_timing_advance": global_timing_advance,
                "log_ip_level": log_ip_level,
                "ul_noise_spd": ul_noise_spd,
                "noise_spd": noise_spd,
                "num_cells": num_cells,
                "cell_position_offset": cell_position_offset,
                "rx_to_tx_latency": rx_to_tx_latency,
                "nof_antennas_dl": nof_antennas_dl,
                "nof_antennas_ul": nof_antennas_ul,
                "pdcch_log": pdcch_log,
                "ue_sds": ue_sds if ue_sds is not None else [],
            },
        },
        "gnb": {
            "parameters": {
                "band": band,
                "dl_arfcn": _get_dl_arfcn(band),
                "common_scs": common_scs,
                "bandwidth": bandwidth,
                "time_alignment_calibration": time_alignment_calibration,
                "common_search_space_enable": common_search_space_enable,
                "prach_config_index": prach_config_index,
                "enable_channel_noise": noise_spd != 0,
                "enable_qos_reestablishment": enable_qos_reestablishment,
                "num_cells": num_cells,
                "enable_security_mode": enable_security_mode,
                "enable_dddsu": enable_dddsu,
                "nof_antennas_dl": nof_antennas_dl,
                "nof_antennas_ul": nof_antennas_ul,
                "enable_drx": enable_drx,
                "pdsch_mcs_table": pdsch_mcs_table,
                "pusch_mcs_table": pusch_mcs_table,
                "cu_cp_inactivity_timer": cu_cp_inactivity_timer,
                "use_format_0": use_format_0,
                "pucch_set1_format": pucch_set1_format,
                "pdsch_interleaving_bundle_size": pdsch_interleaving_bundle_size,
                "slices": slices if slices is not None else [],
                "warning_extra_regex": (
                    (r"(?!.*" + r")(?!.*".join(warning_allowlist) + r")") if warning_allowlist else ""
                ),
            },
        },
        "du": {
            "parameters": {
                "band": band,
                "dl_arfcn": _get_dl_arfcn(band),
                "common_scs": common_scs,
                "bandwidth": bandwidth,
                "time_alignment_calibration": time_alignment_calibration,
                "common_search_space_enable": common_search_space_enable,
                "prach_config_index": prach_config_index,
                "enable_channel_noise": noise_spd != 0,
                "enable_qos_reestablishment": enable_qos_reestablishment,
                "enable_dddsu": enable_dddsu,
                "nof_antennas_dl": nof_antennas_dl,
                "nof_antennas_ul": nof_antennas_ul,
                "enable_drx": enable_drx,
                "pdsch_mcs_table": pdsch_mcs_table,
                "pusch_mcs_table": pusch_mcs_table,
                "use_format_0": use_format_0,
                "pucch_set1_format": pucch_set1_format,
                "pdsch_interleaving_bundle_size": pdsch_interleaving_bundle_size,
                "slices": slices if slices is not None else [],
            },
        },
        "cu": {
            "parameters": {
                "enable_security_mode": enable_security_mode,
                "cu_cp_inactivity_timer": cu_cp_inactivity_timer,
                "num_cells": num_cells,
            },
        },
        "5gc": {
            "parameters": {
                "ims_mode": ims_mode,
                "slices": [slice["sd"] for slice in slices] if slices is not None else [],
            }
        },
    }
    if n3_enable is not None and n3_enable:
        retina_data.test_config["gnb"]["parameters"]["pcap"] = True
        retina_data.test_config["gnb"]["parameters"]["n3_enable"] = True
    if sample_rate is not None:
        retina_data.test_config["ue"]["parameters"]["sample_rate"] = sample_rate
        retina_data.test_config["gnb"]["parameters"]["sample_rate"] = sample_rate
    if is_tdd(band):
        retina_data.test_config["ue"]["parameters"]["rx_ant"] = "rx"

    if ntn_config is not None:
        configure_ntn_parameters(retina_data=retina_data, ntn_config=ntn_config)

    for node_name in retina_manager.get_testbed_info().get("generic", {}).keys():
        if "metrics-server" in node_name:
            metrics_server_dict = retina_manager.get_testbed_info()["generic"][node_name]
            metrics_server = MetricServerInfo(metrics_server_dict.metadata["ip"], metrics_server_dict.port)
            logging.info(
                "Metrics Server in %s:%s will be used for this test.", metrics_server.address, metrics_server.port
            )
            configure_metric_server_for_gnb(retina_manager=retina_manager, metrics_server=metrics_server)

    logging.info("Test config: \n%s", pformat(retina_data.test_config))
    retina_manager.parse_configuration(retina_data.test_config)
    retina_manager.push_all_config()


def is_tdd(band: int) -> bool:
    """
    Return True if the band is tdd
    """
    return band in (41, 78)


def _get_dl_arfcn(band: int) -> int:
    """
    Get dl arfcn
    """
    return {3: 368500, 7: 536020, 41: 520002, 78: 632628, 256: 437000}[band]


def _get_ul_arfcn(band: int) -> int:
    """
    Get dl arfcn
    """
    return {3: 349500, 7: 512020, 41: 520002, 78: 632628, 256: 399000}[band]


def _get_ssb_arfcn(band: int, bandwidth: int) -> int:
    """
    Get SSB arfcn
    """
    return {  # type: ignore
        3: defaultdict(
            lambda: 368500,
            {
                5: 368410,
                10: 367930,
                20: 366970,
                30: 366010,
                40: 365050,
                50: 364090,
            },
        ),
        7: defaultdict(
            lambda: 535930,
            {
                20: 534490,
                30: 533530,
                40: 532570,
                50: 531610,
            },
        ),
        41: defaultdict(
            lambda: 519870,
            {
                20: 518910,
                30: 517950,
                40: 516990,
                50: 516030,
                100: 511950,
            },
        ),
        78: defaultdict(
            lambda: 632544,
            {
                20: 632256,
                30: 631968,
                40: 631680,
                50: 631296,
            },
        ),
        256: defaultdict(
            lambda: 437090,
            {
                5: 437090,
            },
        ),
    }[band][bandwidth]


def get_minimum_sample_rate_for_bandwidth(bandwidth: int) -> int:
    """
    Get the smallest sample rate for the selected bandwidth
    """
    f_s_list = [5.76, 7.68, 11.52, 15.36, 23.04, 30.72, 61.44, 122.88, 245.76]
    f_s_min = int(1e6 * min(filter(lambda f: f > bandwidth, f_s_list)))
    return f_s_min


def configure_metric_server_for_gnb(
    *, retina_manager: RetinaTestManager, metrics_server: MetricServerInfo  # The "*" enforces keyword-only arguments
):
    """
    Report gnb ip and port to the metrics-server configuration
    """
    # Send a UDP packet to the metrics-server with the gnb's ip:port

    for item in retina_manager.get_testbed_info().get("gnb", {}).values():
        gnb_ip = item.address
        break

    message = f"{gnb_ip}:8001"

    try:
        # Create UDP socket and send message to metrics server
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(message.encode("utf-8"), (metrics_server.address, metrics_server.port))
        sock.close()
        logging.info("Sent gnb info '%s' to metrics server %s:%s", message, metrics_server.address, metrics_server.port)
    except socket.error as e:
        logging.error("Failed to send gnb info to metrics server: %s", e)


class NrRasterParams(NamedTuple):
    """
    Class to store NR-ARFCN parameters for the global frequency raster,
    as per Table 5.4.2.1-1, TS 38.104, Rel. 17, version 17.8.0.
    """

    F_low: float  # Lower frequency in MHz
    F_high: float  # Upper frequency in MHz
    delta_F_kHz: int  # Frequency step in kHz
    F_REF_Offs_MHz: float
    N_REF_Offs: int
    N_REF_min: int
    N_REF_max: int


nr_fr_params: List[NrRasterParams] = [
    NrRasterParams(0, 3000, 5, 0.0, 0, 0, 599999),
    NrRasterParams(3000, 24250, 15, 3000.0, 600000, 600000, 2016666),
    NrRasterParams(24250, 100000, 60, 24250.08, 2016667, 2016667, 3279165),
]


def get_raster_params(nr_arfcn: int) -> Optional[NrRasterParams]:
    """
    Find the corresponding raster parameters for the given NR ARFCN.
    """
    for fr in nr_fr_params:
        if fr.N_REF_min <= nr_arfcn <= fr.N_REF_max:
            return fr
    return None


def nr_arfcn_to_freq(nr_arfcn: int) -> float:
    """
    Convert NR ARFCN to frequency in Hz using the raster parameters.
    """
    params = get_raster_params(nr_arfcn)
    if params is None:
        return 0.0

    return int(params.F_REF_Offs_MHz * 1e6 + params.delta_F_kHz * (nr_arfcn - params.N_REF_Offs) * 1e3)
