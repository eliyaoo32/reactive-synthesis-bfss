import os.path
from glob import glob


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
    return os.path.join(output_path, filename + ".json")


def create_folder(folder_name):
    if not os.path.exists(folder_name):
        os.makedirs(folder_name)


OUTPUT_DIR_PATH = "./outputs"
TLSF_FILES_PATH = "../../benchmarks/tlsf"


if __name__ == "__main__":
    create_folder(OUTPUT_DIR_PATH)
    tlsf_files = get_all_unprocessed_tlsf_files(TLSF_FILES_PATH, OUTPUT_DIR_PATH)
    print("Found {} unprocessed TLSF files.".format(len(tlsf_files)))
