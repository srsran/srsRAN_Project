"""
Test ping
"""
from collections import defaultdict
import logging
from contextlib import contextmanager, suppress

import grpc
import pytest
from pytest_html import extras
from retina.launcher.test_base import BaseTest
from retina.launcher.utils import get_ids, get_ids_band
from retina.protocol.base_pb2 import Empty, Integer, PingRequest
from retina.protocol.epc_pb2 import EPCStartInfo
from retina.protocol.gnb_pb2 import GNBStartInfo
from retina.protocol.ue_pb2 import UEAttachedInfo, UEStartInfo


class TestBasicE2ESuite(BaseTest):
    """
    Ping test class
    """

    BOOTUP_TIMEOUT = 30
    ATTACH_TIMEOUT = 30
    PING_COUNT = 10

    BANDWIDTH = [
        10,
        15,
        20,
        25,
        30,
        40,
        50,
    ]
    COMMON_SCS = [
        15,
    ]
    BAND_DECLARATION = [
        {"band": 3, "dl_arfcn": 368500},
        {"band": 7, "dl_arfcn": 536020},
    ]

    @pytest.mark.parametrize("bandwidth", BANDWIDTH, ids=get_ids(BANDWIDTH, "bandwidth"))
    @pytest.mark.parametrize("common_scs", COMMON_SCS, ids=get_ids(COMMON_SCS, "common_scs"))
    @pytest.mark.parametrize("band_declaration", BAND_DECLARATION, ids=get_ids_band(BAND_DECLARATION))
    def test_ping(self, extra, bandwidth, common_scs, band_declaration):
        """
        Test
        """

        logging.info("Ping Test")

        with self.get_items(extra, bandwidth, common_scs, band_declaration) as items:

            ue, gnb, epc = items

            ue_def = ue.GetDefinition(Empty())
            gnb_def = gnb.GetDefinition(Empty())
            epc_def = epc.GetDefinition(Empty())

            epc.AddUESubscriber(ue_def)

            # Start
            epc.Start(EPCStartInfo())
            logging.info("EPC started")

            gnb.Start(
                GNBStartInfo(
                    ue_definition=ue_def,
                    epc_definition=epc_def,
                    timeout=self.BOOTUP_TIMEOUT,
                )
            )
            logging.info("GNB started")

            ue.Start(
                UEStartInfo(
                    gnb_definition=gnb_def,
                    timeout=self.BOOTUP_TIMEOUT,
                )
            )
            logging.info("UE started")

            ue_attached_info: UEAttachedInfo = ue.WainUntilAttached(Integer(value=self.ATTACH_TIMEOUT))
            logging.info("UE Attached %s", ue_attached_info)

            ue_to_epc_ping_task = ue.Ping.future(PingRequest(address=epc_def.tun_ip, count=5))
            epc_to_ue_ping_task = epc.Ping.future(PingRequest(address=ue_attached_info.ipv4, count=5))

            ue_to_epc_ping_result = ue_to_epc_ping_task.result()
            epc_to_ue_ping_result = epc_to_ue_ping_task.result()

            logging.info("Ue to EPC Ping: %s", ue_to_epc_ping_result)
            logging.info("EPC to UE Ping: %s", epc_to_ue_ping_result)

            assert ue_to_epc_ping_result.status is True and epc_to_ue_ping_result.status is True

            logging.info("Test passed")

    @contextmanager
    def get_items(self, extra, bandwidth, common_scs, band_declaration):
        """
        Get items
        """

        try:

            test_config = {
                "ue": {
                    "parameters": {
                        "band": band_declaration["band"],
                        "dl_arfcn": band_declaration["dl_arfcn"],
                        "ssb_arfcn": get_ssb_arfcn(band_declaration["band"], bandwidth),
                        "common_scs": common_scs,
                        "bandwidth": bandwidth,
                        "sample_rate": get_sampling_rate(common_scs),
                        "tx_gain": 89,
                        "rx_gain": 73,
                    }
                },
                "gnb": {
                    "parameters": {
                        "band": band_declaration["band"],
                        "dl_arfcn": band_declaration["dl_arfcn"],
                        "common_scs": common_scs,
                        "bandwidth": bandwidth,
                        "sample_rate": get_sampling_rate(common_scs),
                        "tx_gain": 89,
                        "rx_gain": 73,
                    }
                },
            }
            self.retina_manager.parse_configuration(test_config)

            # Get clients
            ue = self.retina_manager.get_ue()
            gnb = self.retina_manager.get_gnb()
            epc = self.retina_manager.get_epc()

            yield ue, gnb, epc

        except Exception as err:
            logging.error("Test failed")
            raise err from None

        finally:

            with suppress(UnboundLocalError, NameError):
                extra.append(extras.json(test_config))
                extra.append(extras.url(self.relative_output_html_path))

            with suppress(NameError, grpc._channel._InactiveRpcError):
                return_code = gnb.Stop(Empty()).value
                if return_code:
                    logging.warning("GNB stopped with invalid exit code %s", return_code)
            with suppress(NameError, grpc._channel._InactiveRpcError):
                return_code = epc.Stop(Empty()).value
                if return_code:
                    logging.warning("EPC stopped with invalid exit code %s", return_code)
            with suppress(NameError, grpc._channel._InactiveRpcError):
                return_code = ue.Stop(Empty()).value
                if return_code:
                    logging.warning("UE stopped with invalid exit code %s", return_code)


SAMPLE_RATE_DICT = {15: 61.44, 30: 122.88, 60: 245.76, 120: 491.52}


def get_sampling_rate(common_scs):
    """
    Get sampling rate
    """
    return int(SAMPLE_RATE_DICT.get(common_scs, 61.44) * 1e6)

SSB_ARFCN_DICT = {
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
}

def get_ssb_arfcn(band, bandwidth):
    """
    Get SSB arfcn
    """
    return SSB_ARFCN_DICT[band][bandwidth]
