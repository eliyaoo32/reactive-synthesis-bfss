import os
from subprocess import PIPE, Popen
from multiprocessing import Pool
from datetime import datetime
from random import shuffle

from lookup_dependencies import create_folder, get_all_benchmarks

BENCHMARK_OUTPUT_FILE_FORMAT = '{}.hoa'


def get_benchmark_output_path(benchmark_name, output_dir):
    return os.path.join(output_dir, BENCHMARK_OUTPUT_FILE_FORMAT.format(benchmark_name))


def process_benchmark(benchmark, timeout, output_dir, synt_tool, algorithm):
    benchmark_name = benchmark['benchmark_name']
    input_vars = benchmark['input_vars']
    output_vars = benchmark['output_vars']
    ltl_formula = benchmark['ltl_formula']

    print("Processing {}...".format(benchmark_name))

    if synt_tool == 'ltlsynt':
        cli_cmd = 'timeout --signal=HUP {timeout} {synt_tool} --formula="{formula}" --ins="{inputs}" --outs="{outputs}" --algo={algo}'.format(
            timeout=timeout, synt_tool=synt_tool, formula=ltl_formula, inputs=input_vars, outputs=output_vars, algo=algorithm)
    else:
        raise Exception("Unknown tool {}".format(synt_tool))

    start_time = datetime.now()
    with Popen(cli_cmd, stdout=PIPE, stderr=PIPE, shell=True, preexec_fn=os.setsid) as process:
        process_communicate = process.communicate()
        result = process_communicate[0].decode(
            "utf-8") + process_communicate[1].decode("utf-8")

    total_time = datetime.now() - start_time
    result_header = "/* Total Duration: {} ms */\r\n".format(
        total_time.total_seconds() * 1000)
    result_header = "/* Tool: {} ms */\r\n".format(synt_tool)
    result_header = "/* Algorithm: {} ms */\r\n".format(algorithm)
    result = result_header + result

    print("Done Processing {}!".format(benchmark_name))
    with open(get_benchmark_output_path(benchmark_name, output_dir), "w+") as outfile:
        outfile.write(result)


def main():
    from argparse import ArgumentParser
    parser = ArgumentParser(add_help=True)
    parser.add_argument(
        '--name', help="Filter by benchmarks name", type=str, default='')
    parser.add_argument(
        '--benchs_list', help="A path to csv file which store benchmark instances", type=str, required=True)
    parser.add_argument(
        '--output_dir', help="Path to the directory which the output files are stored into", type=str, required=True)
    parser.add_argument(
        '--timeout', help="Timeout of each benchmark", type=str, default='40m')
    parser.add_argument('--all', help="Apply altough output file already exists ",
                        default=False, action='store_true')
    parser.add_argument(
        '--workers', help="Number of workers", type=int, default=16)
    parser.add_argument('--tool', help="Algorithm to use",
                        type=str, choices=['ltlsynt'], required=True)
    parser.add_argument('--algorithm', help="Algorithm to use, passed directly to the tool",
                        type=str, required=False)
    args = parser.parse_args()

    workers = args.workers
    benchmark_name_filter = args.name
    ignore_existing_output = not args.all
    benchmarks_path = args.benchs_list
    benchmarks_timeout = args.timeout
    output_dir = args.output_dir
    algorithm = args.algorithm
    synt_tool = args.tool

    """
    Search for benchmarks by configuration
    """
    create_folder(output_dir)
    benchmarks = get_all_benchmarks(
        output_dir=output_dir,
        ignore_if_output_exists=ignore_existing_output,
        benchmark_name_filter=benchmark_name_filter,
        benchmarks_path=benchmarks_path
    )
    print("Found {} benchmarks.".format(len(benchmarks)))
    shuffle(benchmarks)

    """
    Apply the algorithm
    """
    process_benchmark_args = [
        (benchmark, benchmarks_timeout, output_dir, synt_tool, algorithm)
        for benchmark in benchmarks
    ]
    with Pool(workers) as pool:
        pool.starmap(process_benchmark, process_benchmark_args)


if __name__ == "__main__":
    main()
