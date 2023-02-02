import logging
from collections import defaultdict
from contextlib import contextmanager, suppress

import grpc
from pytest_html import extras
from retina.protocol.base_pb2 import Empty


@contextmanager
def get_ue_gnb_epc(extra, relative_output_html_path, retina_manager, band, common_scs, bandwidth):

    try:

        test_config = {
            "ue": {
                "parameters": {
                    "band": band,
                    "dl_arfcn": get_dl_arfcn(band),
                    "ssb_arfcn": get_ssb_arfcn(band, bandwidth),
                    "common_scs": common_scs,
                    "bandwidth": bandwidth,
                    "sample_rate": get_sampling_rate(common_scs),
                    "tx_gain": 89,
                    "rx_gain": 73,
                }
            },
            "gnb": {
                "parameters": {
                    "band": band,
                    "dl_arfcn": get_dl_arfcn(band),
                    "common_scs": common_scs,
                    "bandwidth": bandwidth,
                    "sample_rate": get_sampling_rate(common_scs),
                    "tx_gain": 89,
                    "rx_gain": 73,
                }
            },
        }
        retina_manager.parse_configuration(test_config)

        # Get clients
        ue = retina_manager.get_ue()
        gnb = retina_manager.get_gnb()
        epc = retina_manager.get_epc()

        yield ue, gnb, epc

        logging.info("Test passed")

    except Exception as err:
        logging.error("Test failed")
        raise err from None

    finally:

        with suppress(UnboundLocalError, NameError):
            extra.append(extras.json(test_config))
            extra.append(extras.url(relative_output_html_path))

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


def get_sampling_rate(common_scs):
    """
    Get sampling rate
    """
    return int({15: 61.44, 30: 122.88, 60: 245.76, 120: 491.52}.get(common_scs, 61.44) * 1e6)


def get_dl_arfcn(band):
    return {3: 368500, 7: 536020}[band]


def get_ssb_arfcn(band, bandwidth):
    """
    Get SSB arfcn
    """
    return {
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
    }[band][bandwidth]
