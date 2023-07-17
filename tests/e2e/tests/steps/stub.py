#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#
"""
Steps related with stubs / resources
"""

import logging
from contextlib import suppress
from typing import Dict, Sequence, Tuple

import grpc
import pytest
from retina.launcher.artifacts import RetinaTestData
from retina.protocol import RanStub
from retina.protocol.base_pb2 import (
    Empty,
    Metrics,
    PingRequest,
    PingResponse,
    StartInfo,
    StopResponse,
    String,
    UEDefinition,
    UInteger,
)
from retina.protocol.epc_pb2 import IPerfResponse
from retina.protocol.epc_pb2_grpc import EPCStub
from retina.protocol.gnb_pb2 import GNBStartInfo
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto, IPerfRequest, UEAttachedInfo, UEStartInfo
from retina.protocol.ue_pb2_grpc import UEStub

RF_MAX_TIMEOUT: int = 3 * 60  # Time enough in RF when loading a new image in the sdr
UE_STARTUP_TIMEOUT: int = RF_MAX_TIMEOUT
GNB_STARTUP_TIMEOUT: int = 5  # GNB delay (we wait x seconds and check it's still alive). UE later and has a big timeout
EPC_STARTUP_TIMEOUT: int = RF_MAX_TIMEOUT
ATTACH_TIMEOUT: int = 120


class StartFailure(Exception):
    """
    Some SUT failed to start
    """


# pylint: disable=too-many-arguments,too-many-locals
def start_and_attach(
    ue_array: Sequence[UEStub],
    gnb: GNBStub,
    epc: EPCStub,
    ue_startup_timeout: int = UE_STARTUP_TIMEOUT,
    gnb_startup_timeout: int = GNB_STARTUP_TIMEOUT,
    epc_startup_timeout: int = EPC_STARTUP_TIMEOUT,
    gnb_pre_cmd: str = "",
    gnb_post_cmd: str = "",
    attach_timeout: int = ATTACH_TIMEOUT,
) -> Dict[UEStub, UEAttachedInfo]:
    """
    Start stubs & wait until attach
    """
    start_network(
        ue_array,
        gnb,
        epc,
        gnb_startup_timeout,
        epc_startup_timeout,
        gnb_pre_cmd,
        gnb_post_cmd,
    )

    return ue_start_and_attach(
        ue_array,
        gnb,
        epc,
        ue_startup_timeout=ue_startup_timeout,
        attach_timeout=attach_timeout,
    )


# pylint: disable=too-many-arguments,too-many-locals
def start_network(
    ue_array: Sequence[UEStub],
    gnb: GNBStub,
    epc: EPCStub,
    gnb_startup_timeout: int = GNB_STARTUP_TIMEOUT,
    epc_startup_timeout: int = EPC_STARTUP_TIMEOUT,
    gnb_pre_cmd: str = "",
    gnb_post_cmd: str = "",
):
    """
    Start Network (EPC + gNB)
    """

    ue_def_for_gnb = UEDefinition()
    for ue_stub in ue_array:
        ue_def: UEDefinition = ue_stub.GetDefinition(Empty())
        epc.AddUESubscriber(ue_def.subscriber)
        if ue_def.zmq_ip is not None:
            ue_def_for_gnb = ue_def

    try:
        # EPC Start
        epc.Start(StartInfo(timeout=epc_startup_timeout))
        logging.info("EPC [%s] started", id(epc))

        # GNB Start
        gnb.Start(
            GNBStartInfo(
                ue_definition=ue_def_for_gnb,
                epc_definition=epc.GetDefinition(Empty()),
                start_info=StartInfo(
                    timeout=gnb_startup_timeout,
                    pre_commands=gnb_pre_cmd,
                    post_commands=gnb_post_cmd,
                ),
            )
        )
        logging.info("GNB [%s] started", id(gnb))

    except grpc.RpcError as err:
        # pylint: disable=protected-access
        if err._state.code is grpc.StatusCode.ABORTED:
            raise StartFailure from None
        raise err from None


