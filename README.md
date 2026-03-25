# Bachelorproef

A repository used for keeping track of code and reports for my undergraduate thesis in Computer Science.
We use Java as our main programming language. We also use Unix so we can use planar graph generators such as plantri.

## Setup

Firstly, make sure you are working in a Unix-like environment.

### Plantri

To check all graphs, we will use a planar graph generator called `plantri`.
To be able to use plantri, first, change the directory to plantri55 by executing this command:

```
cd plantri55/
```

Afterwards, run this command:

```
cc -o plantri -O4 plantri.c
```

This will compile plantri and make sure you can use it.

Now, go straight to the source directory.

```
cd ../code/v5-C/
```

Make sure a python virtual environment is created by running the following command:

```
./setupVenv.sh
```

This will create the venv and install all needed libraries.
Now you're ready to use the coloring algorithm!

## General Usage

The algorithm is easily usable, starting with the two first options:

- **NUMBER_OF_VERTICES**: The amount of vertices of the graphs that are to be checked is a mandatory argument.
  This should always be a value between 3 and 63, but this can also be written as a range of vertices.
  For example: `3:6`, the graphs with vertices between 3 and 6.

- **COLORING_METHOD**: (optional option) The default coloring method to be used is proper coloring,
  this can be changed by using the flag _-c_ with the coloring method as value.
  This coloring method should be like the following: `proper, odd, pCFo, iUMc, ...`.

When using the algorithm for the first time, be sure to also compile the code by adding the `-C` option to the `color` command.

Example usage:

```
./color.sh NUMBER_OF_VERTICES -c COLORING_METHOD -C
```

This will print out the graph6 strings of the graphs and the corresponding chromatic number of the coloring for this graph.
It will also output the overall time it took to calculate everyting.

_The outputs provided will almost always be written to a file, which can be found in the `outputs/` directory.
Each file getting a name corresponding to the time of when the file was created._

### Output

The output for `color.sh` can be altered in three ways:

