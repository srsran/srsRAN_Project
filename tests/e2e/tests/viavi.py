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
import operator
from dataclasses import dataclass
from pathlib import Path
from typing import Callable, List, Optional

import pytest
import yaml
from pytest import mark
from requests import HTTPError
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.public import MetricServerInfo, MetricsSummary
from retina.launcher.utils import configure_artifacts
from retina.protocol.base_pb2 import FiveGCDefinition, PLMN, StartInfo
from retina.protocol.gnb_pb2 import GNBStartInfo
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.viavi.client import CampaignStatusEnum, Viavi
from rich.console import Console
from rich.table import Table

from .steps.configuration import configure_metric_server_for_gnb
from .steps.kpis import get_kpis, KPIs
from .steps.stub import GNB_STARTUP_TIMEOUT, handle_start_error, stop

_OMIT_VIAVI_FAILURE_LIST = ["authentication"]
_FLAKY_ERROR_LIST = ["Error creating the pod", "Viavi API call timed out"]


# pylint: disable=too-many-instance-attributes
@dataclass
class _ViaviConfiguration:
    """
    Viavi configuration
    """

    campaign_filename: str = ""
    test_name: str = ""
    test_timeout: int = 0
    gnb_extra_commands: str = ""
    id: str = ""
    max_pdschs_per_slot: int = 1
    max_puschs_per_slot: int = 1
    enable_qos_viavi: bool = False
    # test/fail criteria
    expected_ul_bitrate: float = 0
    expected_dl_bitrate: float = 0
    fail_if_kos: bool = True
    warning_as_errors: bool = True


# pylint: disable=too-many-instance-attributes
@dataclass
class _ViaviResult:
    """
    Viavi result
    """

    criteria_name: str
    expected: float
    current: float
    is_ok: bool


def load_yaml_config(config_filename: str) -> List[_ViaviConfiguration]:
    """
    Load yaml config
    """
    test_declaration_list: List[_ViaviConfiguration] = []
    config_filepath = Path(__file__).parent.joinpath("viavi", config_filename).absolute()
    with open(config_filepath, "r", encoding="UTF-8") as file:
        test_declaration_list_raw = yaml.safe_load(file)["tests"]

    for test_declaration in test_declaration_list_raw:
        test_declaration_list.append(
            _ViaviConfiguration(
                campaign_filename=test_declaration["campaign_filename"],
                test_name=test_declaration["test_name"],
                test_timeout=test_declaration["test_timeout"],
                gnb_extra_commands=test_declaration["gnb_extra_commands"],
                id=test_declaration["id"],
                max_pdschs_per_slot=test_declaration["max_pdschs_per_slot"],
                max_puschs_per_slot=test_declaration["max_pdschs_per_slot"],
                enable_qos_viavi=test_declaration["enable_qos_viavi"],
                expected_dl_bitrate=test_declaration["expected_dl_bitrate"],
                expected_ul_bitrate=test_declaration["expected_ul_bitrate"],
                fail_if_kos=test_declaration["fail_if_kos"],
                warning_as_errors=test_declaration["warning_as_errors"],
            )
        )
    return test_declaration_list


################################################################################
# Manual tests
################################################################################
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
    capsys: pytest.CaptureFixture[str],
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
    test_declaration = get_viavi_configuration_from_testname(
        viavi_manual_campaign_filename, viavi_manual_test_name, viavi_manual_test_timeout
    )

    _test_viavi(
        capsys=capsys,
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
        test_declaration=test_declaration,
        # Test extra params
        always_download_artifacts=always_download_artifacts,
        gnb_startup_timeout=gnb_startup_timeout,
        gnb_stop_timeout=gnb_stop_timeout,
        log_search=log_search,
    )


