#!/usr/bin/env python3
"""
Simplified build script for ObjectiveFrame
The CMakeLists.txt now auto-detects vcpkg, so this script is optional.
You can also build directly with CMake commands.
"""
import os, shutil, argparse, platform


class CMakeRunner:
    def __init__(self):
        self.build_type = 'Release'
        self.debug_dir = 'build-debug'
        self.release_dir = 'build-release'
        self.preset = 'default'
        self.jobs = 4

    def clean(self):
        print("\n--- Cleaning build directories...\n")

        if os.path.exists(self.debug_dir):
            shutil.rmtree(self.debug_dir)
        if os.path.exists(self.release_dir):
            shutil.rmtree(self.release_dir)
        

    def configure(self, build_type='Release'):
        print("\n--- Configuring CMake...\n")

        self.build_type = build_type

        os.environ['CMAKE_BUILD_PARALLEL_LEVEL'] = str(self.jobs)
        if platform.system() == 'Windows':
            os.system(f'cmake -S . -B {self.build_dir} -DCMAKE_BUILD_TYPE={self.build_type} -G"Visual Studio 17 2022"')
        else:
            os.system(f'cmake -S . -B {self.build_dir} -DCMAKE_BUILD_TYPE={self.build_type}')

    def build(self, build_type='Release'):
        print("\n--- Building project...\n")
        self.build_type = build_type
        os.system(f'cmake --build {self.build_dir} --config {self.build_type}')

    @property
    def build_dir(self):
        return self.debug_dir if self.build_type == 'Debug' else self.release_dir

def main():
    parser = argparse.ArgumentParser(
        description='Simplified build script for ObjectiveFrame. CMakeLists.txt now auto-detects vcpkg.'
    )
    parser.add_argument('--clean', action='store_true', help='Clean build directory')
    parser.add_argument('--configure', action='store_true', help='Configure project')
    parser.add_argument('--build', action='store_true', help='Build project')
    parser.add_argument('--build-type', choices=['Debug', 'Release'], default='Release', help='Build type')
    parser.add_argument('--jobs', type=int, default=4, help='Number of jobs to run in parallel')
    args = parser.parse_args()

    cmake = CMakeRunner()
    cmake.jobs = args.jobs

    if args.clean:
        cmake.clean()
        return

    if args.configure:
        cmake.configure(args.build_type)
        return

    if args.build:
        cmake.configure(args.build_type)
        cmake.build(args.build_type)
        return

    # If no arguments provided, show usage
    parser.print_help()


if __name__ == '__main__':
    main()