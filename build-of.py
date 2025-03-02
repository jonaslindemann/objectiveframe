import os, shutil, argparse, platform

cmake_preset_template = '''{
  "version": 3,
  "configurePresets": [
    {
      "name": "default",
      "toolchainFile": "%s"
    }
  ]
}'''

# c:/vcpkg/scripts/buildsystems/vcpkg.cmake

def setup_cmake_preset():
    if platform.system() == 'Windows':
        
        if not os.path.exists('CMakePresets.json'):

            print("\n--- Setting up toolchain file...\n")

            new_toolchain_file = None

            while os.path.exists(new_toolchain_file) == False:

                new_toolchain_file = input('Enter the path to the toolchain file (default: c:/vcpkg/scripts/buildsystems/vcpkg.cmake): ') or 'c:/vcpkg/scripts/buildsystems/vcpkg.cmake'

                if not os.path.exists(new_toolchain_file):
                    print(f'Error: Toolchain file not found at {new_toolchain_file}')
                    return

            with open('CMakePresets.json', 'w') as f:
                f.write(cmake_preset_template % (new_toolchain_file))

        else:
            print('CMakePresets.json already exists. Skipping...')


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
        

    def configure(self, preset='default', build_type='Release'):
        print("\n--- Configuring CMake...\n")

        self.build_type = build_type
        self.preset = preset

        os.environ['CMAKE_BUILD_PARALLEL_LEVEL'] = str(self.jobs)
        if platform.system() == 'Windows':
            os.system(f'cmake -S . -B {self.build_dir} -DCMAKE_BUILD_TYPE={self.build_type} -G"Visual Studio 17 2022" --preset={self.preset}')
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

    setup_cmake_preset()

    parser = argparse.ArgumentParser()
    parser.add_argument('--clean', action='store_true', help='Clean build directory')
    parser.add_argument('--configure', action='store_true', help='Configure project')
    parser.add_argument('--build', action='store_true', help='Build project')
    parser.add_argument('--build-type', choices=['Debug', 'Release'], default='Debug', help='Build type')
    parser.add_argument('--preset', default='default', help='CMake preset')
    parser.add_argument('--jobs', type=int, default=4, help='Number of jobs to run in parallel')
    args = parser.parse_args()

    cmake = CMakeRunner()
    cmake.jobs = args.jobs

    if args.clean:
        cmake.clean()
        return

    if args.configure:
        cmake.configure(args.preset, args.build_type)
        return

    if args.build:
        cmake.configure(args.preset, args.build_type)
        cmake.build(args.build_type)
        return


if __name__ == '__main__':
    main()