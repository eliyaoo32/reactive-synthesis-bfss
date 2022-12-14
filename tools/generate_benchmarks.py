from pathlib import Path
from tqdm import tqdm
import sys
from glob import glob
import csv
import os


TLSF_FILES_PATH = os.path.join(
    Path(__file__).parent.resolve(), '../benchmarks/tlsf')
OUTPUT_BENCHMARKS = os.path.join(
    Path(__file__).parent.resolve(), '../benchmarks.csv')
ERROR_BENCHMARKS_PATH = os.path.join(
    Path(__file__).parent.resolve(), '../benchmarks-errors.csv')


def get_all_tlsf_files(all_tlsf_file_path):
    all_tlsf_files = glob(all_tlsf_file_path + "/**/*.tlsf")
    return all_tlsf_files


def omit_spaces(string):
    return ','.join([x.strip() for x in string.split(",")])


def get_benchmark_name(tlsf_filepath):
    return Path(tlsf_filepath).stem


def generate_benchmark_from_tlsf(tlsf_filepath):
    benchmark_name = get_benchmark_name(tlsf_filepath)
    input_vars = omit_spaces(
        os.popen('syfco {} -ins'.format(tlsf_filepath)).read()).lower()
    output_vars = omit_spaces(
        os.popen('syfco {} -outs'.format(tlsf_filepath)).read()).lower()
    ltl_formula = os.popen(
        'syfco {} -f ltlxba'.format(tlsf_filepath)).read().strip()

    return {
        'benchmark_name': benchmark_name,
        'input_vars': input_vars,
        'output_vars': output_vars,
        'ltl_formula': ltl_formula,
        'has_error': (ltl_formula == "" or input_vars == "" or output_vars == "")
    }


def main():
    all_tlsf_files = get_all_tlsf_files(TLSF_FILES_PATH)
    processed_benchmarks = [
        generate_benchmark_from_tlsf(tlsf_filepath)
        for tlsf_filepath in tqdm(all_tlsf_files)
    ]

    benchmarks = [b for b in processed_benchmarks if b['has_error'] == False]
    benchmarks_with_errors = [
        b for b in processed_benchmarks if b['has_error'] == True]

    if len(processed_benchmarks) == 0:
        print("No benchmarks found.")
        sys.exit(1)

    with open(OUTPUT_BENCHMARKS, 'w', newline='', encoding='utf-8') as csvfile:
        dict_writer = csv.DictWriter(csvfile, benchmarks[0].keys())
        dict_writer.writeheader()
        dict_writer.writerows(benchmarks)

    if len(benchmarks_with_errors) > 0:
        with open(ERROR_BENCHMARKS_PATH, 'w', newline='', encoding='utf-8') as csvfile:
            dict_writer = csv.DictWriter(
                csvfile, benchmarks_with_errors[0].keys())
            dict_writer.writeheader()
            dict_writer.writerows(benchmarks_with_errors)


if __name__ == '__main__':
    main()
