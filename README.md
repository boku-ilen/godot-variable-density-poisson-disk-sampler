# WIP: Poisson Disk Sampler GDExtension

Adapted from Steven Sell's [tutorial](https://www.vertexfragment.com/ramblings/variable-density-poisson-sampler/) and [Unity project](https://github.com/ssell/VariablePoissonSampler).

Repository created from the [GDExtension template](https://github.com/godotengine/godot-cpp-template).

### Configuring an IDE
You can develop your own extension with any text editor and by invoking scons on the command line, but if you want to work with an IDE (Integrated Development Environment), you can use a compilation database file called `compile_commands.json`. Most IDEs should automatically identify this file, and self-configure appropriately.
To generate the database file, you can run one of the following commands in the project root directory:
```shell
# Generate compile_commands.json while compiling
scons compiledb=yes

# Generate compile_commands.json without compiling
scons compiledb=yes compile_commands.json
```