def ue_start_and_attach(
    ue_array: Sequence[UEStub],
    gnb: GNBStub,
    epc: EPCStub,
    ue_startup_timeout: int = UE_STARTUP_TIMEOUT,
    attach_timeout: int = ATTACH_TIMEOUT,
) -> Dict[UEStub, UEAttachedInfo]:
    """
    Start an array of UEs and wait until attached to already running gnb and epc
    """

    try:
        for ue_stub in ue_array:
            ue_stub.Start(
                UEStartInfo(
                    gnb_definition=gnb.GetDefinition(Empty()),
                    epc_definition=epc.GetDefinition(Empty()),
                    start_info=StartInfo(timeout=ue_startup_timeout),
                )
            )
            logging.info("UE [%s] started", id(ue_stub))

    except grpc.RpcError as err:
        # pylint: disable=protected-access
        if err._state.code is grpc.StatusCode.ABORTED:
            raise StartFailure from None
        raise err from None

    # Attach in parallel
    ue_attach_task_dict: Dict[UEStub, grpc.Future] = {
        ue_stub: ue_stub.WaitUntilAttached.future(UInteger(value=attach_timeout)) for ue_stub in ue_array
    }
    for ue_stub, task in ue_attach_task_dict.items():
        task.add_done_callback(lambda _task, _ue_stub=ue_stub: _log_attached_ue(_task, _ue_stub))

    ue_attach_info_dict: Dict[UEStub, UEAttachedInfo] = {}
    with suppress(grpc.RpcError):
        ue_attach_info_dict = {
            ue_stub: task.result() for ue_stub, task in ue_attach_task_dict.items()  # Waiting for attach
        }

    if not ue_attach_info_dict:
        pytest.fail("Attach timeout reached")

    return ue_attach_info_dict


def _log_attached_ue(future: grpc.Future, ue_stub: UEStub):
    with suppress(grpc.RpcError):
        logging.info(
            "UE [%s] attached: \n%s%s ",
            id(ue_stub),
            ue_stub.GetDefinition(Empty()).subscriber,
            future.result(),
        )


def ping(
    ue_attach_info_dict: Dict[UEStub, UEAttachedInfo],
    epc: EPCStub,
    ping_count,
):
    """
    Ping command between an UE and a EPC
    """

    ping_success = True

    # For each attached UE
    for ue_stub, ue_attached_info in ue_attach_info_dict.items():
        # Launch both ping in parallel: ue -> epc and epc -> ue
        ue_to_epc = ue_stub.Ping.future(PingRequest(address=ue_attached_info.ipv4_gateway, count=ping_count))
        epc_to_ue = epc.Ping.future(PingRequest(address=ue_attached_info.ipv4, count=ping_count))

        # Wait both ping to end
        ue_to_epc_result: PingResponse = ue_to_epc.result()
        epc_to_ue_result: PingResponse = epc_to_ue.result()

        # Wait both ping to end & print result
        _print_ping_result(f"[{ue_attached_info.ipv4}] UE -> EPC", ue_to_epc_result)
        _print_ping_result(f"[{ue_attached_info.ipv4}] EPC -> UE", epc_to_ue_result)

        ping_success &= ue_to_epc_result.status and epc_to_ue_result.status

    if not ping_success:
        pytest.fail("Ping. Some packages got lost.")


def _print_ping_result(msg: str, result: PingResponse):
    log_fn = logging.info
    if not result.status:
        log_fn = logging.error
    log_fn("Ping %s: %s", msg, result)


def iperf(
    ue_attach_info_dict: Dict[UEStub, UEAttachedInfo],
    epc: EPCStub,
    protocol: IPerfProto,
    direction: IPerfDir,
    iperf_duration: int,
    bitrate: int,
    bitrate_threshold_ratio: float,  # real_bitrate > (bitrate_threshold_ratio * ideal_bitrate)
):
    """
    iperf command between an UE and a EPC
    """

    iperf_success = True

    # For each attached UE
    for ue_stub, ue_attached_info in ue_attach_info_dict.items():
        # Start IPerf Server
        task, iperf_request = iperf_start(ue_stub, ue_attached_info, epc, protocol, direction, iperf_duration, bitrate)
        iperf_success &= iperf_wait_until_finish(ue_attached_info, epc, task, iperf_request, bitrate_threshold_ratio)

    if not iperf_success:
        pytest.fail("iperf did not achieve the expected data rate.")


