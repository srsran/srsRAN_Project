#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
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
    pcap: Optional[bool] = None,
    gtpu_enable: Optional[bool] = None,
    common_search_space_enable: bool = False,
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
            },
        },
    }
    if pcap is not None:
        retina_data.test_config["gnb"]["parameters"]["pcap"] = pcap
    if gtpu_enable is not None:
        retina_data.test_config["gnb"]["parameters"]["gtpu_enable"] = gtpu_enable
    if sample_rate is not None:
        retina_data.test_config["ue"]["parameters"]["sample_rate"] = sample_rate
        retina_data.test_config["gnb"]["parameters"]["sample_rate"] = sample_rate
    if _is_tdd(band):
        retina_data.test_config["ue"]["parameters"]["rx_ant"] = "rx"

    logging.info("Test config: \n%s", pformat(retina_data.test_config))
    retina_manager.parse_configuration(retina_data.test_config)
    retina_manager.push_all_config()


def _is_tdd(band: int) -> bool:
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
            lambda: 368410,
            {
                30: 367450,
                40: 366490,
                50: 365530,
            },
        ),
        7: defaultdict(
            lambda: 535930,
            {
                30: 534970,
                40: 534010,
                50: 533050,
            },
        ),
        41: defaultdict(
            lambda: 519870,
            {
                20: 520110,
                30: 518910,
                40: 517950,
                50: 517230,
            },
        ),
        78: defaultdict(lambda: 632544),
    }[band][bandwidth]


def get_minimum_sample_rate_for_bandwidth(bandwidth: int) -> int:
    """
    Get the smallest sample rate for the selected bandwidth
    """
    f_s_list = [5.76, 7.68, 11.52, 15.36, 23.04, 30.72, 61.44, 122.88, 245.76]
    f_s_min = int(1e6 * min(filter(lambda f: f > bandwidth, f_s_list)))
    return f_s_min
