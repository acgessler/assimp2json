assimp2json
========

#### JSON exporter for Open Asset Import Library ####

### Introduction ###

`assimp2json` is a command line tool designed to expose the import capabilities of the Open Asset Import Library ("Assimp", http://assimp.sourceforge.net) to JavaScript/Web developers. The tool takes a single 3d model as input file, imports it using Assimp and converts its in-memory output data structure to a plain JSON representation.

assimp2json is platform-independent, its only dependency is Assimp itself.

### Output Format ###

The output format is a one-by-one translation of Assimp's C datastructure (http://assimp.sourceforge.net/lib_html/structai_scene.html), with a few changes to make the resulting JSON look more natural. All fields are lower-case and the prefixes (such as m, pc, ..) are omitted. Array lengths are not written because this information is implicitly contained in the arrays. Empty arrays are not written, i.e. a node without children doesn't have an empty `children:[]` field.

The /samples folder contains some sample JSON dumps.

### Build ###

The build system for assimp2json is CMake. Build as usual or check for CMake tutorials if you are new to it.

### Usage ###

``` 
$ assimp2json [flags] input_file [output_file] 
```

(omit the output argument to get the data on stdout)

Invoke `assimp2json` with no arguments for detailed information.