- The first way is by using the flag `-r`.
  This will make sure the only output outputted to the standard out are the corresponding chromatic numbers for the colorings.
  One can also give this option a value: 1, 2 or 3.
  - A value of 1 makes sure the output is only the chromatic numbers of the graphs.
  - A value of 2 only outputs the graph6 strings of the graphs (only useful when filtering, explained later in this section).
  - A value of 3 only outputs the graph6 strings followed by the used colors ordered by index
    (this is used by the show function, also explained [later](#showing-graphs)).
- The second way is by using a different flag, `-o`.
  This will give an overview of the corresponding chromatic numbers for the colorings of the graphs.
  Showing the amount of graphs with a certain chromatic number, similar to how _nauty_ does it with `countg`.
  _(Choosing this option will not create a file containing the output)._
- And lastly, one can also use the flag `-f`, followed by a value.
  This will make it so that only the graphs with a minimum chromatic number, specified by the value, will be shown.

Example usages:

```
./color.sh 6 -c proper -r
./color.sh 9 -c iCFo -o
./color.sh 10 -of4
```

> **_NOTE:_** When giving flags to the program, the order doesn't matter.

### Progress in generation

Instead of just computing graphs without knowing when it would end, the progress view can be used.
This will use the `pv` command, which can be installed with:

```aiignore
sudo apt install pv
```

Using this progress view with the program is done with the `-p` option.
This will also generate an extra file: `graph_counts.json`.
This keeps track of the amount of graphs so it can quite accurately predict the needed time.

> **_NOTE:_** When first using this option, it will have to start by counting and therefore take longer to start.

After counting, a progress view is shown in the command view.

### Showing graphs

One can choose to generate graph images for the outputted graphs by `color.sh`.
This can be done by using the flag `-s`, followed by an optional value.
This value should be the format for the graph images.
Possible formats for these images include: _emf, eps, pdf, png, ps, raw, rgba, svg, svgz, tex_.
The default value for show is _png_.

Note that the first run using this option will take some time as
it has to install all the needed libraries in the used python virtual environment.

Example usage:

```
./color.sh 3:10 -c pUMo -f 6 -s
./color.sh 6 -s pdf
./color.sh 8 -f 4 -c odd -s tex
```

The images the script creates can be found in the directory `images/`, this will get created on launch.

> **_WARNING:_**
> Each time this option is chosen, all the existing images already in `images/` are removed.

### Faster computation

Parallelism of a computer can be used to speed up the computation process of coloring graphs.
This is done by splitting the given graphs (by plantri) up in multiple disjoint portions,
where each is then fed into their own process.
With modern-day computers, this dramatically speeds things up.

This multiprocessing can be done using the option `-M` with the amount of processes as the argument.

> **_NOTE:_** The usage of a progress view together with multiprocessing may not accurately predict the needed time.

Example usage:

```
./color.sh 3:10 -c pUMo -f6M4o
./color.sh 6 -c odd -M 2 -p
```

### Manual usage

If one wants to manually enter a graph into the program, one can do so by using the flag `-m`, followed by the _graph6_ string.
Here it is important to always enter the _graph6_ string as a string.
Giving the amount of vertices is not mandatory as this isn't used in the computation.

One can also choose to pipe their own graphs into the algorithm.
This is possible by giving a specific value with the manual option.
This value being `"pipe"`.
Do note that the piped strings should also be _graph6_ strings and should be divided by new lines (\n).
_(Choosing this option will not create a file containing the output)._

Example usage:

```
./color.sh --manual "H|tIIL|" -c proper
./color.sh -m "I|tYJL`LO" -c pUMc -r
echo "H~eKMD^" | ./color.sh -m pipe -c odd -o
```

## Checking Correctness

> **_NOTE:_** For this section, nauty should be installed in the main directory. Nauty can be installed [here](https://users.cecs.anu.edu.au/~bdm/nauty/).

If one wants to check the correctness of the output of the program. This can be done by using the other programs `checkOutputs.sh` and `checkCycles.sh`. These bash-scripts do the following:

- `checkOutputs.sh` compares the output from our own program in two ways:
  - If a proper coloring is given to be compared. Our own output is compared to that of the nauty file _countg_.
    This is only done for the (normal) chromatic numbers, as _countg_ doesn't support other types.
  - If other colorings are given, we compare the optimized script to the output of a previously checked implementation, this implementation is in turn checked with the naive algorithm.
    This is done by using `checkCorrectOutputs.sh`, found in scripts/.

  The script should be run with multiple arguments: the number of vertices and then each coloring method one wishes to use as a different argument. If all colorings should get checked, use `all` instead as an argument.

- `checkCycles.sh` checks the other coloring methods, by finding the amount of colors used in cycle graphs ($C_i$).
  These can then be compared to the known values for these types of graphs.
  This lets us check whether the other colorings are also correct.
  An overview of how the cycle graphs should be colored can be found [here](#overview-cycle-graphs)

The number of vertices or the coloring method should still be given to the program as explained before.

Usage:

```
./checkOutputs.sh NUMBER_OF_VERTICES COLORING_METHOD1 COLORING_METHOD2 ...
./checkOutputs.sh NUMBER_OF_VERTICES all
./checkCycles.sh COLORING_METHOD
./checkNaiveOutputs.sh NUMBER_OF_VERTICES -c COLORING_METHOD
```

Example usage:

```
./checkOutputs.sh 11 proper iUMo iCFo iCFc odd
./checkOutputs.sh 5:13 proper odd pCFc iUMc
./checkNaiveOutputs.sh 3:11 -c pUMo
./checkNaiveOutputs.sh 10
./checkCycles.sh pUMc
```

### Overview cycle graphs

Here one can find a quick overview of how cycle graphs ($C_i$) should be colored, ordered by coloring method, in the section below.

> <details>
> <summary>General other chromatic numbers for cycle graphs</summary>
>
> <div style="margin-left: 2em;">
>
> <details>
> <summary>Odd coloring</summary>
>
> | Number of vertices  | Odd chromatic number |
> | ------------------- | -------------------- |
> | `n` multiple of `3` | 3                    |
> | `n = 5`             | 5                    |
> | other               | 4                    |
>
> </details>
>
> </div>
>
> <div style="margin-left: 2em;">
>
> <details>
> <summary>Improper conflict-free coloring (open neighbourhood)</summary>
>
> | Number of vertices  | iCFo chromatic number |
> | ------------------- | --------------------- |
> | `n` multiple of `4` | 2                     |
> | other               | 3                     |
>
> </details>
>
> </div>
>
> <div style="margin-left: 2em;">
>
> <details>
> <summary>Improper conflict-free coloring (closed neighbourhood)</summary>
>
> | Number of vertices | iCFc chromatic number |
> | ------------------ | --------------------- |
> | all                | 2                     |
>
> </details>
>
> </div>
>
> <div style="margin-left: 2em;">
>
> <details>
> <summary>Proper conflict-free coloring (open neighbourhood)</summary>
>
> | Number of vertices  | pCFo chromatic number |
> | ------------------- | --------------------- |
> | `n` multiple of `3` | 3                     |
> | `n = 5`             | 5                     |
> | other               | 4                     |
>
> </details>
>
> </div>
>
> <div style="margin-left: 2em;">
>
> <details>
> <summary>Proper conflict-free coloring (closed neighbourhood)</summary>
>
> | Number of vertices  | pCFc chromatic number |
> | ------------------- | --------------------- |
> | `n` multiple of `2` | 2                     |
> | other               | 3                     |
>
> </details>
>
> </div>
>
> <div style="margin-left: 2em;">
>
> <details>
> <summary>Improper unique-maximum coloring (open neighbourhood)</summary>
>
> | Number of vertices  | iUMo chromatic number |
> | ------------------- | --------------------- |
> | `n` multiple of `4` | 2                     |
> | other               | 3                     |
>
> </details>
>
> </div>
>
> <div style="margin-left: 2em;">
>
> <details>
> <summary>Improper unique-maximum coloring (closed neighbourhood)</summary>
>
> | Number of vertices  | iUMo chromatic number |
> | ------------------- | --------------------- |
> | `n` multiple of `3` | 2                     |
> | other               | 3                     |
>
> </details>
>
> </div>
>
> <div style="margin-left: 2em;">
>
> <details>
> <summary>Proper unique-maximum coloring (open neighbourhood)</summary>
>
> | Number of vertices  | Odd chromatic number |
> | ------------------- | -------------------- |
> | `n` multiple of `3` | 3                    |
> | `n = 5`             | 5                    |
> | other               | 4                    |
>
> </details>
>
> </div>
>
> <div style="margin-left: 2em;">
>
> <details>
> <summary>Proper unique-maximum coloring (closed neighbourhood)</summary>
>
> | Number of vertices | Odd chromatic number |
> | ------------------ | -------------------- |
> | `n = 5`            | 4                    |
> | other              | 3                    |
>
> </details>
>
> </div>
>
> </details>
