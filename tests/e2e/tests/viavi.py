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
import operator
from dataclasses import dataclass
from enum import Enum
from pathlib import Path
from typing import Callable, Optional

import pytest
from pytest import mark, param
from requests import HTTPError
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.public import MetricServerInfo, MetricsSummary
from retina.launcher.utils import configure_artifacts
from retina.protocol.base_pb2 import FiveGCDefinition, PLMN, StartInfo
from retina.protocol.gnb_pb2 import GNBStartInfo
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.viavi.client import CampaignStatusEnum, Viavi

from .steps.configuration import configure_metric_server_for_gnb
from .steps.stub import get_metrics, GNB_STARTUP_TIMEOUT, GnbMetrics, handle_start_error, stop

CAMPAIGN_FILENAME = "C:\\ci\\CI 4x4 ORAN-FH.xml"
_OMIT_VIAVI_FAILURE_LIST = ["authentication"]


class _TestName(Enum):
    """
    Test names
    """

    UE1_STATIC_DL_UL_UDP = "1UE static DL + UL UDP - Dell"
    UE32_STATIC_DL_UL_UDP = "32UE static DL + UL UDP - Dell"


@dataclass
class _ViaviConfiguration:
    """
    Viavi configuration
    """

    max_pdschs_per_slot: int = 1
    max_puschs_per_slot: int = 1
    enable_qos_viavi: bool = False
    warning_as_errors: bool = True
    expected_ul_bitrate: float = 0
    expected_dl_bitrate: float = 0


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
        metrics_summary=None,
        campaign_filename=viavi_manual_campaign_filename,
        test_name=viavi_manual_test_name,
        test_timeout=viavi_manual_test_timeout,
        # Test extra params
        always_download_artifacts=always_download_artifacts,
        gnb_startup_timeout=gnb_startup_timeout,
        gnb_stop_timeout=gnb_stop_timeout,
        log_search=log_search,
    )