def iperf_start(
    ue: UEStub,  # pylint: disable=invalid-name
    ue_attached_info: UEAttachedInfo,
    epc: EPCStub,
    protocol: IPerfProto,
    direction: IPerfDir,
    duration: int,
    bitrate: int,
) -> Tuple[grpc.Future, IPerfRequest]:
    """
    Start a Iperf and keep it running
    """

    iperf_request = IPerfRequest(
        server=epc.StartIPerfService(String(value=ue_attached_info.ipv4_gateway)),
        duration=duration,
        direction=direction,
        proto=protocol,
        bitrate=bitrate,
    )

    # Run iperf
    task = ue.IPerf.future(iperf_request)

    logging.info(
        "Iperf %s [%s %s] started",
        ue_attached_info.ipv4,
        _iperf_proto_to_str(iperf_request.proto),
        _iperf_dir_to_str(iperf_request.direction),
    )

    return (task, iperf_request)


def iperf_wait_until_finish(
    ue_attached_info: UEAttachedInfo,
    epc: EPCStub,
    task: grpc.Future,
    iperf_request: IPerfRequest,
    bitrate_threshold_ratio: float,  # real_bitrate > (bitrate_threshold_ratio * ideal_bitrate)
) -> bool:
    """
    Wait until the requested iperf has finished.
    """

    # Stop server, get results and print it
    with suppress(grpc.RpcError):
        task.result()
    iperf_data: IPerfResponse = epc.StopIPerfService(iperf_request.server)
    logging.info(
        "Iperf %s [%s %s] result %s",
        ue_attached_info.ipv4,
        _iperf_proto_to_str(iperf_request.proto),
        _iperf_dir_to_str(iperf_request.direction),
        iperf_data,
    )

    # Assertion
    iperf_success = True
    if iperf_request.direction in (IPerfDir.DOWNLINK, IPerfDir.BIDIRECTIONAL):
        if iperf_data.downlink.bits_per_second == 0:
            logging.warning(
                "Downlink bitrate is 0. Requested: %s",
                iperf_request.bitrate,
            )
        elif iperf_data.downlink.bits_per_second < bitrate_threshold_ratio * iperf_request.bitrate:
            logging.warning(
                "Downlink bitrate too low. Requested: %s - Measured: %s",
                iperf_request.bitrate,
                iperf_data.downlink.bits_per_second,
            )
            iperf_success = False
    if iperf_request.direction in (IPerfDir.UPLINK, IPerfDir.BIDIRECTIONAL):
        if iperf_data.uplink.bits_per_second == 0:
            logging.warning(
                "Uplink bitrate is 0. Requested: %s",
                iperf_request.bitrate,
            )
        elif iperf_data.uplink.bits_per_second < bitrate_threshold_ratio * iperf_request.bitrate:
            logging.warning(
                "Uplink bitrate too low. Requested: %s - Measured: %s",
                iperf_request.bitrate,
                iperf_data.uplink.bits_per_second,
            )
            iperf_success = False
    return iperf_success


def _iperf_proto_to_str(proto):
    return {IPerfProto.TCP: "tcp", IPerfProto.UDP: "udp"}[proto]


def _iperf_dir_to_str(direction):
    return {
        IPerfDir.DOWNLINK: "downlink",
        IPerfDir.UPLINK: "uplink",
        IPerfDir.BIDIRECTIONAL: "bidirectional",
    }[direction]


def stop(
    ue_array: Sequence[UEStub],
    gnb: GNBStub,
    epc: EPCStub,
    retina_data: RetinaTestData,
    ue_stop_timeout: int = 0,  # Auto
    gnb_stop_timeout: int = 0,
    epc_stop_timeout: int = 0,
    log_search: bool = True,
    warning_as_errors: bool = True,
    fail_if_kos: bool = False,
):
    """
    Stop ue(s), gnb and epc
    """
    # Stop
    error_msg_array = []
    for index, ue_stub in enumerate(ue_array):
        error_msg_array.append(
            _stop_stub(
                ue_stub,
                f"UE_{index+1}",
                retina_data,
                ue_stop_timeout,
                log_search,
                warning_as_errors,
            )
        )
    error_msg_array.append(_stop_stub(gnb, "GNB", retina_data, gnb_stop_timeout, log_search, warning_as_errors))
    error_msg_array.append(_stop_stub(epc, "EPC", retina_data, epc_stop_timeout, log_search, warning_as_errors))

    # Fail if stop errors
    error_msg_array = list(filter(bool, error_msg_array))
    if error_msg_array:
        pytest.fail(
            f"Stop stage. {error_msg_array[0]}"
            + (("\nFull list of errors:\n - " + "\n - ".join(error_msg_array)) if len(error_msg_array) > 1 else "")
        )

    # Metrics after Stop
    metrics_msg_array = []
    for index, ue_stub in enumerate(ue_array):
        metrics_msg_array.append(_get_metrics(ue_stub, f"UE_{index+1}", fail_if_kos=fail_if_kos))
    metrics_msg_array.append(_get_metrics(gnb, "GNB", fail_if_kos=fail_if_kos))
    metrics_msg_array.append(_get_metrics(epc, "EPC", fail_if_kos=fail_if_kos))

    # Fail if metric errors
    metrics_msg_array = list(filter(bool, metrics_msg_array))
    if metrics_msg_array:
        pytest.fail(
            f"Metrics validation. {metrics_msg_array[0]}"
            + (("\nFull list of errors:\n - " + "\n - ".join(metrics_msg_array)) if len(metrics_msg_array) > 1 else "")
        )


