#!/usr/bin/env python3

import bson
import click
import pymongo
import random

from misc.db.python.Open5GS import Open5GS


def add_user(imsi, key="00112233445566778899aabbccddeeff", op=None,
             opc="63bfa50ee6523365ff14c1f45f88737d", amf="9001", apn="srsapn", qci="9", ip_alloc=""):
    '''Add UE data to Open5GS mongodb'''

    if op is not None:
        opc = None

    slice_data = [
        {
            "sst": 1,
            "default_indicator": True,
            "session": [
                {
                    "name": apn,
                    "type": 3, "pcc_rule": [], "ambr": {"uplink": {"value": 1, "unit": 3}, "downlink": {"value": 1, "unit": 3}},
                    "qos": {
                        "index": int(qci),
                        "arp": {"priority_level": 8, "pre_emption_capability": 1, "pre_emption_vulnerability": 1}
                    },
                    "ue": {
                        "addr": ip_alloc
                    }
                },
                {
                    "name": "ims",
                    "type": 3, "pcc_rule": [], "ambr": {"uplink": {"value": 1, "unit": 3}, "downlink": {"value": 1, "unit": 3}},
                    "qos": {
                        "index": 5,
                        "arp": {"priority_level": 8, "pre_emption_capability": 1, "pre_emption_vulnerability": 1}
                    }
                }
            ]
        }
    ]

    sub_data = {
        "imsi": imsi,
        "subscribed_rau_tau_timer": 12,
        "network_access_mode": 2,
        "subscriber_status": 0,
        "access_restriction_data": 32,
        "slice": slice_data,
        "ambr": {"uplink": {"value": 1, "unit": 3}, "downlink": {"value": 1, "unit": 3}},
        "security": {
            "k": key,
            "amf": amf,
            "op": op,
            "opc": opc
        },
        "schema_version": 1,
        "__v": 0
    }

    return sub_data


def read_from_db(db_file):
    '''Read UE data from a subscriber db csv-file.'''
    subscriber_db = []
    db_file = open(db_file, "r")

    for line in db_file:
        if line.startswith("#"):
            pass
        else:
            name, imsi, key, op_type, op_c, amf, qci, ip_alloc = line.split(
                ',')
            opc = op_c
            op = None
            if op_type == "op":
                op = op_c
                opc = None

            subscriber_db.append({"imsi": imsi, "key": key, "op": op,
                                  "opc": opc, "amf": amf, "qci": qci, "ip_alloc": ip_alloc.rstrip()})

    return subscriber_db


def read_from_string(sub_data):
    '''
    Read UE data from subscriber data string.
    Example string: "001010123456780,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,8000,9,10.45.1.2"
    '''

    subscriber_db = []

    imsi, key, op_type, op_c, amf, qci, ip_alloc = sub_data.split(
        ',')
    opc = op_c
    op = None
    if op_type == "op":
        op = op_c
        opc = None

    subscriber_db.append({"imsi": imsi, "key": key, "op": op,
                          "opc": opc, "amf": amf, "qci": qci, "ip_alloc": ip_alloc.rstrip()})

    return subscriber_db


@click.command()
@click.option("--mongodb", default="127.0.0.1", help="IP address or hostname of the mongodb instance.")
@click.option("--mongodb_port", default=27017, help="Port to connect to the mongodb instance.")
@click.option("--subscriber_data", default="001010123456780,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,8000,9,10.45.1.2", help="Single subscriber data string or full path to subscriber data csv-file.")
def main(mongodb, mongodb_port, subscriber_data):

    open5gs_client = Open5GS(mongodb, mongodb_port)

    if subscriber_data.endswith(".csv"):
        print("Reading subscriber data from csv-file.")
        subscriber_db = read_from_db(subscriber_data)
    else:
        print("Reading subscriber data from cmd.")
        subscriber_db = read_from_string(subscriber_data)

    for ue in subscriber_db:
        try:
            sub_data = add_user(**ue)
            # Add Subscriber using dict of sub_data
            print(open5gs_client.AddSubscriber(sub_data))
        except pymongo.errors.DuplicateKeyError:
            print(f"UE (IMSI={ue['imsi']}) already exists, updating it.")
            sub_data = add_user(**ue)
            # Update Subscriber using dict of sub_data
            print(open5gs_client.UpdateSubscriber(ue['imsi'], sub_data))


if __name__ == "__main__":
    main()
