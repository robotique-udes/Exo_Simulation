# Exo_Simulation

## Table of contents

- [Description](#description)
- [Getting started](#getting-started)
	- [Dependencies](#dependencies)
	- [Build](#build)
- [Contributing](#contributing)
	- [Folder overview](#folder-overview)
	- [Guidelines](#guidelines)

## Description

This project is an OpenSim simulation to evaluate and compare the metabolic cost of different control loops for BioGenius.

## Getting started

### Dependencies

- OpenSim 4.6
- Visual Studio Community 2026 (For MSVC and CMake)

Follow the [installation guide](docs/articles/installation.md) for instructions.

### Build

Clone the repository
```bash
git clone https://github.com/robotique-udes/Exo_Simulation.git
cd Exo_Simulation
```

Generate the project using CMake or CMake GUI

<u>CMake</u>
```bash
mkdir build && cd build
cmake ..
```

<u>CMake GUI</u>
1. Open CMake GUI
2. In "Where is the source code", enter the path to the project
3. In "Where to build the binaries", add a `build` directory to the project's path. CMake GUI will automatically create this directory if it does not exist
4. At the bottom, click 'Configure' then 'Generate'. Keep the default options
![Sélectionner le *workload* "Desktop development with C++"](docs/Multimedia/CMakeGUI.png)

Open `build/simulation.sln` with Visual Studio. To run the project, press Ctrl+F5 or click on the "Start without debugging" button
![Click on the "Start without debugging" button](docs/Multimedia/VisualStudioRun.png)

## Contributing

### Folder overview

- [src](src): C++ source files (*.cpp)
- [include](include): C++ header files (*.hpp)
- [docs](docs): Documentation for the project
	- [articles](docs/articles): Procedures, guidelines, notes, etc (*.md)
	- [multimedia](docs/multimedia): Images and videos (*.png, *.jpg, *.mp4, etc)
- [opensim](opensim): All files required for the simulation
	- [Model](opensim/Model): OpenSim models (*.osim)
	- [Geometry](opensim/Geometry): Meshes used by models (*.stl)
	- [Motion](opensim/Motion): Motion files (*.mot)
	- [Config](opensim/Config): Configurations used by OpenSim tools (*.xml)
	- Results: Simulation results. NOT TRACKED BY GIT
- build: Build directory. NOT TRACKED BY GIT

### Guidelines

All contributors must read and follow the [C++ guidelines](docs/articles/code_guideline.md) and the [Git guidelines](docs/articles/git_guideline.md).