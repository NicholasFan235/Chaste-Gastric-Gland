# Using CellML with (Cardiac) Chaste

There is a companion tool to Chaste, named chaste_codegen, which can generate Chaste-compatible C++ code from cardiac ionic cell models described in CellML.
This allows Chaste to make use of any such model.
It also applies some optimisations to improve the speed of simulations.
(Chaste_codegen is a python3 re-implementation of the legacy python2 tool PyCml)

Full documentation can be found on the Chaste wiki at:
https://chaste.cs.ox.ac.uk/trac/wiki/ChasteGuides/CodeGenerationFromCellML

Static versions of the guide accompanying Chaste releases are also available.
See e.g.:
https://chaste.cs.ox.ac.uk/chaste/tutorials/release_2021.1/ChasteGuides/CodeGenerationFromCellML.html
for release 2021.1.
