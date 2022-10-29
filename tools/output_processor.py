import json
import re
from glob import glob
import os
from pathlib import Path

from search_dependencies_in_benchmark import get_all_benchmarks

OUTPUT_FOLDERS = '../outputs'
BENCHMARKS = '../benchmarks.csv'
SUMMARY_FILE = '../results.json'


def split_and_trim(string):
    if string == "":
        return []
    return [x.strip() for x in string.split(',')]


def get_all_output_files():
    return glob(os.path.join(OUTPUT_FOLDERS, '*.out'))


def process_output_file(output_file_path, benchmarks_information):
    benchmark_name = Path(output_file_path).stem
    info = next((x for x in benchmarks_information if x['benchmark_name'] == benchmark_name), None)

    processed_output = {
        'benchmark_name': benchmark_name,
        'input_vars': info['input_vars'].split(','),
        'output_vars': info['output_vars'].split(','),
        'ltl_formula': info['ltl_formula'],

        'is_completed_successfully': None,
        'total_duration': -1,
        'was_spec_constructed': None,
        'spec_construction_duration': -1,

        'dependent_vars': [],
        # Tested vars are elements with the from: { variable: value, test_duration: duration }
        'tested_vars': [],
    }

    with open(output_file_path, 'r') as f:
        output_file = f.read()

    for output_line in output_file.split("\n"):
        if ":" not in output_line:
            continue

        if '- Variable' in output_line:
            var_info = output_line.split("- Variable:")[-1].strip()
            processed_var = re.compile("(\S+)\s*Duration:\s*(.*)\s*", re.MULTILINE).match(var_info)
            processed_output['tested_vars'].append({
                'variable': processed_var.group(1),
                'duration': processed_var.group(2)
            })
            continue

        key, value = output_line.split(":")
        value = value.strip()

        if key.startswith("Total Duration"):
            processed_output['total_duration'] = value
        elif key.startswith("Spec construction duration"):
            processed_output['spec_construction_duration'] = value
        elif key.startswith("Is completed successfully"):
            processed_output['is_completed_successfully'] = value
        elif key.startswith("Was spec constructed"):
            processed_output['was_spec_constructed'] = value
        elif key.startswith("Dependent Variables"):
            processed_output['dependent_vars'] = split_and_trim(value)

    return processed_output


def main():
    output_files = get_all_output_files()
    benchmarks = get_all_benchmarks(ignore_existing=False)

    processed_outputs = [
        process_output_file(output_file, benchmarks)
        for output_file in output_files
    ]

    with open(SUMMARY_FILE, 'w+') as f:
        f.write(json.dumps(processed_outputs, indent=4))

    print("Summary is written to the file: {}".format(SUMMARY_FILE))


if __name__ == "__main__":
    main()
