#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

import json
import os
from pathlib import Path
from typing import List

import yaml
from jsonschema import validate, ValidationError


def get_yml_file_list(folder: str) -> List[Path]:
    folder_path = Path(folder)
    file_list = [file for file in folder_path.glob("test_declaration*.yml")]
    return file_list


def validate_yaml(filepath: str, schema: dict):
    """
    Validate a yaml file against a json schema
    """
    with open(filepath, "r", encoding="UTF-8") as file:
        data = yaml.safe_load(file)
    try:
        validate(instance=data, schema=schema)
        print(f"The config file {filepath} is valid.")
    except ValidationError as e:
        print(f"The config file {filepath} is NOT valid:")
        print(e.message)
        exit(1)


def main():
    current_directory = Path(__file__).parent
    yaml_files = get_yml_file_list(current_directory.absolute())

    schema_filepath = current_directory.joinpath("test_declaration_schema.json").absolute()
    with open(schema_filepath, "r", encoding="UTF-8") as schema_file:
        schema = json.load(schema_file)

    for yaml_file in yaml_files:
        validate_yaml(str(yaml_file), schema)


if __name__ == "__main__":
    main()
