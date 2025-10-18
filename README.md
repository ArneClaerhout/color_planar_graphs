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

If one wants to manually enter a graph into the program, one can do so by using the flag _-m_, followed by the graph6 string.
Here it is important to always enter the graph6 string as a string. 
Giving the amount of vertices is not mandatory as this isn't used in the computation.

Example usage:
```
./colorScript.sh -m "H|tIIL|" -c proper
./colorScript.sh -m "I|tYJL`LO" -c pUMc --raw
```

## Checking Correctness

> **_NOTE:_** For this section, nauty should be installed in the main directory. Nauty can be found [here](https://users.cecs.anu.edu.au/~bdm/nauty/).

If one wants to check the correctness of the output of the program. This can be done by using the program `compareOutputs.sh`. This bash-script compares the output from our own program to that of the nauty file `countg`. This is only done for the (normal) chromatic numbers, as `countg` doesn't support other types.

The number of vertices should still be given to the program as explained before.

Example usage:
```
./compareOutputs.sh NUMBER_OF_VERTICES
```




