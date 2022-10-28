from pathlib import Path
from tqdm import tqdm
import sys
from glob import glob
import csv
import os


TLSF_FILES_PATH = '/Users/eliyahub/Documents/Thesis/reactive-synthesis-bfss/benchmarks/tlsf'
OUTPUT_BENCHMARKS = '../benchmarks.csv'


def get_all_tlsf_files(all_tlsf_file_path):
    all_tlsf_files = glob(all_tlsf_file_path + "/**/*.tlsf")
    return all_tlsf_files


def omit_spaces(string):
    return ','.join([x.strip() for x in string.split(",")])


def get_benchmark_name(tlsf_filepath):
    return Path(tlsf_filepath).stem


def generate_benchmark_from_tlsf(tlsf_filepath):
    benchmark_name = get_benchmark_name(tlsf_filepath)
    input_vars = omit_spaces(os.popen('syfco {} -ins'.format(tlsf_filepath)).read())
    output_vars = omit_spaces(os.popen('syfco {} -outs'.format(tlsf_filepath)).read())
    ltl_formula = os.popen('syfco {} -f ltlxba'.format(tlsf_filepath)).read().strip()

    return {
        'benchmark_name': benchmark_name,
        'input_vars': input_vars,
        'output_vars': output_vars,
        'ltl_formula': ltl_formula
    }


def main():
    all_tlsf_files = get_all_tlsf_files(TLSF_FILES_PATH)
    benchmarks = [
        generate_benchmark_from_tlsf(tlsf_filepath)
        for tlsf_filepath in tqdm(all_tlsf_files)
    ]

    if len(benchmarks) == 0:
        print("No benchmarks found.")
        sys.exit(1)

    with open(OUTPUT_BENCHMARKS, 'w', newline='', encoding='utf-8') as csvfile:
        dict_writer = csv.DictWriter(csvfile, benchmarks[0].keys())
        dict_writer.writeheader()
        dict_writer.writerows(benchmarks)


if __name__ == '__main__':
    main()