################################################################################
# CI tests
################################################################################
@pytest.mark.parametrize(
    "test_declaration",
    [
        pytest.param(test_declaration, id=test_declaration.id)
        for test_declaration in load_yaml_config("test_declaration.yml")
    ],
)
@mark.viavi
@mark.flaky(
    reruns=2,
    only_rerun=_FLAKY_ERROR_LIST,
)
# pylint: disable=too-many-arguments, too-many-locals
def test_viavi(
    capsys: pytest.CaptureFixture[str],
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
    test_declaration: _ViaviConfiguration,
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
        capsys=capsys,
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
        test_declaration=test_declaration,
        # Test extra params
        always_download_artifacts=always_download_artifacts,
        gnb_startup_timeout=gnb_startup_timeout,
        gnb_stop_timeout=gnb_stop_timeout,
        log_search=log_search,
    )


@pytest.mark.parametrize(
    "test_declaration",
    [
        pytest.param(test_declaration, id=test_declaration.id)
        for test_declaration in load_yaml_config("test_declaration_debug.yml")
    ],
)
@mark.viavi_debug
@mark.flaky(
    reruns=2,
    only_rerun=_FLAKY_ERROR_LIST,
)
# pylint: disable=too-many-arguments, too-many-locals
def test_viavi_debug(
    capsys: pytest.CaptureFixture[str],
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
    test_declaration: _ViaviConfiguration,
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
        capsys=capsys,
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
        test_declaration=test_declaration,
        # Test extra params
        always_download_artifacts=always_download_artifacts,
        gnb_startup_timeout=gnb_startup_timeout,
        gnb_stop_timeout=gnb_stop_timeout,
        log_search=log_search,
    )


# pylint: disable=too-many-arguments, too-many-locals
def _test_viavi(
    capsys: pytest.CaptureFixture[str],
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
    test_declaration: _ViaviConfiguration,
    # Test extra params
    always_download_artifacts: bool = True,
    gnb_startup_timeout: int = GNB_STARTUP_TIMEOUT,
    gnb_stop_timeout: int = 0,
    log_search: bool = True,
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
                "max_rb_size": -1,
                "dl_arfcn": 625000,
                "band": 78,
                "bandwidth": 100,
                "common_scs": 30,
                "tac": 7,
                "pci": 1,
                "prach_config_index": 159,
                "max_puschs_per_slot": test_declaration.max_puschs_per_slot,
                "max_pdschs_per_slot": test_declaration.max_pdschs_per_slot,
                "enable_qos_viavi": test_declaration.enable_qos_viavi,
            },
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
                    post_commands=(test_declaration.gnb_extra_commands,),
                ),
            )
        )

    # Create campaign
    logging.info(
        f"Starting Campaign {test_declaration.campaign_filename}"
        + (f" - Test {test_declaration.test_name}" if test_declaration.test_name is not None else "")
    )
    campaign_name = viavi.schedule_campaign(test_declaration.campaign_filename, test_declaration.test_name)

    # Start campaign
    viavi.run_campaign(campaign_name)
    logging.info("Campaign started")

    # Wait until end
    try:
        info = viavi.wait_until_running_campaign_finishes(test_declaration.test_timeout)
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
            warning_as_errors=test_declaration.warning_as_errors,
            fail_if_kos=test_declaration.fail_if_kos,
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
            check_metrics_criteria(test_declaration, gnb, viavi, metrics_summary, test_declaration.fail_if_kos, capsys)
        except HTTPError:
            logging.error("Viavi Reports could not be downloaded")


