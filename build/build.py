#!/usr/bin/env python3
import sys
import os
from pyfastogt import build_utils


def print_usage():
    print("Usage:\n"
          "[required] argv[1] build type(release/debug)\n"
          "[required] argv[2] license key\n"
          "[optional] argv[3] license algo\n"
          "[optional] argv[4] prefix_path\n")


if __name__ == "__main__":
    argc = len(sys.argv)

    if argc > 1:
        build_type = sys.argv[1]
    else:
        print_usage()
        sys.exit(1)

    if argc > 2:
        license_key = sys.argv[2]
    else:
        print_usage()
        sys.exit(1)

    license_algo = 0
    if argc > 3:
        license_algo = sys.argv[3]

    prefix_path = '/usr/local'
    if argc > 4:
        prefix_path = sys.argv[4]

    pwd = os.getcwd()
    os.chdir('..')
    build_utils.build_command_cmake(prefix_path,
                                    ['-DLICENSE_KEY=%s' % license_key, '-DHARDWARE_LICENSE_ALGO=%s' % license_algo],
                                    build_type)
    os.chdir(pwd)
