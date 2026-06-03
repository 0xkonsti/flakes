import subprocess
import sys
from pathlib import Path

BUILD_DIR = "build"

BOLD = '\033[1m'
DIM = "\033[2m"
GREEN = "\033[0;32m"
LIGHT_GRAY = "\033[0;37m"
BLUE = "\033[0;34m"
RED = "\033[0;31m"
RESET = '\033[0m'

def run(cmd: list[str]):
    print(f"{DIM}>{RESET} {BOLD}{BLUE}{' '.join(cmd)}{RESET}")
    subprocess.check_call(cmd)

def help():
    print("Usage: python build.py [help|configure|build|clean|rebuild|release|format]")
    print("Commands:")
    print("  help      - Show this help message")
    print("  configure - Generate build files using CMake")
    print("  build     - Build the project using the generated build files")
    print("  clean     - Remove the build directory")
    print("  rebuild   - Clean, configure, and build the project")
    print("  release   - Configure and build the project in Release mode")
    print("  format    - Format the source code using the cmake target")

def configure(type: str = "Debug"):
    run([
        "cmake",
        "-B", BUILD_DIR,
        "-G", "Ninja",
        "-DCMAKE_C_COMPILER=clang",
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
        f"-DCMAKE_BUILD_TYPE={type}",
    ])

def build():
    run([
        "cmake",
        "--build", BUILD_DIR,
    ])

def clean():
    build_path = Path(BUILD_DIR)

    if build_path.exists():
        print(f"{DIM}> {BOLD}{RED}Removing build directory...{RESET}")
        import shutil
        shutil.rmtree(build_path)
        print(f"    {DIM}| {GREEN}Build directory removed.{RESET}")

def format():
    run([
        "cmake",
        "--build", BUILD_DIR,
        "--target", "format",
    ])

def tidy():
    run([
        "cmake",
        "--build", BUILD_DIR,
        "--target", "tidy",
    ])

def main():
    if len(sys.argv) < 2:
        help()
        return

    cmd = sys.argv[1]

    if cmd == "help":
        help()
        return

    elif cmd == "configure":
        configure()

    elif cmd == "build":
        if not Path(BUILD_DIR).exists():
            configure()
        build()

    elif cmd == "clean":
        clean()

    elif cmd == "rebuild":
        clean()
        configure()
        build()

    elif cmd == "release":
        clean()
        configure(type="Release")
        build()

    elif cmd == "format":
        format()

    elif cmd == "tidy":
        tidy()

    else:
        print(f"Unknown command: {cmd}")

if __name__ == "__main__":
    main()
