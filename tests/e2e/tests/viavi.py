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
Launch tests in Viavi
"""

import logging
from pathlib import Path
from typing import Optional

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


@pytest.fixture
def viavi_manual_campaign_filename(request):
    """
    Campaign filename
    """
    return request.config.getoption("viavi_manual_campaign_filename")


@pytest.fixture
def viavi_manual_test_name(request):
    """
    Test name
    """
    return request.config.getoption("viavi_manual_test_name")


@pytest.fixture
def viavi_manual_test_timeout(request):
    """
    Test timeout
    """
    return request.config.getoption("viavi_manual_test_timeout")


@mark.viavi_manual
# pylint: disable=too-many-arguments, too-many-locals
def test_viavi_manual(
    # Retina
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    test_log_folder: str,
    # Clients
    gnb: GNBStub,
    viavi: Viavi,
    # Test info
    viavi_manual_campaign_filename: str,  # pylint: disable=redefined-outer-name
    viavi_manual_test_name: str,  # pylint: disable=redefined-outer-name
    viavi_manual_test_timeout: int,  # pylint: disable=redefined-outer-name
    # Test extra params
    always_download_artifacts: bool = True,
    gnb_startup_timeout: int = GNB_STARTUP_TIMEOUT,
    gnb_stop_timeout: int = 0,
    log_search: bool = True,
):
    """
    Runs a test using Viavi
    """
    _test_viavi(
        # Retina
        retina_manager=retina_manager,
        retina_data=retina_data,
        test_log_folder=test_log_folder,
        # Clients
        gnb=gnb,
        viavi=viavi,
        metrics_server=None,
        # Test info
        campaign_filename=viavi_manual_campaign_filename,
        test_name=viavi_manual_test_name,
        test_timeout=viavi_manual_test_timeout,
        # Test extra params
        always_download_artifacts=always_download_artifacts,
        gnb_startup_timeout=gnb_startup_timeout,
        gnb_stop_timeout=gnb_stop_timeout,
        log_search=log_search,
        max_rb_size=-1,
    )


@mark.parametrize(
    "campaign_filename, test_name, test_timeout, post_commands, max_rb_size",
    (
        param(
            "C:\\ci\\CI 4x4 ORAN-FH.xml", "1UE static DL + UL UDP - Dell", 45 * 60, "", -1, id="1UE Bidirectional UDP"
        ),
        param(
            "C:\\ci\\CI 4x4 ORAN-FH.xml",
            "32UE static DL + UL UDP - Dell",
            45 * 60,
            "",
            45,
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
    max_rb_size: int,
    # Test extra params
    always_download_artifacts: bool = True,
    gnb_startup_timeout: int = GNB_STARTUP_TIMEOUT,
    gnb_stop_timeout: int = 0,
    log_search: bool = True,
):
    """
    Runs a test using Viavi
    """
    _test_viavi(
        # Retina
        retina_manager=retina_manager,
        retina_data=retina_data,
        test_log_folder=test_log_folder,
        # Clients
        gnb=gnb,
        viavi=viavi,
        metrics_server=metrics_server,
        # Test info
        campaign_filename=campaign_filename,
        test_name=test_name,
        test_timeout=test_timeout,
        # Test extra params
        always_download_artifacts=always_download_artifacts,
        gnb_startup_timeout=gnb_startup_timeout,
        gnb_stop_timeout=gnb_stop_timeout,
        log_search=log_search,
        post_commands=post_commands,
        max_rb_size=max_rb_size,
    )


# pylint: disable=too-many-arguments, too-many-locals
def _test_viavi(
    # Retina
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    test_log_folder: str,
    # Clients
    gnb: GNBStub,
    viavi: Viavi,
    metrics_server: Optional[MetricServerInfo],
    # Test info
    campaign_filename: str,
    test_name: str,
    test_timeout: int,
    # Test extra params
    always_download_artifacts: bool = True,
    gnb_startup_timeout: int = GNB_STARTUP_TIMEOUT,
    gnb_stop_timeout: int = 0,
    log_search: bool = True,
    warning_as_errors: bool = False,
    post_commands: str = "",
    max_rb_size: int = -1,
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
                "max_rb_size": max_rb_size,
                "dl_arfcn": 625000,
                "band": 78,
                "channel_bandwidth_MHz": 100,
                "common_scs": 30,
                "tac": 7,
                "pci": 1,
                "nof_antennas_dl": 4,
                "nof_antennas_ul": 1,
                "prach_config_index": 159,
            },
            "templates": {"extra": str(Path(__file__).joinpath("../viavi/config.yml").resolve())},
        },
    }
    if metrics_server is not None:
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
                start_info=StartInfo(
                    timeout=gnb_startup_timeout,
                    post_commands=post_commands,
                ),
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
