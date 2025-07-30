#!/usr/bin/env python3

from contextlib import suppress
import os
import json
from time import sleep
import websocket


def _on_open(ws: websocket.WebSocketApp):
    ws.send(json.dumps({"cmd": "metrics_subscribe"}))


def _on_message(_ws: websocket.WebSocketApp, message: str):
    with suppress(json.JSONDecodeError):
        metric = json.loads(message)
        if "cmd" not in metric:
            print(json.dumps(metric))


if __name__ == "__main__":
    ws_app = websocket.WebSocketApp(
        "ws://" + os.environ["WS_URL"],
        on_open=_on_open,
        on_message=_on_message,
    )
    while ws_app.run_forever():  # Returns False when the connection is closed
        sleep(1)
