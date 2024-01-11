#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Launch tests in Viavi
"""

import logging
from pathlib import Path

import pytest
from pytest import mark, param
from requests import HTTPError
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.public import MetricServerInfo
from retina.launcher.utils import configure_artifacts
from retina.protocol.base_pb2 import FiveGCDefinition, PLMN, StartInfo
from retina.protocol.gnb_pb2 import GNBStartInfo
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.viavi.client import CampaignStatusEnum, Viavi

from .steps.configuration import configure_metric_server_for_gnb
from .steps.stub import GNB_STARTUP_TIMEOUT, handle_start_error, stop


@mark.parametrize(
    "campaign_filename, test_name, test_timeout, post_commands",
    (
        param("C:\\ci\\CI 4x4 ORAN-FH.xml", "1UE static DL + UL UDP - Dell", 45 * 60, "", id="1UE Bidirectional UDP"),
        param(
            "C:\\ci\\CI 4x4 ORAN-FH.xml",
            "32UE static DL + UL UDP - Dell",
            45 * 60,
            "cell_cfg pdsch --max_rb_size 45",
            id="32UE Bidirectional UDP",
        ),
    ),
)
@mark.viavi
# pylint: disable=too-many-arguments, too-many-locals
def test_viavi(
    # Retina
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    test_log_folder: str,
    # Clients
    gnb: GNBStub,
    viavi: Viavi,
    metrics_server: MetricServerInfo,
    # Test info
    campaign_filename: str,
    test_name: str,
    test_timeout: int,
    post_commands: str,
    # Test extra params
    always_download_artifacts: bool = True,
    gnb_startup_timeout: int = GNB_STARTUP_TIMEOUT,
    gnb_stop_timeout: int = 0,
    log_search: bool = True,
    warning_as_errors: bool = True,
):
    """
    Runs a test using Viavi
    """

    retina_data.test_config = {
        "gnb": {
            "parameters": {
                "gnb_id": 1,
                "log_level": "warning",
                "pcap": False,
            },
            "templates": {"cell": str(Path(__file__).joinpath("../viavi/config.yml").resolve())},
        },
    }
    configure_metric_server_for_gnb(
        retina_manager=retina_manager, retina_data=retina_data, metrics_server=metrics_server
    )
    retina_manager.parse_configuration(retina_data.test_config)
    retina_manager.push_all_config()

    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=always_download_artifacts,
    )

    # Start the GNB
    amf_ip, amf_port = viavi.get_core_definition()
    with handle_start_error(name=f"GNB [{id(gnb)}]"):
        # GNB Start
        gnb.Start(
            GNBStartInfo(
                plmn=PLMN(mcc="001", mnc="01"),
                fivegc_definition=FiveGCDefinition(amf_ip=amf_ip, amf_port=amf_port),
                start_info=StartInfo(timeout=gnb_startup_timeout, post_commands=post_commands),
            )
        )

    # Create campaign
    logging.info(f"Starting Campaign {campaign_filename}" + (f" - Test {test_name}" if test_name is not None else ""))
    campaign_name = viavi.schedule_campaign(campaign_filename, test_name)

    # Start campaign
    viavi.run_campaign(campaign_name)
    logging.info("Campaign started")

    # Wait until end
    try:
        info = viavi.wait_until_running_campaign_finishes(test_timeout)
        if info.status is not CampaignStatusEnum.PASS:
            pytest.fail(f"Viavi Test Failed: {info.message}")
        # Final stop
        stop(
            (),
            gnb,
            None,
            retina_data,
            gnb_stop_timeout=gnb_stop_timeout,
            log_search=log_search,
            warning_as_errors=warning_as_errors,
            fail_if_kos=False,
        )

    # This except and the finally should be inside the request, but the campaign_name makes it complicated
    except (TimeoutError, KeyboardInterrupt):
        logging.info("Stopping test due to timeout")
        viavi.stop_running_campaign()
        pytest.fail("Viavi Test did not end in the expected timeout")

    finally:
        try:
            logging.info("Generating Viavi report")
            report_folder = viavi.generate_report(campaign_name)
            logging.info("Downloading Viavi report")
            viavi.download_directory(report_folder, Path(test_log_folder).joinpath("viavi"))
        except HTTPError:
            logging.error("Viavi Reports could not be downloaded")
