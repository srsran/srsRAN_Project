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

from contextlib import suppress
import logging
from typing import Iterable
import grpc

from retina.protocol.base_pb2 import (
    EPCDefinition,
    Empty,
    GNBDefinition,
    PingRequest,
    StartInfo,
    String,
    UEDefinition,
    UInteger,
)
from retina.protocol.epc_pb2 import IPerfResponse
from retina.protocol.epc_pb2_grpc import EPCStub
from retina.protocol.gnb_pb2 import GNBStartInfo
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto, IPerfRequest, UEAttachedInfo, UEAttachedInfoList, UEStartInfo
from retina.protocol.ue_pb2_grpc import UEStub

UE_STARTUP_TIMEOUT: int = 3 * 60  # RF requires more time
GNB_STARTUP_TIMEOUT: int = 5
EPC_STARTUP_TIMEOUT: int = UE_STARTUP_TIMEOUT
ATTACH_TIMEOUT: int = 120


# pylint: disable=too-many-arguments
def start_and_attach(
    ue: UEStub,  # pylint: disable=invalid-name
    gnb: GNBStub,
    epc: EPCStub,
    ue_startup_timeout: int = UE_STARTUP_TIMEOUT,
    gnb_startup_timeout: int = GNB_STARTUP_TIMEOUT,
    epc_startup_timeout: int = EPC_STARTUP_TIMEOUT,
    gnb_pre_cmd: str = "",
    gnb_post_cmd: str = "",
    attach_timeout: int = ATTACH_TIMEOUT,
) -> Iterable[UEAttachedInfo]:
    """
    Start stubs & wait until attach
    """

    ue_def: UEDefinition = ue.GetDefinition(Empty())
    gnb_def: GNBDefinition = gnb.GetDefinition(Empty())
    epc_def: EPCDefinition = epc.GetDefinition(Empty())

    for subscriber in ue_def.subscriber_list:
        epc.AddUESubscriber(subscriber)

    epc.Start(StartInfo(timeout=epc_startup_timeout))
    logging.info("EPC started")

    gnb.Start(
        GNBStartInfo(
            ue_definition=ue_def,
            epc_definition=epc_def,
            start_info=StartInfo(
                timeout=gnb_startup_timeout,
                pre_commands=gnb_pre_cmd,
                post_commands=gnb_post_cmd,
            ),
        )
    )
    logging.info("GNB started")

    ue.Start(
        UEStartInfo(
            gnb_definition=gnb_def,
            epc_definition=epc_def,
            start_info=StartInfo(timeout=ue_startup_timeout),
        )
    )
    logging.info("UE started")

    ue_attached_info_list: UEAttachedInfoList = ue.WainUntilAttached(UInteger(value=attach_timeout))
    logging.info("UEs attached %s", ue_attached_info_list)

    return ue_attached_info_list.value  # type: ignore


def ping(
    ue: UEStub,  # pylint: disable=invalid-name
    epc: EPCStub,
    ue_attached_info_list: Iterable[UEAttachedInfo],
    ping_count,
):
    """
    Ping command between an UE and a EPC
    """

    # For each attached UE
    for ue_attached_info in ue_attached_info_list:
        # Launch both ping in parallel: ue -> epc and epc -> ue
        ue_to_epc = ue.Ping.future(PingRequest(address=ue_attached_info.ipv4_gateway, count=ping_count))
        epc_to_ue = epc.Ping.future(PingRequest(address=ue_attached_info.ipv4, count=ping_count))

        # Wait both ping to end
        ue_to_epc_result = ue_to_epc.result()
        epc_to_ue_result = epc_to_ue.result()

        # Print status
        logging.info("Ping [%s] UE -> EPC: %s", ue_attached_info.ipv4, ue_to_epc_result)
        logging.info("Ping [%s] EPC -> UE: %s", ue_attached_info.ipv4, epc_to_ue_result)

        # Validate both ping results
        assert all(map(lambda r: r.status, (ue_to_epc_result, epc_to_ue_result))) is True, "Ping failed!"


def iperf(
    ue: UEStub,  # pylint: disable=invalid-name
    epc: EPCStub,
    ue_attached_info_list: Iterable[UEAttachedInfo],
    protocol: IPerfProto,
    direction: IPerfDir,
    iperf_duration: int,
    bitrate: int,
    bitrate_threshold_ratio: float,  # real_bitrate > (bitrate_threshold_ratio * ideal_bitrate)
):
    """
    iperf command between an UE and a EPC
    """

    # For each attached UE
    for ue_attached_info in ue_attached_info_list:
        # Start IPerf Server
        server = epc.StartIPerfService(String(value=ue_attached_info.ipv4_gateway))

        # Run iperf
        with suppress(grpc.RpcError):
            ue.IPerf(
                IPerfRequest(
                    server=server,
                    duration=iperf_duration,
                    direction=direction,
                    proto=protocol,
                    bitrate=bitrate,
                )
            )

        # Stop server, get results and print it
        iperf_data: IPerfResponse = epc.StopIPerfService(server)
        logging.info(
            "Iperf %s [%s %s] result %s",
            ue_attached_info.ipv4,
            _iperf_proto_to_str(protocol),
            _iperf_dir_to_str(direction),
            iperf_data,
        )

        # Assertion
        if direction in (IPerfDir.DOWNLINK, IPerfDir.BIDIRECTIONAL):
            if iperf_data.downlink.bits_per_second < bitrate_threshold_ratio * bitrate:
                logging.warning(
                    "Downlink bitrate too low. Requested: %s - Measured: %s",
                    bitrate,
                    iperf_data.downlink.bits_per_second,
                )
        if direction in (IPerfDir.UPLINK, IPerfDir.BIDIRECTIONAL):
            if iperf_data.uplink.bits_per_second < bitrate_threshold_ratio * bitrate:
                logging.warning(
                    "Uplink bitrate too low. Requested: %s - Measured: %s",
                    bitrate,
                    iperf_data.uplink.bits_per_second,
                )


def _iperf_proto_to_str(proto):
    return {IPerfProto.TCP: "tcp", IPerfProto.UDP: "udp"}[proto]


def _iperf_dir_to_str(direction):
    return {
        IPerfDir.DOWNLINK: "downlink",
        IPerfDir.UPLINK: "uplink",
        IPerfDir.BIDIRECTIONAL: "bidirectional",
    }[direction]
