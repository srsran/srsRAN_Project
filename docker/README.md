# srsRAN Project Multi-Container Solution

This folder contains multiple docker compose configurations for different deployment scenarios:

## Available Docker Compose Files

| Compose File | Services | Purpose |
|--------------|----------|---------|
| `docker-compose.yml` | `5gc`, `gnb` | Complete gNB + Core deployment |
| `docker-compose.split.yml` | `cu-cp`, `cu-up`, `du` | CU/DU split architecture that replace the gNB |
| `docker-compose.ui.yml` | `telegraf`, `influxdb`, `grafana` | Monitoring and metrics visualization |

## Quick Start

### Running the default stack

To just launch the solution with gNB + core network:

```bash
docker compose -f docker/docker-compose.yml up
```

or

```bash
cd docker/
docker compose up
```

### Combining Multiple Compose Files to run custom deployments

You can use [docker compose override feature](https://docs.docker.com/compose/how-tos/multiple-compose-files/merge/) to combine the compose files to create custom deployments:

```bash
# Run gNB + core
docker compose -f docker/docker-compose.yml up

# Run cu-cp + cu-up + du + core (gNB is replaced)
docker compose -f docker/docker-compose.yml -f docker/docker-compose.split.yml up

# Run gNB + core with monitoring stack
docker compose -f docker/docker-compose.yml -f docker/docker-compose.ui.yml up

# Run split architecture with monitoring stack
docker compose -f docker/docker-compose.yml -f docker/docker-compose.split.yml -f docker/docker-compose.ui.yml up

# Run only the monitoring components (useful when connecting to external gNB
docker compose -f docker/docker-compose.ui.yml up
```

## Service Management

### Extra Start Options

- To force a new build of the containers (including a new build of srsRAN gNB), please add a `--build` flag at the end of the previous command.
- To run it in background, please add a `-d` flag at the end of the previous command.
- For more options, check `docker compose up --help`

### See logs

To see services' output, you can run:

```bash
docker compose logs [OPTIONS] [SERVICE...]
```

- For more options, check `docker compose logs --help`

### Tear down the deployment

To stop any deployment:

```bash
docker compose -f docker/docker-compose.yml -f docker/docker-compose.split.yml -f docker/docker-compose.ui.yml down --remove-orphans
```

- If you also want to remove all internal data except the setup, you can add `--volumes` flag at the end of the previous command.
- For more options, check `docker compose down --help`

If you're not familiarized with `docker compose` tool, it will be recommended to check its [website](https://docs.docker.com/compose/) and `docker compose --help` output.

## Configuration

### Enabling metrics reporting in the gNB

To be able to see gNB's metrics in the monitoring UI (Grafana + InfluxDB + Telegraf), it's required to enable metrics reporting in the gNB config.

**Note**: When using the monitoring stack (`docker-compose.ui.yml` or combined deployments), ensure your gNB configuration includes:

```yml
metrics:
  autostart_stdout_metrics: true
  enable_json: true
remote_control:
  bind_addr: 0.0.0.0
  enabled: true
```

`gnb` and `du` services already have those options configured in their respective docker-compose.yml files.

### Customizations

- Default docker compose uses `configs/gnb_rf_b200_tdd_n78_20mhz.yml` config file. You can change it by setting the variable `${GNB_CONFIG_PATH}` in the shell, in the `docker compose up` command line or using the existing env-file `.env`. More info about how to do it in docker documentation here: <https://docs.docker.com/compose/environment-variables/set-environment-variables/>

F.e.:

```bash
# Set variables for specific deployment
export GNB_CONFIG_PATH=configs/gnb_custom.yml
docker compose -f docker-compose.yml -f docker-compose.ui.yml up
```

- Network: If you are using an existing core-network on same machine, then you can comment the `5gc` service section and also link your srsran container to some existing AMF N2/N3 subnet, doing something like this:

```yml
  gnb: ...
    networks:
      network1:
          ipv4_address: 192.168.70.163 # Setting a fixed IP in the "network1" net

networks:
  network1:
    name: my-pre-existing-network
    external: true
```

More info here: <https://docs.docker.com/compose/networking/>

### Open5GS Container Parameters

Advanced parameters for the Open5GS container are stored in [open5gs.env](open5gs/open5gs.env) file. You can modify it or use a totally different file by setting `OPEN_5GS_ENV_FILE` variable like in:

```bash
OPEN_5GS_ENV_FILE=/my/open5gs.env docker compose -f docker/docker-compose.yml up 5gc
```

The following parameters can be set:

- MONGODB_IP (default: 127.0.0.1): This is the IP of the mongodb to use. 127.0.0.1 is the mongodb that runs inside this container.
- SUBSCRIBER_DB (default: "001010123456780,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,8000,10.45.1.2"): This adds subscriber data for a single or multiple users to the Open5GS mongodb. It contains either:
  - Comma separated string with information to define a subscriber
  - `subscriber_db.csv`. This is a csv file that contains entries to add to open5gs mongodb. Each entry will represent a subscriber. It must be stored in `srsgnb/docker/open5gs/`
- OPEN5GS_IP: This must be set to the IP of the container (here: 10.53.1.2).
- UE_IP_BASE: Defines the IP base used for connected UEs (here: 10.45.0).
- DEBUG (default: false): This can be set to true to run Open5GS in debug mode.

For more info, please check it's own [README.md](open5gs/README.md).

### Open5GS Container Applications

Open5Gs container includes other binaries such as

- 5gc: 5G Core Only
- epc: EPC Only
- app: Both 5G Core and EPC

By default 5gc is launched. If you want to run another binary, remember you can use `docker compose run` to run any command inside the container. For example:

```bash
docker compose -f docker/docker-compose.yml run 5gc epc -c open5gs-5gc.yml
```

If you need to use custom configuration files, remember you can share folder and files between your local PC (host) and the container:

```bash
docker compose -f docker/docker-compose.yml run -v /tmp/my-open5gs-5gc.yml:/config/my-open5gs-5gc.yml 5gc epc -c /config/my-open5gs-5gc.yml
```

### Metric UI Setup

Change the environment variables define in `.env` that are used to setup and deploy the stack

```bash
├── .env         <---
├── docker-compose.yml
├── Dockerfile
└── ...
```

You can access grafana in <http://localhost:3300>. By default, you'll be in view mode without needing to log in. If you want to modify anything, you need to log in using following credentials:

- username: `admin`
- password: `admin`

After your fist log, it will ask you to change the password for a new one, but it can be skipped.

Provisioned Dashboards are into `Home > Dashboards`. **They don't support variable substitution**, so if you change default values in `.env` file, you'll need to go to `grafana/dashboards/` and manually search and replace values such as influxdb uid or bucket in every `.json` file.
