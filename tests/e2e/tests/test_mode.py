#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Validate Test Mode
"""

import logging
import tempfile
from pathlib import Path
from time import sleep
from typing import Iterable

import pytest
from google.protobuf.empty_pb2 import Empty
from pytest import mark, param
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.utils import configure_artifacts
from retina.protocol.base_pb2 import FiveGCDefinition, GNBDefinition, Metrics, PLMN, StartInfo, UEDefinition
from retina.protocol.fivegc_pb2 import FiveGCStartInfo
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2 import GNBStartInfo
from retina.protocol.gnb_pb2_grpc import GNBStub

from .steps.iperf_helpers import get_max_theoretical_bitrate, QAMTable
from .steps.stub import FIVEGC_STARTUP_TIMEOUT, GNB_STARTUP_TIMEOUT, handle_start_error, stop

_POD_ERROR = "Error creating the pod"


@mark.parametrize(
    "extra_config, nof_ant",
    (
        param("test_mode test_ue --ri 1", 1, id="Test UE 1x1 Rank 1"),
        param("test_mode test_ue --ri 1", 2, id="Test UE 2x2 Rank 1"),
        param("test_mode test_ue --ri 2", 2, id="Test UE 2x2 Rank 2"),
        param("test_mode test_ue --ri 1", 4, id="Test UE 4x4 Rank 1"),
        param("test_mode test_ue --ri 2", 4, id="Test UE 4x4 Rank 2"),
        param("test_mode test_ue --ri 3", 4, id="Test UE 4x4 Rank 3"),
        param("test_mode test_ue --ri 4", 4, id="Test UE 4x4 Rank 4"),
    ),
)
@mark.test_mode
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_ue(
    # Retina
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    # Clients
    fivegc: FiveGCStub,
    gnb: GNBStub,
    # Test
    extra_config: str,
    nof_ant: int,
    duration: int = 5 * 60,
    # Test extra params
    always_download_artifacts: bool = True,
    gnb_startup_timeout: int = GNB_STARTUP_TIMEOUT,
    fivegc_startup_timeout: int = FIVEGC_STARTUP_TIMEOUT,
    gnb_stop_timeout: int = 0,
    log_search: bool = True,
    warning_as_errors: bool = True,
    fail_if_kos: bool = True,
):  # pylint: disable=too-many-locals
    """
    Run gnb in test mode.
    """

    # Configuration
    with tempfile.NamedTemporaryFile(mode="w+") as tmp_file:
        tmp_file.write(" ")  # Make it not empty to overwrite default one
        tmp_file.flush()

        retina_data.test_config = {
            "gnb": {
                "parameters": {
                    "gnb_id": 1,
                    "log_level": "warning",
                    "pcap": False,
                    "nof_antennas_dl": nof_ant,
                    "nof_antennas_ul": nof_ant,
                },
                "templates": {
                    "cu": str(Path(__file__).joinpath("../test_mode/config_ue.yml").resolve()),
                    "du": tmp_file.name,
                },
            },
        }
        retina_manager.parse_configuration(retina_data.test_config)
        retina_manager.push_all_config()

    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=always_download_artifacts,
    )

    # 5GC and GNB Start
    with handle_start_error(name=f"5GC [{id(fivegc)}]"):
        fivegc_def: FiveGCDefinition = fivegc.GetDefinition(Empty())
        fivegc.Start(
            FiveGCStartInfo(
                plmn=PLMN(mcc="001", mnc="01"),
                start_info=StartInfo(timeout=fivegc_startup_timeout),
            )
        )

    with handle_start_error(name=f"GNB [{id(gnb)}]"):
        gnb_def: GNBDefinition = gnb.GetDefinition(Empty())
        gnb.Start(
            GNBStartInfo(
                plmn=PLMN(mcc="001", mnc="01"),
                ue_definition=UEDefinition(zmq_ip=gnb_def.zmq_ip, zmq_port_array=gnb_def.zmq_port_array),
                fivegc_definition=fivegc_def,
                start_info=StartInfo(
                    timeout=gnb_startup_timeout,
                    post_commands=(
                        "",
                        extra_config,
                    ),
                ),
            )
        )

    logging.info("Running Test Mode for %s seconds", duration)
    sleep(duration)

    # Stop
    stop(
        tuple(),
        gnb,
        fivegc,
        retina_data,
        gnb_stop_timeout=gnb_stop_timeout,
        log_search=log_search,
        warning_as_errors=warning_as_errors,
        fail_if_kos=fail_if_kos,
    )


@mark.test_mode_acc100
@mark.flaky(
    reruns=2,
    only_rerun=[_POD_ERROR],
)
def test_ru_acc100(
    # Retina
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    # Clients
    gnb: GNBStub,
):
    """
    Run gnb in test mode ru dummy.
    """
    _test_ru(
        retina_manager,
        retina_data,
        gnb,
        ru_config="config_ru_acc100.yml",
        min_dl_bitrate=1e6,
        warning_allowlist=(
            "Resource grid with identifier",
            "Could not enqueue PDCCH",
            "received late DL request from slot",
        ),
    )


@mark.test_mode
@mark.flaky(
    reruns=2,
    only_rerun=[_POD_ERROR],
)
def test_ru(
    # Retina
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    # Clients
    gnb: GNBStub,
):
    """
    Run gnb in test mode ru dummy.
    """
    _test_ru(retina_manager, retina_data, gnb, ru_config="config_ru.yml")


@mark.test_mode
@mark.flaky(
    reruns=2,
    only_rerun=[_POD_ERROR],
)
def test_ru_16cell_50ue(
    # Retina
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    # Clients
    gnb: GNBStub,
):
    """
    Run gnb in test mode ru dummy.
    """
    _test_ru(retina_manager, retina_data, gnb, ru_config="config_ru_16cell_50ue.yml", warning_as_errors=False)


@mark.test_mode_not_crash
@mark.flaky(
    reruns=2,
    only_rerun=[_POD_ERROR],
)
def test_ru_not_crash(
    # Retina
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    # Clients
    gnb: GNBStub,
):
    """
    Run gnb with sanitizers in test mode ru dummy.
    It ignores warnings and KOs, so it will fail if the gnb+sanitizer fails
    """
    _test_ru(
        retina_manager,
        retina_data,
        gnb,
        ru_config="config_ru.yml",
        gnb_stop_timeout=150,
        warning_as_errors=False,
        fail_if_kos=False,
    )


@mark.test_mode_not_crash
@mark.flaky(
    reruns=2,
    only_rerun=[_POD_ERROR],
)
def test_ru_16cell_50ue_not_crash(
    # Retina
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    # Clients
    gnb: GNBStub,
):
    """
    Run gnb with sanitizers in test mode ru dummy.
    It ignores warnings and KOs, so it will fail if the gnb+sanitizer fails
    """
    _test_ru(
        retina_manager,
        retina_data,
        gnb,
        ru_config="config_ru_16cell_50ue.yml",
        duration=15 * 60,
        gnb_stop_timeout=150,
        warning_as_errors=False,
        fail_if_kos=False,
    )


# pylint: disable=too-many-arguments,too-many-positional-arguments
def _test_ru(
    # Retina
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    # Clients
    gnb: GNBStub,
    # Test
    ru_config,
    nof_ant: int = 4,
    duration: int = 5 * 60,
    # Test extra params
    always_download_artifacts: bool = True,
    gnb_startup_timeout: int = GNB_STARTUP_TIMEOUT,
    gnb_stop_timeout: int = 0,
    # Criteria
    log_search: bool = True,
    warning_as_errors: bool = True,
    warning_allowlist: Iterable[str] = tuple(),
    fail_if_kos: bool = True,
    min_dl_bitrate: float = 1,
    min_ul_bitrate: float = 1,
):  # pylint: disable=too-many-locals
    # Configuration
    with tempfile.NamedTemporaryFile(mode="w+") as tmp_file:
        tmp_file.write(" ")  # Make it not empty to overwrite default one
        tmp_file.flush()
        retina_data.test_config = {
            "gnb": {
                "parameters": {
                    "gnb_id": 1,
                    "log_level": "warning",
                    "pcap": False,
                    "nof_antennas_dl": nof_ant,
                    "nof_antennas_ul": nof_ant,
                    "warning_extra_regex": (
                        (r"(?!.*" + r")(?!.*".join(warning_allowlist) + r")") if warning_allowlist else ""
                    ),
                },
                "templates": {
                    "cu": str(Path(__file__).joinpath(f"../test_mode/{ru_config}").resolve()),
                    "du": tmp_file.name,
                    "ru": tmp_file.name,
                },
            },
        }
        retina_manager.parse_configuration(retina_data.test_config)
        retina_manager.push_all_config()

    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=always_download_artifacts,
    )

    # GNB Start
    with handle_start_error(name=f"GNB [{id(gnb)}]"):
        gnb.Start(
            GNBStartInfo(
                plmn=PLMN(mcc="001", mnc="01"),
                ue_definition=UEDefinition(zmq_port_array=tuple(range(nof_ant))),
                fivegc_definition=FiveGCDefinition(amf_ip="127.0.0.1", amf_port=38412),
                start_info=StartInfo(
                    timeout=gnb_startup_timeout,
                    post_commands=("cu_cp amf --no_core 1",),
                ),
            )
        )

    logging.info("Running Test Mode for %s seconds", duration)
    sleep(duration)

    # Stop
    stop(
        tuple(),
        gnb,
        None,
        retina_data,
        gnb_stop_timeout=gnb_stop_timeout,
        log_search=log_search,
        warning_as_errors=warning_as_errors,
        fail_if_kos=fail_if_kos,
    )

    metrics: Metrics = gnb.GetMetrics(Empty())
    if metrics.total.dl_bitrate < min_dl_bitrate:
        pytest.fail(f"Low DL Bitrate: {metrics.total.dl_bitrate} [< {min_dl_bitrate}]")
    if metrics.total.ul_bitrate < min_ul_bitrate:
        pytest.fail(f"Low UL Bitrate: {metrics.total.ul_bitrate} [< {min_ul_bitrate}]")


@mark.test_mode_many_ues
@mark.flaky(
    reruns=2,
    only_rerun=[_POD_ERROR],
)
def test_mode_many_ues(
    # Retina
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    # Clients
    gnb: GNBStub,
    # Test
    ru_config="config_ru_800_ues.yml",
    nof_ant: int = 4,
    duration: int = 5 * 60,
    # Test extra params
    always_download_artifacts: bool = True,
    gnb_startup_timeout: int = GNB_STARTUP_TIMEOUT,
    gnb_stop_timeout: int = 0,
    log_search: bool = True,
    warning_as_errors: bool = True,
    fail_if_kos: bool = True,
    extra_cli_config: str = "",
):
    """
    Run gnb in test mode ru dummy and 800 UEs.
    It fails if less than 800 UEs are connected or the total bitrate is below the expected threshold
    """
    with tempfile.NamedTemporaryFile(mode="w+") as tmp_file:
        tmp_file.write(" ")  # Make it not empty to overwrite default one
        tmp_file.flush()
        retina_data.test_config = {
            "gnb": {
                "parameters": {
                    "gnb_id": 1,
                    "log_level": "warning",
                    "pcap": False,
                    "nof_antennas_dl": nof_ant,
                    "nof_antennas_ul": nof_ant,
                },
                "templates": {
                    "cu": str(Path(__file__).joinpath(f"../test_mode/{ru_config}").resolve()),
                    "du": tmp_file.name,
                    "ru": tmp_file.name,
                },
            },
        }
        retina_manager.parse_configuration(retina_data.test_config)
        retina_manager.push_all_config()

    configure_artifacts(
        retina_data=retina_data,
        always_download_artifacts=always_download_artifacts,
    )

    # GNB Start
    with handle_start_error(name=f"GNB [{id(gnb)}]"):
        gnb.Start(
            GNBStartInfo(
                plmn=PLMN(mcc="001", mnc="01"),
                ue_definition=UEDefinition(zmq_port_array=tuple(range(nof_ant))),
                fivegc_definition=FiveGCDefinition(amf_ip="127.0.0.1", amf_port=38412),
                start_info=StartInfo(
                    timeout=gnb_startup_timeout,
                    post_commands=(f"cu_cp amf --no_core 1 {extra_cli_config}",),
                ),
            )
        )

    logging.info("Running Test Mode for %s seconds", duration)
    sleep(duration)

    # Stop
    stop(
        tuple(),
        gnb,
        None,
        retina_data,
        gnb_stop_timeout=gnb_stop_timeout,
        log_search=log_search,
        warning_as_errors=warning_as_errors,
        fail_if_kos=fail_if_kos,
    )

    # These value must be consistent with the configuration in config_ru_800_ues.yml
    min_dl_bitrate = get_max_theoretical_bitrate(
        100,
        78,
        4,
        True,
        QAMTable.QAM256,
    )
    min_ul_bitrate = get_max_theoretical_bitrate(
        100,
        78,
        1,
        False,
        QAMTable.QAM256,
    )

    effective_rate_coeff_dl = 0.65  # empirical value
    effective_rate_coeff_ul = 0.55  # empirical value

    dl_brate_threshold = min_dl_bitrate * effective_rate_coeff_dl
    ul_brate_threshold = min_ul_bitrate * effective_rate_coeff_ul

    metrics: Metrics = gnb.GetMetrics(Empty())
    logging.info(
        "DL tot brate %.3f Mbps (expected > %.3f Mbps)",
        metrics.total.dl_bitrate * 1e-6,
        dl_brate_threshold * 1e-6,
    )
    logging.info(
        "UL tot brate %.3f Mbps (expected > %.3f Mbps)",
        metrics.total.ul_bitrate * 1e-6,
        ul_brate_threshold * 1e-6,
    )
    # Assess the expected bitrate
    if metrics.total.dl_bitrate < dl_brate_threshold:
        pytest.fail(f"Low DL Bitrate: {metrics.total.dl_bitrate*1e-6}Mbps [< {dl_brate_threshold*1e-6}]Mbps")
    if metrics.total.ul_bitrate < ul_brate_threshold:
        pytest.fail(f"Low UL Bitrate: {metrics.total.ul_bitrate*1e-6}Mbps [< {ul_brate_threshold*1e-6}]Mbps")

    # Assess number of UEs
    nof_ues = 32
    if len(metrics.ue_array) != nof_ues:
        pytest.fail(f"Expected {nof_ues} UEs, found {len(metrics.ue_array)} UEs in the metrics")

    # UE bitrate should be at least 70% of the average UE bitrate divided, as the Round Robin
    # scheduler cannot achive complete fairness
    min_expected_ue_dl_brate = metrics.total.dl_bitrate * 0.7 / nof_ues
    min_expected_ue_ul_brate = metrics.total.ul_bitrate * 0.7 / nof_ues
    ue_below_threshold = []
    for ue_m in metrics.ue_array:
        if (
            ue_m.dl_bitrate_peak_av.av_30_samples < min_expected_ue_dl_brate
            or ue_m.ul_bitrate_peak_av.av_30_samples < min_expected_ue_ul_brate
        ):
            ue_below_threshold.append(ue_m.rnti)
    if len(ue_below_threshold) > 0:
        pytest.fail(f"UEs with RNTI:{ue_below_threshold} have bitrate below the expected threshold")