################################################################################
# Helper functions
################################################################################
def check_metrics_criteria(
    test_configuration: _ViaviConfiguration,
    gnb: GNBStub,
    viavi: Viavi,
    metrics_summary: Optional[MetricsSummary],
    fail_if_kos: bool,
    capsys: pytest.CaptureFixture[str],
):
    """
    Check pass/fail criteria
    """

    is_ok = True

    # Check metrics
    viavi_failure_manager = viavi.get_test_failures()
    kpis: KPIs = get_kpis(gnb, viavi_failure_manager=viavi_failure_manager, metrics_summary=metrics_summary)

    criteria_result: List[_ViaviResult] = []
    criteria_dl_brate_aggregate = check_criteria(
        kpis.dl_brate_aggregate, test_configuration.expected_dl_bitrate, operator.gt
    )
    criteria_result.append(
        _ViaviResult(
            "DL bitrate", test_configuration.expected_dl_bitrate, kpis.dl_brate_aggregate, criteria_dl_brate_aggregate
        )
    )

    criteria_ul_brate_aggregate = check_criteria(
        kpis.ul_brate_aggregate, test_configuration.expected_ul_bitrate, operator.gt
    )
    criteria_result.append(
        _ViaviResult(
            "UL bitrate", test_configuration.expected_ul_bitrate, kpis.ul_brate_aggregate, criteria_ul_brate_aggregate
        )
    )

    criteria_nof_ko_aggregate = check_criteria(kpis.nof_ko_aggregate, 0, operator.eq) or not fail_if_kos
    criteria_result.append(
        _ViaviResult("Number of KOs and/or retrxs", 0, kpis.nof_ko_aggregate, criteria_nof_ko_aggregate)
    )

    # Check procedure table
    viavi_failure_manager.print_failures(_OMIT_VIAVI_FAILURE_LIST)
    criteria_procedure_table = viavi_failure_manager.get_number_of_failures(_OMIT_VIAVI_FAILURE_LIST) == 0
    criteria_result.append(
        _ViaviResult(
            "Procedure table",
            0,
            viavi_failure_manager.get_number_of_failures(_OMIT_VIAVI_FAILURE_LIST),
            criteria_procedure_table,
        )
    )

    is_ok = (
        criteria_dl_brate_aggregate
        and criteria_ul_brate_aggregate
        and criteria_nof_ko_aggregate
        and criteria_procedure_table
    )

    create_table(criteria_result, capsys)
    if not is_ok:
        criteria_errors_str = []
        for criteria in criteria_result:
            if not criteria.is_ok:
                criteria_errors_str.append(criteria.criteria_name)
        pytest.fail("Test didn't pass the following criteria: " + ", ".join(criteria_errors_str))


def create_table(results: List[_ViaviResult], capsys):
    """
    Create a table with the results
    """
    table = Table(title="Viavi Results")

    table.add_column("Criteria Name", justify="left", style="cyan", no_wrap=True)
    table.add_column("Expected", justify="right", style="magenta")
    table.add_column("Result", justify="right", style="magenta")
    table.add_column("Pass", justify="center", style="magenta")

    for result in results:
        row_style = "green" if result.is_ok else "red"
        table.add_row(
            result.criteria_name,
            f"{get_str_number_criteria(result.expected)}",
            f"{get_str_number_criteria(result.current)}",
            "✅" if result.is_ok else "❌",
            style=row_style,
        )

    console = Console()
    # Capture the table to print it in the console
    with console.capture() as capture:
        console.print(table)
    output = "\n" + capture.get()

    # Disable temporarily the capsys to print the table
    with capsys.disabled():
        logging.info(output)


def check_criteria(
    current: float,
    expected: float,
    operator_method: Callable[[float, float], bool],
) -> bool:
    """
    Check criteria
    """
    is_ok = operator_method(current, expected)
    return is_ok


def get_str_number_criteria(number_criteria: float) -> str:
    """
    Get string number criteria
    """
    if number_criteria >= 1_000_000_000:
        return f"{number_criteria / 1_000_000_000:.1f}G"
    if number_criteria >= 1_000_000:
        return f"{number_criteria / 1_000_000:.1f}M"
    if number_criteria >= 1_000:
        return f"{number_criteria / 1_000:.1f}K"
    return str(number_criteria)


def get_viavi_configuration_from_testname(campaign_filename: str, test_name: str, timeout: int) -> _ViaviConfiguration:
    """
    Get Viavi configuration from dict
    """
    config = load_yaml_config("test_declaration.yml")
    for test_config in config:
        if test_config.test_name == test_name:
            test_declaration = test_config
            break

    if test_declaration is None:
        logging.warning("There is no config for the test: %s", test_name)
        test_declaration = _ViaviConfiguration()

    test_declaration.campaign_filename = campaign_filename
    test_declaration.test_timeout = timeout
    return test_declaration
