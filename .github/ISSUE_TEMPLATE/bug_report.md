---
name: Bug report
about: Create a report to help us improve
title: ''
labels: bug
assignees: ''

---

Please see the [guides](https://chaste.cs.ox.ac.uk/trac/wiki/ChasteGuides), and [discussions](https://github.com/Chaste/Chaste/discussions) for answers to common problems.

**Describe the bug**
A clear and concise description of the problem.

**To Reproduce**
Steps to reproduce the problem.

```
git clone --recursive https://github.com/Chaste/Chaste.git chaste
cd chaste
mkdir build
cmake ..
```

```
CMake Error: The source directory does not appear to contain CMakeLists.txt.
Specify --help for usage, or press the help button on the CMake GUI.
```

**Expected behavior**
A clear and concise description of what you expected to happen.

**System**
Please provide relevant details of your system, such as:
 * OS version
 * CMake version
 * C++ compiler version
 * Python version
 * Relevant Chaste dependency versions e.g. PETSc, Boost, Sundials etc.

Some of this information can be generated by running `TestChasteBuildInfo`:
```
git clone --recursive https://github.com/Chaste/Chaste.git chaste
cd chaste
mkdir build
cd build
cmake ..
cmake --build . --target TestChasteBuildInfo
ctest -V -R TestChasteBuildInfo
```

Please paste the full output in a [code block](https://docs.github.com/en/get-started/writing-on-github/working-with-advanced-formatting/creating-and-highlighting-code-blocks). It will be similar to the truncated example below. 
```
6:  ***** TestChasteBuildInfo.hpp *****
6: Entering TestShowInfo
6: <ChasteBuildInfo>
.
.
.
6: </ChasteBuildInfo>
6: Passed
6: OK!
1/1 Test #6: TestChasteBuildInfo ..............   Passed    2.38 sec

```

**Additional context**
Include additional information about the problem here if relevant.

**Screenshots**
If applicable, add screenshots to help explain your problem.