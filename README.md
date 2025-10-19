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

The output for `colorScript.sh` can be altered in two ways.

 - The first way is by using the flag `--raw`. This will make sure the only output outputted to the standard out are the corresponding chromatic numbers for the colorings.
 - The second way is by using a different flag, `--overview`. This will give an overview of the corresponding chromatic numbers for the colorings of the graphs. Showing the amount of graphs with a certain chromatic number, similar to how _nauty_ does it with `countg`.

Example usages: 
```
./colorScript.sh 6 -c proper --raw
./colorScript.sh 9 -c iCFo --overview
```

> **_NOTE:_**  When giving flags to the program, the order of these flags don't matter.

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

> **_NOTE:_** For this section, nauty should be installed in the main directory. Nauty can be found [here](https://users.cecs.anu.edu.au/~bdm/nauty/).

If one wants to check the correctness of the output of the program. This can be done by using the other programs `checkOutputs.sh` and `checkCycles.sh`. These bash-scripts do the following:

 - `checkOutputs.sh` compares the output from our own program to that of the nauty file `countg`. 
This is only done for the (normal) chromatic numbers, as `countg` doesn't support other types.
 - `checkCycles.sh` checks the other coloring methods, by finding the amount of colors used in cycle graphs. 
These can then be compared to the known values for these types of graphs. 
This lets us check whether the other colorings are also correct.

A quick overview of how these graphs should be colored, ordered by coloring method, can be found in the section below.

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
> | `n = 5`                | 4                    |
> | other                  | 5                    |
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
> | `n = 5`             | 4                     |
> | other               | 5                     |
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
> | `n = 5`                | 4                    |
> | other                  | 5                    |
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


The number of vertices or the coloring should still be given to the program as explained before.

Example usage:
```
./checkOutputs.sh NUMBER_OF_VERTICES
./checkCycles.sh COLORING_METHOD
```