def ue_stop(
    ue_array: Sequence[UEStub],
    retina_data: RetinaTestData,
    ue_stop_timeout: int = 0,  # Auto
    log_search: bool = True,
    warning_as_errors: bool = True,
):
    """
    Stop an array of UEs to detach from already running gnb and epc
    """
    error_msg_array = []
    for index, ue_stub in enumerate(ue_array):
        error_msg_array.append(
            _stop_stub(ue_stub, f"UE_{index+1}", retina_data, ue_stop_timeout, log_search, warning_as_errors)
        )
    error_msg_array = list(filter(bool, error_msg_array))
    if error_msg_array:
        pytest.fail(
            f"UE Stop. {error_msg_array[0]}"
            + (("\nFull list of errors:\n - " + "\n - ".join(error_msg_array)) if len(error_msg_array) > 1 else "")
        )


def _stop_stub(
    stub: RanStub,
    name: str,
    retina_data: RetinaTestData,
    timeout: int = 0,
    log_search: bool = True,
    warning_as_errors: bool = True,
) -> str:
    """
    Stop a stub in the defined timeout (0=auto).
    It uses retina_data to save artifacts in case of failure
    """

    error_msg = ""

    with suppress(grpc.RpcError):
        stop_info: StopResponse = stub.Stop(UInteger(value=timeout))

        if stop_info.exit_code:
            retina_data.download_artifacts = True
            error_msg = f"{name} crashed with exit code {stop_info.exit_code}. "

        if log_search:
            log_msg = f"{name} has {stop_info.error_count} errors and {stop_info.warning_count} warnings. "
            if stop_info.error_count:
                log_msg += f"First error is: {stop_info.error_msg}"
            elif stop_info.warning_count:
                log_msg += f"First warning is: {stop_info.warning_msg}"

            if stop_info.error_count or (warning_as_errors and stop_info.warning_count):
                retina_data.download_artifacts = True
                error_msg += log_msg
            elif not warning_as_errors and stop_info.warning_count:
                logging.warning(log_msg)

        if error_msg:
            logging.error(error_msg)
        else:
            logging.info("%s has stopped", name)

    return error_msg


def _get_metrics(
    stub: RanStub, name: str, fail_if_kos: bool = False, cpu_threshold: float = 90, ram_threshold: float = 90
) -> str:
    error_msg = ""

    with suppress(grpc.RpcError):
        metrics: Metrics = stub.GetMetrics(Empty())

        if metrics.nof_kos:
            kos_msg = f"{name} has {metrics.nof_kos} KOs"
            if fail_if_kos:
                logging.error(kos_msg)
                # error_msg += kos_msg
            else:
                logging.warning(kos_msg)
        if metrics.nof_retx:
            logging.warning("%s has %s retrxs", name, metrics.nof_retx)
        if metrics.nof_lates:
            logging.warning("%s has %s UHD Lates", name, metrics.nof_lates)
        if metrics.nof_under:
            logging.warning("%s has %s UHD Underflows", name, metrics.nof_under)
        if metrics.nof_seq_err:
            logging.warning("%s has %s UHD Sequence errors", name, metrics.nof_seq_err)

        if metrics.cpu_max >= cpu_threshold:
            logging.warning("%s CPU usage %s is over the threshold", name, metrics.cpu_max)
        if metrics.ram_max >= ram_threshold:
            logging.warning("%s RAM usage %s is over the threshold", name, metrics.ram_max)

    return error_msg
