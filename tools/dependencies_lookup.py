from multiprocessing import Pool
import os.path
from glob import glob
from random import shuffle
from subprocess import PIPE, Popen


# TODO: fetch those params via argparse
OUTPUT_DIR_PATH = "./outputs"
TLSF_FILES_PATH = "../benchmarks/tlsf"
TLSF_DEPENDECIES_TOOL_PATH = "./tlsf_dependecies"
FIND_DEPENDECIES_TOOL = "../src/cmake-build-debug/find_dependencies"
MAX_WORKERS = 4


def get_all_unprocessed_tlsf_files(all_tlsf_file_path, output_path):
    all_tlsf_files = glob(all_tlsf_file_path + "/**/*.tlsf")
    all_unprocessed_tlsf_files = [
        tlsf_path
        for tlsf_path in all_tlsf_files
        if not os.path.exists(get_tlsf_output_path(tlsf_path, output_path))
    ]

    return all_unprocessed_tlsf_files


def get_tlsf_output_path(tlsf_path, output_path):
    filename = os.path.basename(tlsf_path)
    return os.path.join(output_path, filename + ".txt")


def create_folder(folder_name):
    if not os.path.exists(folder_name):
        os.makedirs(folder_name)


def find_tlsf_dependencies(tlsf_file_path):
    print("Starting searching for dependencies in: {}".format(tlsf_file_path))
    tlsf_dependecies_cli = "{tlsf_dependecies_tool} {tlsf_file_path} {find_dependecies_tool}".format(
        tlsf_dependecies_tool=TLSF_DEPENDECIES_TOOL_PATH,
        tlsf_file_path=tlsf_file_path,
        find_dependecies_tool=FIND_DEPENDECIES_TOOL
    )

    with Popen(tlsf_dependecies_cli, stdout=PIPE, stderr=PIPE, shell=True, preexec_fn=os.setsid) as process:
        result = process.communicate()[0].decode("utf-8")

    print("Done searching for dependencies in: {}".format(tlsf_file_path))

    # Writing the results
    output_file = get_tlsf_output_path(tlsf_file_path, OUTPUT_DIR_PATH)
    with open(output_file, "w+") as outfile:
        outfile.write(result)


if __name__ == "__main__":
    create_folder(OUTPUT_DIR_PATH)
    all_tlsf_files = get_all_unprocessed_tlsf_files(TLSF_FILES_PATH, OUTPUT_DIR_PATH)
    shuffle(all_tlsf_files)

    print("Found {} unprocessed TLSF files.".format(len(all_tlsf_files)))

    pool = Pool(processes=MAX_WORKERS)
    pool.map_async(find_tlsf_dependencies, all_tlsf_files)
    pool.close()
    pool.join()
