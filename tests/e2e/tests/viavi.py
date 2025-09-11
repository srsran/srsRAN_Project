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
Launch tests in Viavi
"""
import logging
import operator
from dataclasses import dataclass, field
from pathlib import Path, PureWindowsPath
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
from retina.viavi.client import CampaignStatusEnum, Viavi, ViaviKPIs
from rich.console import Console
from rich.table import Table

from .steps.configuration import configure_metric_server_for_gnb
from .steps.kpis import get_kpis, KPIs
from .steps.stub import _stop_stub, GNB_STARTUP_TIMEOUT, handle_start_error, stop

_OMIT_VIAVI_FAILURE_LIST = ["authentication"]
_FLAKY_ERROR_LIST = [
    "Timeout reached while reserving and/or waiting for pods to be ready",
    "Error creating the pod",
    "Viavi API call timed out",
]
_GNB_STOP_TIMEOUT = 15  # When timeout reached, retina gets GDB backtrace and sends sigkill. 0 means no timeout


# pylint: disable=too-many-instance-attributes
@dataclass
class _ViaviConfiguration:
    """
    Viavi configuration
    """

    id: str = ""
    campaign_filename: str = ""
    test_name: str = ""
    test_timeout: int = 0
    gnb_extra_commands: str = ""
    retina_params: dict = field(default_factory=dict)
    # test/fail criteria
    expected_ul_bitrate: float = 0
    expected_dl_bitrate: float = 0
    expected_nof_kos: float = 0  # Infinity value is represented in a float
    expected_max_late_harqs: int = 0
    warning_as_errors: bool = True
    warning_allowlist: List[str] = field(default_factory=list)


# pylint: disable=too-many-instance-attributes
@dataclass
class _ViaviResult:
    """
    Viavi result
    """

    criteria_name: str
    current: float
    expected_operator: str
    expected: float
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
                id=test_declaration["id"],
                campaign_filename=test_declaration["campaign_filename"],
                test_name=test_declaration["test_name"],
                test_timeout=test_declaration["test_timeout"],
                gnb_extra_commands=_convert_extra_config_into_command(test_declaration["gnb_extra_config"]),
                retina_params=test_declaration.get("retina_params", {}),
                expected_dl_bitrate=test_declaration["expected_dl_bitrate"],
                expected_ul_bitrate=test_declaration["expected_ul_bitrate"],
                expected_max_late_harqs=test_declaration["expected_max_late_harqs"],
                expected_nof_kos=test_declaration["expected_nof_kos"],
                warning_as_errors=test_declaration["warning_as_errors"],
                warning_allowlist=test_declaration.get("warning_allowlist", []),
            )
        )
    return test_declaration_list


def _convert_extra_config_into_command(extra_config: dict) -> str:
    """
    Convert extra config into command
    """
    cmd_args = ""
    for key, value in sorted(extra_config.items(), key=lambda item: isinstance(item[1], dict)):
        if isinstance(value, dict):
            cmd_args += f"{key} " + _convert_extra_config_into_command(value)
        elif value is None:
            cmd_args += f"{key} "
        else:
            cmd_args += f"--{key}={value} "
    return cmd_args


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


@pytest.fixture
def viavi_manual_gnb_arguments(request):
    """
    Extra GNB arguments
    """
    return request.config.getoption("viavi_manual_gnb_arguments")


@mark.viavi_manual
@mark.flaky(
    reruns=2,
    only_rerun=_FLAKY_ERROR_LIST,
)
# pylint: disable=too-many-arguments,too-many-positional-arguments, too-many-locals
def test_viavi_manual(
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
    viavi_manual_campaign_filename: str,  # pylint: disable=redefined-outer-name
    viavi_manual_test_name: str,  # pylint: disable=redefined-outer-name
    viavi_manual_test_timeout: int,  # pylint: disable=redefined-outer-name
    viavi_manual_gnb_arguments: str,  # pylint: disable=redefined-outer-name
    # Test extra params
    always_download_artifacts: bool = True,
    gnb_startup_timeout: int = GNB_STARTUP_TIMEOUT,
    gnb_stop_timeout: int = _GNB_STOP_TIMEOUT,
    log_search: bool = True,
):
    """
    Runs a test using Viavi
    """
    test_declaration = get_viavi_configuration_from_testname(
        campaign_filename=viavi_manual_campaign_filename,
        test_name=viavi_manual_test_name,
        timeout=viavi_manual_test_timeout,
        gnb_arguments=viavi_manual_gnb_arguments,
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
        metrics_server=metrics_server,
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
# pylint: disable=too-many-arguments,too-many-positional-arguments, too-many-locals
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
    gnb_stop_timeout: int = _GNB_STOP_TIMEOUT,
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
# pylint: disable=too-many-arguments,too-many-positional-arguments, too-many-locals
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
    gnb_stop_timeout: int = _GNB_STOP_TIMEOUT,
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


# pylint: disable=too-many-arguments,too-many-positional-arguments, too-many-locals
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
    gnb_stop_timeout: int = _GNB_STOP_TIMEOUT,
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
                "inactivity_timer": 7200,
                "tac": 7,
                "pci": 1,
                "prach_config_index": 159,
                "nof_antennas_dl": 4,
                "nof_antennas_ul": 1,
                "rlc_metrics": True,
                "enable_high_latency_diagnostics": True,
                "warning_extra_regex": (
                    (r"(?!.*" + r")(?!.*".join(test_declaration.warning_allowlist) + r")")
                    if test_declaration.warning_allowlist
                    else ""
                ),
                **test_declaration.retina_params,
            },
        },
    }
    if metrics_server is not None:
        configure_metric_server_for_gnb(retina_manager=retina_manager, metrics_server=metrics_server)

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
        logging.info("GNB extra commands: %s", test_declaration.gnb_extra_commands)
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
        "Starting Campaign %s%s",
        test_declaration.campaign_filename,
        (f" - Test {test_declaration.test_name}" if test_declaration.test_name is not None else ""),
    )
    campaign_name = viavi.schedule_campaign(test_declaration.campaign_filename, test_declaration.test_name)

    # Start campaign
    viavi.run_campaign(campaign_name)
    logging.info("Campaign started")

    # Wait until end
    try:
        info = viavi.wait_until_running_campaign_teardown(test_declaration.test_timeout)
        try:
            # Final stop
            logging.info("Stopping GNB")
            stop(
                ue_array=(),
                gnb=gnb,
                fivegc=None,
                retina_data=retina_data,
                gnb_stop_timeout=gnb_stop_timeout,
                log_search=log_search,
                warning_as_errors=test_declaration.warning_as_errors,
                fail_if_kos=False,
            )
        finally:
            info = viavi.wait_until_running_campaign_finishes(test_declaration.test_timeout)
            if info.status is not CampaignStatusEnum.PASS:
                pytest.fail(f"Viavi Test Failed: {info.message}")

    # This except and the finally should be inside the request, but the campaign_name makes it complicated
    except (TimeoutError, KeyboardInterrupt):
        logging.info("Stopping test due to timeout")
        viavi.stop_running_campaign()
        logging.warning("Viavi Test did not end in the expected timeout")

    finally:
        try:
            logging.info("Generating Viavi report")
            report_folder = viavi.generate_report(campaign_name)
            logging.info("Folder with Viavi report: %s", report_folder)
            logging.info("Downloading Viavi report")
            viavi.download_directory(report_folder, Path(test_log_folder).joinpath("viavi"))
            _, gnb_error_count = _stop_stub(
                stub=gnb,
                name="GNB",
                retina_data=retina_data,
                timeout=gnb_stop_timeout,
                log_search=log_search,
                warning_as_errors=test_declaration.warning_as_errors,
            )
            check_metrics_criteria(
                test_configuration=test_declaration,
                gnb=gnb,
                viavi=viavi,
                metrics_summary=metrics_summary,
                capsys=capsys,
                gnb_error_count=gnb_error_count,
                warning_as_errors=test_declaration.warning_as_errors,
            )
        except HTTPError:
            logging.error("Viavi Reports could not be downloaded")


################################################################################
# Helper functions
################################################################################
def check_metrics_criteria(
    *,  # This enforces keyword-only arguments
    test_configuration: _ViaviConfiguration,
    gnb: GNBStub,
    viavi: Viavi,
    metrics_summary: Optional[MetricsSummary],
    capsys: pytest.CaptureFixture[str],
    gnb_error_count: int,
    warning_as_errors: bool,
):
    """
    Check pass/fail criteria
    """

    # Check metrics
    viavi_kpis: ViaviKPIs = viavi.get_test_kpis()
    viavi_kpis.print_procedure_failures(_OMIT_VIAVI_FAILURE_LIST)
    kpis: KPIs = get_kpis(du_or_gnb_array=[gnb], viavi_kpis=viavi_kpis, metrics_summary=metrics_summary)

    criteria_result = [
        _create_viavi_result(
            criteria_name="DL bitrate",
            current=kpis.dl_brate_aggregate,
            operator_method=operator.gt,
            expected=test_configuration.expected_dl_bitrate,
        ),
        _create_viavi_result(
            criteria_name="UL bitrate",
            current=kpis.ul_brate_aggregate,
            operator_method=operator.gt,
            expected=test_configuration.expected_ul_bitrate,
        ),
        _create_viavi_result(
            criteria_name="DL KOs (gnb)",
            current=kpis.nof_ko_dl,
            operator_method=operator.le,
            expected=test_configuration.expected_nof_kos,
        ),
        _create_viavi_result(
            criteria_name="DL KOs (viavi)",
            current=viavi_kpis.dl_data.num_tbs_errors if viavi_kpis.dl_data.num_tbs_errors is not None else 0,
            operator_method=operator.le,
            expected=test_configuration.expected_nof_kos,
        ),
        _create_viavi_result(
            criteria_name="UL KOs (gnb)",
            current=kpis.nof_ko_ul,
            operator_method=operator.le,
            expected=test_configuration.expected_nof_kos,
        ),
        _create_viavi_result(
            criteria_name="UL KOs (viavi)",
            current=viavi_kpis.ul_data.num_tbs_nack if viavi_kpis.ul_data.num_tbs_nack is not None else 0,
            operator_method=operator.le,
            expected=test_configuration.expected_nof_kos,
        ),
        _create_viavi_result(
            criteria_name="Late DL HARQs (gnb)",
            current=kpis.max_late_dl_harqs,
            operator_method=operator.le,
            expected=test_configuration.expected_max_late_harqs,
        ),
        _create_viavi_result(
            criteria_name="Late UL HARQs (gnb)",
            current=kpis.max_late_ul_harqs,
            operator_method=operator.le,
            expected=test_configuration.expected_max_late_harqs,
        ),
        _create_viavi_result(
            criteria_name="Error Indications",
            current=kpis.nof_error_indications,
            operator_method=operator.eq,
            expected=0,
        ),
        _create_viavi_result(
            criteria_name="Errors" + (" & warnings" if warning_as_errors else ""),
            current=gnb_error_count,
            operator_method=operator.eq,
            expected=0,
        ),
        _create_viavi_result(
            criteria_name="Viavi Warnings",
            current=len(viavi_kpis.warning_array),
            operator_method=operator.lt,
            expected=float("inf"),
        ),
        _create_viavi_result(
            criteria_name="Procedure table",
            current=viavi_kpis.get_number_of_procedure_failures(_OMIT_VIAVI_FAILURE_LIST),
            operator_method=operator.eq,
            expected=0,
        ),
    ]

    create_table(criteria_result, capsys)
    criteria_errors_str = []
    for criteria in criteria_result:
        if not criteria.is_ok:
            criteria_errors_str.append(criteria.criteria_name)
    if criteria_errors_str:
        pytest.fail("Test didn't pass the following criteria: " + ", ".join(criteria_errors_str))


def _create_viavi_result(
    *,  # This enforces keyword-only arguments
    criteria_name: str,
    current: float,
    operator_method: Callable,
    expected: float,
) -> _ViaviResult:

    is_ok = operator_method(current, expected)

    return _ViaviResult(
        criteria_name=criteria_name,
        current=current,
        expected_operator={
            operator.lt: "<",
            operator.le: "<=",
            operator.eq: "==",
            operator.ne: "!=",
            operator.gt: ">",
            operator.ge: ">=",
        }.get(operator_method, "?"),
        expected=expected,
        is_ok=is_ok,
    )


def create_table(results: List[_ViaviResult], capsys):
    """
    Create a table with the results
    """
    table = Table(title="Viavi Results")

    table.add_column("Criteria Name", justify="left", style="cyan", no_wrap=True)
    table.add_column("Result", justify="right", style="magenta")
    table.add_column("Expected", justify="right", style="magenta")
    table.add_column("Pass", justify="center", style="magenta")

    for result in results:
        row_style = "green" if result.is_ok else "red"
        table.add_row(
            result.criteria_name,
            f"{get_str_number_criteria(result.current)}",
            f"{result.expected_operator} {get_str_number_criteria(result.expected)}",
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
    *,  # This enforces keyword-only arguments
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
    if number_criteria == float("inf"):
        return "∞"
    if number_criteria == float("-inf"):
        return "-∞"
    if number_criteria >= 1_000_000_000:
        return f"{number_criteria / 1_000_000_000:.1f}G"
    if number_criteria >= 1_000_000:
        return f"{number_criteria / 1_000_000:.1f}M"
    if number_criteria >= 1_000:
        return f"{number_criteria / 1_000:.1f}K"
    return str(number_criteria)


def get_viavi_configuration_from_testname(
    *, campaign_filename: str, test_name: str, timeout: int, gnb_arguments=""  # The "*" enforces keyword-only arguments
) -> _ViaviConfiguration:
    """
    Get Viavi configuration from dict
    """
    config = load_yaml_config("test_declaration.yml")

    # Try to find the test in the test_declaration by campaing filename and ID!
    for test_config in config:
        if (
            PureWindowsPath(test_config.campaign_filename) == PureWindowsPath(campaign_filename)
            and test_config.id == test_name
        ):
            test_declaration = test_config
            break
    else:
        logging.warning(
            "Test: %s - %s is not in test_declaration. No extra pass/fail criteria will be applied.",
            campaign_filename,
            test_name,
        )
        test_declaration = _ViaviConfiguration(campaign_filename=campaign_filename, test_name=test_name)

    # Override the timeout and extra gnb arguments
    test_declaration.test_timeout = timeout
    if gnb_arguments:
        test_declaration.gnb_extra_commands = gnb_arguments

    return test_declaration
