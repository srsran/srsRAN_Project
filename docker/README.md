# SRSRAN Project Docker

To build and launch the gnb, please run:

```bash
docker compose -f docker/docker-compose.yml up
```

If you want to force a new build of the container, please add a `--build` flag to the previous command.

For more options, check `docker compose --help`

## Customizations

- Default docker compose uses `configs/gnb_rf_b200_tdd_n78_20mhz.yml` config file. You can change it by setting the variable `${GNB_CONFIG_PATH}` in the shell, in the `docker compose up` command line or using an external env-file. More info about how to do it in docker documentation here: <https://docs.docker.com/compose/environment-variables/set-environment-variables/>

- Network: If you are using an existing core-network on same machine, then you can comment the `5gc` service section and also link your srsran container to some existing AMF N1/N2 subnet, doing something like this:

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
