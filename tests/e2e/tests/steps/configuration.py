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
from typing import Union

from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData


# pylint: disable=too-many-arguments
def configure_test_parameters(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    band: int,
    common_scs: int,
    bandwidth: int,
    global_timing_advance: int,
    time_alignment_calibration: Union[int, str],
    mcs: int = 10,
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
                "mcs": mcs,
                "time_alignment_calibration": time_alignment_calibration,
            },
        },
    }
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
