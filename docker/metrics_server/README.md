# Metrics Server

This server receives metrics data from a running srsRAN Project gnb and send it to the configured InfluxDB.

## Run from cmd line

```bash
usage: python3 -m srs_metrics_server [-h] [--input INPUT] [--offline] [--db-config [DB_CONFIG ...]] [--bucket BUCKET]

Real time receiving and parsing srsRAN Project gnb metrics data and pushing it to influx db.

options:
  -h, --help            show this help message and exit
  --port PORT           Port to listen from.
  --offline
  --db-config [DB_CONFIG ...]
                        Data base configuration in the format "key=value key=value"
  --bucket BUCKET       Bucket to save the data.
  --testbed TESTBED     Testbed where srsRAN Project was run
```

For example:

```bash
metrics-server --port 55555 --bucket srsran --testbed default --db-config url=http://localhost:8086 org=srs token=605bc59413b7d5457d181ccf20f9fda15693f81b068d70396cc183081b264f3b
```

## Run from Docker

You can configure the metrics-server run by setting following variables:

- PORT
- BUCKET
- TESTBED
- URL
- ORG
- TOKEN

For example:

```bash
docker compose run -e PORT=55555 BUCKET=... srsran/metrics_server
```
