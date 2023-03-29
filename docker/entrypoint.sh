#!/usr/bin/env bash

set -euo pipefail

CONFIG_DIR="/srsran/etc"
UHD_IMAGES_DIR=/srsran/images

array=(${CONFIG_DIR}/gnb_config.yaml)

for c in "${array[@]}"; do
    # grep variable names (format: ${VAR}) from template to be rendered
    VARS=$(grep -oP '@[a-zA-Z0-9_]+@' ${c} | sort | uniq | xargs)
    echo "Now setting these variables '${VARS}'"

    # create sed expressions for substituting each occurrence of ${VAR}
    # with the value of the environment variable "VAR"
    EXPRESSIONS=""
    for v in ${VARS}; do
        NEW_VAR=$(echo $v | sed -e "s#@##g")
        if [[ -z ${!NEW_VAR+x} ]]; then
            echo "Error: Environment variable '${NEW_VAR}' is not set." \
                "Config file '$(basename $c)' requires all of $VARS."
            exit 1
        fi
        EXPRESSIONS="${EXPRESSIONS};s|${v}|${!NEW_VAR}|g"
    done
    EXPRESSIONS="${EXPRESSIONS#';'}"

    # render template and inline replace config file
    sed -i "${EXPRESSIONS}" ${c}
done

echo -e  "Done setting the configuration\n"

echo -e  "Running gNB Service \n"
/srsran/bin/gnb --gnb_id $GNB_ID -c /srsran/etc/gnb_config.yaml

# if [[ ${RUN_ZMQ_UE} == "yes" ]];then
#         echo -e  "Running UE Service \n"
#         ip netns add ue1
#         ./srsran/bin/srsue
# fi

exec "$@"

