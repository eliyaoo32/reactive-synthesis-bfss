from subprocess import PIPE, Popen
from multiprocessing import Pool
import os
import csv
from random import shuffle


BENCHMARKS_PATH = '../benchmarks.csv'
OUTPUT_DIR = '../outputs'
MAX_WORKERS = 4
TIMEOUT_PER_BENCHMARK = '30s'
FIND_DEPENDENCIES_PATH = '../build/find_dependencies'
BENCHMARK_OUTPUT_FILE_FORMAT = '{}.out'


def get_benchmark_output_path(benchmark_name):
    return os.path.join(OUTPUT_DIR, BENCHMARK_OUTPUT_FILE_FORMAT.format(benchmark_name))


def is_benchmark_output_exists(benchmark_name):
    return os.path.exists(get_benchmark_output_path(benchmark_name))


def get_all_benchmarks():
    with open(BENCHMARKS_PATH, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        benchmarks = [
            {
                'benchmark_name': row['benchmark_name'],
                'input_vars': row['input_vars'],
                'output_vars': row['output_vars'],
                'ltl_formula': row['ltl_formula']
            }
            for row in reader
            if not is_benchmark_output_exists(row['benchmark_name'])
        ]

    return benchmarks


def process_benchmark(benchmark):
    benchmark_name = benchmark['benchmark_name']
    input_vars = benchmark['input_vars']
    output_vars = benchmark['output_vars']
    ltl_formula = benchmark['ltl_formula']

    print("Processing {}...".format(benchmark_name))

    config = {
        'process_timeout': TIMEOUT_PER_BENCHMARK,
        'find_deps_cli_path': FIND_DEPENDENCIES_PATH,
        'formula': ltl_formula,
        'inputs': input_vars,
        'outputs': output_vars
    }
    find_deps_cli = 'timeout --signal=HUP {process_timeout} {find_deps_cli_path} "{formula}" "{inputs}" "{outputs}"'.format(**config)

    with Popen(find_deps_cli, stdout=PIPE, stderr=PIPE, shell=True, preexec_fn=os.setsid) as process:
        result = process.communicate()[0].decode("utf-8")

    print("Done Processing {}!".format(benchmark_name))
    with open(get_benchmark_output_path(benchmark_name), "w+") as outfile:
        outfile.write(result)


def create_folder(folder_name):
    if not os.path.exists(folder_name):
        os.makedirs(folder_name)


def main():
    create_folder(OUTPUT_DIR)
    benchmarks = get_all_benchmarks()
    print("Found {} unprocessed benchmarks.".format(len(benchmarks)))
    shuffle(benchmarks)

    pool = Pool(processes=MAX_WORKERS)
    pool.map_async(process_benchmark, benchmarks)
    pool.close()
    pool.join()


if __name__ == "__main__":
    main()