@mark.parametrize(
    "test_name, test_timeout, post_commands",
    (
        param(
            _TestName.UE1_STATIC_DL_UL_UDP.value,
            45 * 60,
            "log --mac_level=info",
            id="1UE Bidirectional UDP",
        ),
        param(
            _TestName.UE32_STATIC_DL_UL_UDP.value,
            45 * 60,
            "log --mac_level=info",
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
    metrics_summary: MetricsSummary,
    test_name: str,
    test_timeout: int,
    post_commands: str,
    # Test extra params
    warning_as_errors: bool = True,
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
        metrics_summary=metrics_summary,
        campaign_filename=CAMPAIGN_FILENAME,
        test_name=test_name,
        test_timeout=test_timeout,
        # Test extra params
        warning_as_errors=warning_as_errors,
        always_download_artifacts=always_download_artifacts,
        gnb_startup_timeout=gnb_startup_timeout,
        gnb_stop_timeout=gnb_stop_timeout,
        log_search=log_search,
        post_commands=post_commands,
    )


@mark.parametrize(
    "test_name, test_timeout, post_commands",
    (
        param(
            _TestName.UE32_STATIC_DL_UL_UDP.value,
            45 * 60,
            "log --mac_level=info",
            id="32UE Bidirectional UDP",
        ),
    ),
)
@mark.viavi_debug
# pylint: disable=too-many-arguments, too-many-locals
def test_viavi_debug(
    # Retina
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    test_log_folder: str,
    # Clients
    gnb: GNBStub,
    viavi: Viavi,
    metrics_server: MetricServerInfo,
    # Test info
    metrics_summary: MetricsSummary,
    test_name: str,
    test_timeout: int,
    post_commands: str,
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
        metrics_summary=metrics_summary,
        campaign_filename=CAMPAIGN_FILENAME,
        test_name=test_name,
        test_timeout=test_timeout,
        # Test extra params
        always_download_artifacts=always_download_artifacts,
        gnb_startup_timeout=gnb_startup_timeout,
        gnb_stop_timeout=gnb_stop_timeout,
        log_search=log_search,
        post_commands=post_commands,
        warning_as_errors=False,
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
    metrics_summary: Optional[MetricsSummary],
    campaign_filename: str,
    test_name: str,
    test_timeout: int,
    # Test extra params
    warning_as_errors: bool = True,
    always_download_artifacts: bool = True,
    gnb_startup_timeout: int = GNB_STARTUP_TIMEOUT,
    gnb_stop_timeout: int = 0,
    log_search: bool = True,
    post_commands: str = "",
    fail_if_kos: bool = True,
):
    """
    Runs a test using Viavi
    """
    test_configuration = get_viavi_configuration(test_name, warning_as_errors)

    retina_data.test_config = {
        "gnb": {
            "parameters": {
                "gnb_id": 1,
                "log_level": "warning",
                "pcap": False,
                "max_rb_size": -1,
                "dl_arfcn": 625000,
                "band": 78,
                "bandwidth": 100,
                "common_scs": 30,
                "tac": 7,
                "pci": 1,
                "nof_antennas_dl": 4,
                "nof_antennas_ul": 1,
                "prach_config_index": 159,
                "max_puschs_per_slot": test_configuration.max_puschs_per_slot,
                "max_pdschs_per_slot": test_configuration.max_pdschs_per_slot,
                "enable_qos_viavi": test_configuration.enable_qos_viavi,
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
            warning_as_errors=test_configuration.warning_as_errors,
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
            logging.info("Folder with Viavi report: %s", report_folder)
            logging.info("Downloading Viavi report")
            viavi.download_directory(report_folder, Path(test_log_folder).joinpath("viavi"))
            check_metrics_criteria(test_configuration, gnb, viavi, metrics_summary, fail_if_kos)
        except HTTPError:
            logging.error("Viavi Reports could not be downloaded")


def check_metrics_criteria(
    test_configuration: _ViaviConfiguration,
    gnb: GNBStub,
    viavi: Viavi,
    metrics_summary: Optional[MetricsSummary],
    fail_if_kos: bool,
):
    """
    Check pass/fail criteria
    """

    is_ok = True

    # Check metrics
    gnb_metrics: GnbMetrics = get_metrics(gnb)

    is_ok &= check_and_print_criteria(
        "DL bitrate", gnb_metrics.dl_brate_agregate, test_configuration.expected_dl_bitrate, operator.gt
    )
    is_ok &= check_and_print_criteria(
        "UL bitrate", gnb_metrics.ul_brate_agregate, test_configuration.expected_ul_bitrate, operator.gt
    )
    is_ok &= (
        check_and_print_criteria("Number of KOs and/or retrxs", gnb_metrics.nof_kos_aggregate, 0, operator.eq)
        or not fail_if_kos
    )

    # Save metrics
    if metrics_summary is not None:
        metrics_summary.write_metric("dl_bitrate", gnb_metrics.dl_brate_agregate)
        metrics_summary.write_metric("ul_bitrate", gnb_metrics.ul_brate_agregate)
        metrics_summary.write_metric("kos", gnb_metrics.nof_kos_aggregate)

    # Check procedure table
    viavi_failure_manager = viavi.get_test_failures()
    viavi_failure_manager.print_failures(_OMIT_VIAVI_FAILURE_LIST)
    is_ok &= viavi_failure_manager.get_number_of_failures(_OMIT_VIAVI_FAILURE_LIST) > 0

    if not is_ok:
        pytest.fail("Test didn't pass all the criteria")


def check_and_print_criteria(
    name: str, current: float, expected: float, operator_method: Callable[[float, float], bool]
) -> bool:
    """
    Check and print criteria
    """
    is_ok = operator_method(current, expected)
    (logging.info if is_ok else logging.error)(f"{name} expected: {expected}, actual: {current}")
    return is_ok


def get_viavi_configuration(test_name: str, warning_as_errors: bool) -> _ViaviConfiguration:
    """
    Get Viavi configuration
    """
    if test_name == _TestName.UE1_STATIC_DL_UL_UDP.value:
        return _ViaviConfiguration(
            max_pdschs_per_slot=8,
            max_puschs_per_slot=8,
            enable_qos_viavi=False,
            warning_as_errors=warning_as_errors,
            expected_dl_bitrate=80e6,
            expected_ul_bitrate=80e6,
        )
    if test_name == _TestName.UE32_STATIC_DL_UL_UDP.value:
        return _ViaviConfiguration(
            max_pdschs_per_slot=1,
            max_puschs_per_slot=1,
            enable_qos_viavi=False,
            warning_as_errors=warning_as_errors,
            expected_dl_bitrate=80e6,
            expected_ul_bitrate=80e6,
        )
    raise ValueError(f"Test name {test_name} not supported")
