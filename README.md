# Bachelorproef
A repository used for keeping track of code and reports for my undergraduate thesis in Computer Science.
We use Java as our main programming language. We also use Unix so we can use planar graph generators such as plantri.

## Setup

Firstly, make sure you are working in Unix.

### Plantri
To check all graphs, we will use a simple planar graph generator called `plantri`. 
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
cd ../Code/optimizedSrc/
```

## General Usage

Example usage:
```
./colorScript.sh NUMBER_OF_VERTICES -c COLORING_METHOD
```

 - **COLORING_METHOD**: The default coloring method to be used is proper coloring, this can be changed by using the flag _-c_ with the coloring method as value. This coloring method should be like the following: `proper, odd, pCFo, iUMc, ...`.

 - **NUMBER_OF_VERTICES**: The amount of vertices of the graphs that are to be checked are (for right now) a mandatory argument. This should always be a value between 3 and 63, but this can also be written as a range of vertices. For example: `3:6`, the graphs with vertices between 3 and 6.

This will print out the graph6 strings of the graphs and the corresponding chromatic number of the coloring for this graph. It will also output the overall time it took to calculate everyting.

### Output

The output for `colorScript.sh` can be altered in three ways.

 - The first way is by using the flag `--raw`. 
This will make sure the only output outputted to the standard out are the corresponding chromatic numbers for the colorings.
 - The second way is by using a different flag, `--overview`. 
This will give an overview of the corresponding chromatic numbers for the colorings of the graphs. 
Showing the amount of graphs with a certain chromatic number, similar to how _nauty_ does it with `countg`.
 - And lastly, one can also use the flag `-f`, followed by a value.
This will make it so that only the graphs with a minimum chromatic number, specified by the value, will be shown.

Example usages: 
```
./colorScript.sh 6 -c proper --raw
./colorScript.sh 9 -c iCFo --overview
./colorScript.sh 10 --overview -f 4
```

> **_NOTE:_**  When giving flags to the program, the order doesn't matter.


### Showing graphs

One can choose to generate graph images for the outputted graphs by `colorScript.sh`.
This can be done by using the flag `--show`, followed by an optional value.
This value should be the format for the graph images. 
Possible formats for these images include: _emf, eps, pdf, png, ps, raw, rgba, svg, svgz, tex_.
The default value for show is _svg_.

Note that the first run using this option will take some time as 
it has to install all the needed libraries in the used python virtual environment.

Example usage:
```
./colorScript.sh 3:10 -c pUMo -f 6 --show
./colorScript.sh 6 --show pdf
./colorScript.sh 8 -f 4 -c odd --show tex
```

The images the script creates can be found in the directory `images/`, this will get created on launch.

> **_WARNING:_** 
Each time this option is chosen, all the existing images already in `images/` are removed.

### Manual usage

If one wants to manually enter a graph into the program, one can do so by using the flag _-m_, followed by the _graph6_ string.
Here it is important to always enter the _graph6_ string as a string. 
Giving the amount of vertices is not mandatory as this isn't used in the computation.

Example usage:
```
./colorScript.sh -m "H|tIIL|" -c proper
./colorScript.sh -m "I|tYJL`LO" -c pUMc --raw
```

## Checking Correctness

> **_NOTE:_** For this section, nauty should be installed in the main directory. Nauty can be installed from [here](https://users.cecs.anu.edu.au/~bdm/nauty/).

If one wants to check the correctness of the output of the program. This can be done by using the other programs `checkOutputs.sh`, `checkCycles.sh` and `checkNaiveOutputs.sh`. These bash-scripts do the following:

 - `checkOutputs.sh` compares the output from our own program to that of the nauty file _countg_. 
This is only done for the (normal) chromatic numbers, as _countg_ doesn't support other types.
 - `checkCycles.sh` checks the other coloring methods, by finding the amount of colors used in cycle graphs ($C_i$). 
These can then be compared to the known values for these types of graphs. 
This lets us check whether the other colorings are also correct. 
An overview of how the cycle graphs should be colored can be found [here](#overview-cycle-graphs)
 - `checkNaiveOutputs.sh` checks, as implied by the name, the outputs to those from the naive implementation.
As the naive implementation is very simple, we can assume that the output for this algorithm will be correct.
Therefore we can always compare our optimized algorithm to the naive algorithm.
The found differences between the two will be displayed.


The number of vertices or the coloring method should still be given to the program as explained before.

Example usage:
```
./checkOutputs.sh NUMBER_OF_VERTICES
./checkCycles.sh COLORING_METHOD
./checkNaiveOutputs.sh NUMBER_OF_VERTICES -c COLORING_METHOD
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
> | Number of vertices     | Odd chromatic number |
> |------------------------|----------------------|
> | `n` multiple of `3`    | 3                    |
> | `n = 5`                | 5                    |
> | other                  | 4                    |
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
> | Number of vertices     | iCFo chromatic number |
> |------------------------|-----------------------|
> | `n` multiple of `4`    | 2                     |
> | other                  | 3                     |
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
> |--------------------|-----------------------|
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
> |---------------------|-----------------------|
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
> |---------------------|-----------------------|
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
> | Number of vertices     | iUMo chromatic number |
> |------------------------|-----------------------|
> | `n` multiple of `4`    | 2                     |
> | other                  | 3                     |
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
> | Number of vertices     | iUMo chromatic number |
> |------------------------|-----------------------|
> | `n` multiple of `3`    | 2                     |
> | other                  | 3                     |
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
> | Number of vertices     | Odd chromatic number |
> |------------------------|----------------------|
> | `n` multiple of `3`    | 3                    |
> | `n = 5`                | 5                    |
> | other                  | 4                    |
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
> | Number of vertices     | Odd chromatic number |
> |------------------------|----------------------|
> | `n = 5`                | 4                    |
> | other                  | 3                    |
>
> </details>
>
> </div>
>
> </details>






