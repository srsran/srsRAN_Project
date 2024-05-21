#
# Copyright 2021-2024 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
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
