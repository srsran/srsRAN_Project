#
# Copyright 2021-2024 Software Radio Systems Limited
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
from collections import defaultdict
from pprint import pformat
from typing import Optional, Union

from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.public import MetricServerInfo


# pylint: disable=too-many-arguments
def configure_test_parameters(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    band: int,
    common_scs: int,
    bandwidth: int,
    sample_rate: Optional[int],
    global_timing_advance: int,
    time_alignment_calibration: Union[int, str],
    gtpu_enable: Optional[bool] = None,
    common_search_space_enable: bool = False,
    prach_config_index: int = -1,
    log_ip_level="",
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
            },
        },
    }
    if gtpu_enable is not None and gtpu_enable:
        retina_data.test_config["gnb"]["parameters"]["pcap"] = True
        retina_data.test_config["gnb"]["parameters"]["gtpu_enable"] = True
    if sample_rate is not None:
        retina_data.test_config["ue"]["parameters"]["sample_rate"] = sample_rate
        retina_data.test_config["gnb"]["parameters"]["sample_rate"] = sample_rate
    if is_tdd(band):
        retina_data.test_config["ue"]["parameters"]["rx_ant"] = "rx"

    metrics_server = None
    for node_name in retina_manager.get_testbed_info().get("generic", {}).keys():
        if "metrics-server" in node_name:
            metrics_server = retina_manager.get_testbed_info()["generic"][node_name]

    if metrics_server is not None and metrics_server.metadata.get("ip", None) is not None:
        retina_data.test_config["gnb"]["parameters"]["metrics_hostname"] = metrics_server.metadata["ip"]
        retina_data.test_config["gnb"]["parameters"]["metrics_port"] = metrics_server.port
        logging.info("Metrics Server in %s:%s will be used for this test.", metrics_server.address, metrics_server.port)

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
    return {3: 368500, 7: 536020, 41: 520002, 78: 632628}[band]


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
    }[band][bandwidth]


def get_minimum_sample_rate_for_bandwidth(bandwidth: int) -> int:
    """
    Get the smallest sample rate for the selected bandwidth
    """
    f_s_list = [5.76, 7.68, 11.52, 15.36, 23.04, 30.72, 61.44, 122.88, 245.76]
    f_s_min = int(1e6 * min(filter(lambda f: f > bandwidth, f_s_list)))
    return f_s_min


def configure_metric_server_for_gnb(
    retina_manager: RetinaTestManager, retina_data: RetinaTestData, metrics_server: MetricServerInfo
):
    """
    Set parameters to set up a metrics server
    """

    if "gnb" not in retina_data.test_config:
        retina_data.test_config["gnb"] = {}
    if "parameters" not in retina_data.test_config["gnb"]:
        retina_data.test_config["gnb"]["parameters"] = {}

    retina_data.test_config["gnb"]["parameters"]["metrics_hostname"] = metrics_server.address
    retina_data.test_config["gnb"]["parameters"]["metrics_port"] = metrics_server.port

    logging.info("Test config: \n%s", pformat(retina_data.test_config))
    retina_manager.parse_configuration(retina_data.test_config)
    retina_manager.push_all_config()
