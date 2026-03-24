# Positron Game Engine

A multiplatform 2D and 3D game engine built with C++ and CMake. The root `CMakeLists.txt` includes both the Engine and an Example Game.

---

## Prerequisites

Before you begin, make sure you have the following installed on your system:

### CMake (version 4.1 or higher)
- **Windows**: Download the installer from [cmake.org](https://cmake.org/download/) and make sure to tick "Add CMake to the system PATH" during setup
- **Linux**:
    - Ubuntu / Debian: `sudo apt install cmake`
    - Fedora / RHEL / CentOS: `sudo dnf install cmake`
    - Arch / Manjaro: `sudo pacman -S cmake`
    - openSUSE: `sudo zypper install cmake`
- **macOS**: Run `brew install cmake` if you have Homebrew, or download the installer from [cmake.org](https://cmake.org/download/)

### A C++20 compatible compiler
- **Windows**: Install [Visual Studio 2022](https://visualstudio.microsoft.com/) and select the "Desktop development with C++" workload, which includes MSVC. Alternatively, install [MinGW-w64](https://www.mingw-w64.org/) for GCC on Windows
- **Linux**:
    - Ubuntu / Debian: `sudo apt install g++`
    - Fedora / RHEL / CentOS: `sudo dnf install gcc-c++`
    - Arch / Manjaro: `sudo pacman -S gcc`
    - openSUSE: `sudo zypper install gcc-c++`

  > GCC is often pre-installed. Run `g++ --version` to check before installing.

- **macOS**: Install Xcode Command Line Tools by running `xcode-select --install` in Terminal. This gives you Clang

### A build tool: Ninja (recommended) or Make

> Make is usually pre-installed on Linux and macOS. Run `make --version` to check. If it is missing, install it using the steps below.

- **Windows**: Download Ninja from [github.com/ninja-build/ninja/releases](https://github.com/ninja-build/ninja/releases) and add it to your PATH. Make is not recommended on Windows
- **Linux** (Ninja):
    - Ubuntu / Debian: `sudo apt install ninja-build`
    - Fedora / RHEL / CentOS: `sudo dnf install ninja-build`
    - Arch / Manjaro: `sudo pacman -S ninja`
    - openSUSE: `sudo zypper install ninja`
- **Linux** (Make):
    - Ubuntu / Debian: `sudo apt install make`
    - Fedora / RHEL / CentOS: `sudo dnf install make`
    - Arch / Manjaro: `sudo pacman -S make`
    - openSUSE: `sudo zypper install make`
- **macOS** (Ninja): `brew install ninja`
- **macOS** (Make): `brew install make`

> **Tip:** If you use [JetBrains CLion](https://www.jetbrains.com/clion/), it handles all of the configuration and building for you automatically. Just open the project folder and CLion takes care of the rest.

---

## Building the Project

### 1. Clone the Repository

```bash
git clone https://github.com/noahbelton29/positron-game-engine
cd positron-game-engine
```

### 2. Create a Build Directory

```bash
mkdir build
cd build
```

### 3. Configure the Project

This step generates the build files for your system. Run one of the following depending on your platform and preferred build tool.

**Windows with MSVC and Ninja (recommended):**
```bash
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
```

**Windows with MSVC and no Ninja (uses the Visual Studio generator):**
```bash
cmake -G "Visual Studio 17 2022" ..
```

**Linux or macOS with Ninja (recommended):**
```bash
cmake -G Ninja ..
```

**Linux or macOS with Make:**
```bash
cmake -G "Unix Makefiles" ..
```

The `..` tells CMake to look in the parent directory for the root `CMakeLists.txt`.

You can also set the build type (Debug, Release, etc.) using `-DCMAKE_BUILD_TYPE`:
```bash
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
```

### 4. Compile

Once configuration is done, run the build command to compile the Engine and Example Game.

**With Ninja:**
```bash
ninja
```

**With Make:**
```bash
make
```

**With Visual Studio (Windows only):**
```bash
cmake --build . --config Release
```

This builds the Engine as a shared library and the Example Game as a runnable executable.

### 5. Run the Game (Optional)

**Linux and macOS:**
```bash
./Game
```

**Windows:**
```bash
Game.exe
```

Replace `Game` with the name of whichever executable you want to run.