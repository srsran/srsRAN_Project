"""
Test ping
"""
from contextlib import suppress
import logging

import grpc

from pytest import mark
from retina.launcher.utils import param
from retina.launcher.test_base import BaseTest
from retina.protocol.base_pb2 import Empty, Integer
from retina.protocol.epc_pb2 import EPCStartInfo
from retina.protocol.gnb_pb2 import GNBStartInfo
from retina.protocol.ue_pb2 import IPerfDir, IPerfProto, IPerfRequest, UEStartInfo

from .utils import get_ue_gnb_epc


class TestIPerf(BaseTest):
    @mark.parametrize(
        "band, common_scs, bandwidth",
        (
            # param(3, 15, 10, marks=mark.intensive, id="band:%s-scs:%s-bandwidth:%s"),
            # param(3, 15, 15, marks=mark.intensive, id="band:%s-scs:%s-bandwidth:%s"),
            param(3, 15, 20, marks=mark.smoke, id="band:%s-scs:%s-bandwidth:%s"),
            # param(3, 15, 25, marks=mark.intensive, id="band:%s-scs:%s-bandwidth:%s"),
            # param(3, 15, 30, marks=mark.intensive, id="band:%s-scs:%s-bandwidth:%s"),
            # param(3, 15, 40, marks=mark.intensive, id="band:%s-scs:%s-bandwidth:%s"),
            # param(3, 15, 50, marks=mark.intensive, id="band:%s-scs:%s-bandwidth:%s"),
            # param(7, 15, 10, marks=mark.intensive, id="band:%s-scs:%s-bandwidth:%s"),
            # param(7, 15, 15, marks=mark.intensive, id="band:%s-scs:%s-bandwidth:%s"),
            # param(7, 15, 20, marks=mark.intensive, id="band:%s-scs:%s-bandwidth:%s"),
            # param(7, 15, 25, marks=mark.intensive, id="band:%s-scs:%s-bandwidth:%s"),
            # param(7, 15, 30, marks=mark.intensive, id="band:%s-scs:%s-bandwidth:%s"),
            # param(7, 15, 40, marks=mark.intensive, id="band:%s-scs:%s-bandwidth:%s"),
            # param(7, 15, 50, marks=mark.intensive, id="band:%s-scs:%s-bandwidth:%s"),
            param(41, 30, 20, marks=mark.smoke, id="band:%s-scs:%s-bandwidth:%s"),
        ),
    )
    @mark.parametrize(
        "direction",
        (
            param(IPerfDir.DOWNLINK, id="downlink"),
            param(IPerfDir.UPLINK, id="uplink"),
            param(IPerfDir.BIDIRECTIONAL, id="bidirectional"),
        ),
    )
    @mark.parametrize(
        "protocol",
        (
            param(IPerfProto.UDP, id="udp"),
            # param(IPerfProto.TCP, id="tcp"),
        ),
    )
    def test(
        self,
        extra,
        band,
        common_scs,
        bandwidth,
        protocol,
        direction,
        startup_timeout=120,
        attach_timeout=120,
        iperf_duration=20,
        bitrate=70e6,
    ):

        logging.info("Ping Test")

        with get_ue_gnb_epc(
            self,
            extra,
            band=band,
            common_scs=common_scs,
            bandwidth=bandwidth,
        ) as items:

            ue, gnb, epc = items

            ue_def = ue.GetDefinition(Empty())
            gnb_def = gnb.GetDefinition(Empty())
            epc_def = epc.GetDefinition(Empty())

            epc.AddUESubscriber(ue_def)
            epc.Start(EPCStartInfo())
            logging.info("EPC started")

            gnb.Start(
                GNBStartInfo(
                    ue_definition=ue_def,
                    epc_definition=epc_def,
                    timeout=startup_timeout,
                )
            )
            logging.info("GNB started")

            ue.Start(
                UEStartInfo(
                    gnb_definition=gnb_def,
                    timeout=startup_timeout,
                )
            )
            logging.info("UE started")

            ue_attached_info = ue.WainUntilAttached(Integer(value=attach_timeout))
            logging.info("UE Attached %s", ue_attached_info)

            server = epc.StartIPerfService(Empty())
            logging.info(
                "IPerf %s %s in progress",
                iperf_proto_to_str(protocol),
                iperf_dir_to_str(direction),
            )

            with suppress(grpc.RpcError):
                ue.IPerf(
                    IPerfRequest(
                        server=server,
                        duration=iperf_duration,
                        direction=direction,
                        proto=protocol,
                        bitrate=int(bitrate),
                    )
                )

            iperf_data = epc.StopIPerfService(server)
            logging.info("Iperf %s %s result %s", iperf_proto_to_str(protocol), iperf_dir_to_str(direction), iperf_data)


def iperf_proto_to_str(proto):
    return {IPerfProto.TCP: "tcp", IPerfProto.UDP: "udp"}[proto]


def iperf_dir_to_str(direction):
    return {
        IPerfDir.DOWNLINK: "downlink",
        IPerfDir.UPLINK: "uplink",
        IPerfDir.BIDIRECTIONAL: "bidirectional",
    }[direction]
