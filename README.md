# Bachelor's thesis on the coloring of planar graphs

A repository used for keeping track of code and reports for my undergraduate thesis in Computer Science. The main goal is the **coloring of planar graphs using a backtracking algorithm**. 
We use C as our main programming language. Unix is also used to allow the use of the planar graph generator _plantri_.

The possible colorings this program can do are these _vertex_ colorings:
- Proper colorings
- Odd colorings
- Conflict-free colorings (4 variants: open/closed neighborhood, proper/improper constraint)
- Unique-maximum colorings (4 variants: open/closed neighborhood, proper/improper constraint)

## Setup

Firstly, make sure you are working in a Unix-like environment.

To check all graphs, we will use a planar graph generator called `plantri`. It can be installed from [here](https://users.cecs.anu.edu.au/~bdm/plantri/).
The algorithm can use plantri if it is installed in the _main_ directory (_color_planar_graphs_ directory). So please install it there before doing anything else.

Some parts of the program may need the use of `nauty`. This can be installed from [here](https://users.cecs.anu.edu.au/~bdm/nauty/) and should also be put in the _main_ directory.

Afterwards, go straight to the code source directory.

```
cd code/
```

Make sure a python virtual environment is created by running the following command:

```
./scripts/setup_venv.sh
```

This will create a virtual environment and install all needed libraries inside it.
Now you're ready to use the coloring algorithm!

## General Usage

The algorithm is easily usable by using the `./color.sh` program. Different options are possible:

- **Compiling**: The code is automatically compiled when using the `-C` option.
  This option is needed when using the program for the first time.
  When inputting larger graphs ($63 <$ # vertices $< 127$), a 2 can be added as an extra argument to compile using different datastructures.

- **NUMBER_OF_VERTICES**: Only when inputting your own graphs into the program does it not require the number of vertices (how this is done is explained [later](#input)).
  This should always be a value between 3 and 63, but this can also be written as a range of vertices.
  For the program to pick this up, the number of vertices should **always** be the first argument.
  For example: $3:6$, the graphs with vertices between 3 and 6 (including 3 and 6).

- **COLORING_METHOD**: The default coloring method to be used is proper coloring,
  this can be changed by using the flag `-c` with the coloring method as value.
  This coloring method should be like the following: `proper, odd, pCFo, iUMc, ...`.

This will output the graph6 strings of the graphs and their corresponding chromatic number of the coloring for this graph.

_The outputs provided will almost always be written to a file, which can be found in the `outputs/` directory.
Each file getting a name corresponding to the time of when the file was created. This is useful for the reusing of outputs._

Example usage:

```
./color.sh NUMBER_OF_VERTICES -c COLORING_METHOD -C
./color.sh 64 -c iCFo -C 2
```

## Output

The output for `color.sh` can be altered by using some other flags:

- `-r`: The 'raw' option.
  
  This will make sure the output is only the corresponding chromatic numbers for the colorings. Therefore, only outputting the raw data.
  You can also give this option a value: 1, 2 or 3.
  - A value of 1 makes sure the output is only the chromatic numbers of the graphs.
  - A value of 2 only outputs the graph6 strings of the graphs (useful after filtering to reuse the output).
  - A value of 3 only outputs the graph6 strings followed by a valid coloring of the graph
    (this is used when generating images, also explained [later](#graph-images)).
- `-o`: The overview option.
  
  This will give an overview of the corresponding chromatic numbers for the colorings of the graphs.
  Showing the amount of graphs with a certain chromatic number, similar to how _nauty_ does this with `countg`.
  _(Choosing this option will not create a file containing the output)._
- `-f`: The filtering option.
  
  This always requires a value. Only graphs with a minimal chromatic of the given value are actually processed. Others are discarded.
- `-n`: The filter file option. _(uses nauty)_
  
  This option will use a filter file, the relative path given as the value, to filter graphs before coloring.
  An example filtering file is [this file](code/example_filter.json).
  Most options allow the adding of the number of vertices as a value. Options include:
  - Number of vertices: _n_ (again possible in a range)
  - Number of edges: _e_
  - Number of cycles: _cycles_
  - Minimum degree: _min_degree_
  - Maximum degree: _max_degree_
  - Girth: _girth_
  - And more...
 
> **_NOTE:_** The order of flags doesn't matter, only the position of the number of vertices is important.

Example usages:

```
./color.sh 6 -c proper -r
./color.sh 9 -c iCFo -o
./color.sh 10 -o -f 4
./color.sh -n 'example_filer.json' -f 4 -c iCFc
```

## Input

The input to the program is automatically configured to use plantri. The default option for the program will only use planar triangluations.
This can be changed by the user when using the following options:

- `-a`: The all planar graphs option.
  
  This option will, instead of only processing planar triangulations, make the program process _all_ planar graphs.
- `-m`: The manual option.
  
  This option allows you to enter a single graph you wish to color as the value.
  Additionally, this can also be used with the `"pipe"` value to input your own pipe input.
  Do note that the piped strings should also be _graph6_ strings and should be divided by new lines (\n).
- `-F`: The input file option.
  
  This option with a given file name (relative to `./color.sh`) option will use that file's graphs instead of _plantri_.
  **Multiprocessing** is also supported for this option.

- `-S`: The subdivide option.

  This option will automatically subdivide all graphs that are being used as input.
  This could be used to disprove conjectures.


Example usage:

```
./color.sh -m 'H|tIIL|' -c proper
echo 'H~eKMD^' | ./color.sh -m pipe -c odd -o
./color.sh 3:10 -c proper -a -o
./color.sh -F 'input.g6' -c pCFo -r
```

## Progress in Generation

Instead of just computing graphs without knowing when the program would end, the progress view can be used.
This will use the `pv` command, which can be installed with:

```aiignore
sudo apt install pv
```

Using this progress view with the program is done with the `-p` option.
This will use an extra file: `graph_counts.json`.
This keeps track of the amount of graphs so it can quite accurately predict the needed time.

> **_NOTE:_** This doesn't always work, for example when using an input file using `-F`.

## Graph Images

You can choose to generate graph images for the outputted graphs by `color.sh`.
This can be done by using the flag `-s`, followed by an optional value.
The images generated can be found in the directory `images/`, this will get created at start.

The value is the format for the graph images.
Possible formats for these images include: _emf, eps, pdf, png, ps, raw, rgba, svg, svgz, tex_.
The default value for the show option is _png_.

> **_WARNING:_**
> Each time this option is chosen, all the existing images already in `images/` are removed.

Example usage:

```
./color.sh 3:10 -c pUMo -f 6 -s
./color.sh 6 -s pdf
./color.sh 8 -f 4 -c odd -s tex
```

## Faster Computation

Parallelism of a computer can be used to speed up the computation process of coloring graphs.
This is done by splitting the given graphs (given by plantri) up in multiple disjoint portions,
where each is then fed into their own process.
With modern-day computers, this dramatically speeds up the coloring process.

This multiprocessing can be done using the option `-M` with the amount of processes as the value.

> **_NOTE:_** The usage of a progress view together with multiprocessing may not accurately predict the needed time.
> 
> Also, ending the program prematurely while this option is enabled may leave some used files in the `outputs/` directory.

Example usage:

```
./color.sh 3:10 -c pUMo -f 6 -M 4 -o
./color.sh 6 -c odd -M 2 -p
```

## Gadget Finding

The program can also be used to find gadgets. Functionality was added to check all possible colorings of a graph. This makes the checking for certain conditions on graphs possible.
This option is accessed with `-x`, and checks the following things:

- For **Odd** coloring:
  
  It checks whether there are vertices that 'see' all other colors an odd amount of times.
- For **Conflict-free** colorings:
  
  It checks whether there are any vertices that 'see' all other colors at least once for every possible coloring.
- For **Unique-maximum** colorings:
  
  It checks whether there are vertices in the graph that always or never have a certain color
  (with both being checked when no value is given, only the first being checked with a value of 1, and the latter being checked with a value of 2).

These rules were used in the construction of graphs with the best-known lower bounds. Using these rules could therefore maybe lead to a new lower bound.

Example usage:

```
./color.sh 3:10 -c pUMo -f 6 -M 4 -x 1
./color.sh 14 -c iCFo -M 10 -p -x -f 3
```

### Extra

Some code in the `code/` directory has also been provided to construct bigger graphs given gadgets and smaller graphs. 
These are the <tt>addGraphToIndex</tt> and <tt>replaceEdgeByGraph</tt> methods found in `code/graph.c`. 
They can be used in the finding of new gadgets, or when constructing bigger graphs.


