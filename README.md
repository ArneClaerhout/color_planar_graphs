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

Now, return to the main directory.
```
cd ..
```

### Code
To use the code, change the directory to the source directory:
```
cd src/
```

Now that we are in the source folder, we can compile the code:
```
javac Main.java
```

## Usage
The default coloring method to be used is proper coloring, this can be changed by using the flag _-c_ with the coloring method as value.
This coloring method should be a string such as: `"proper", "odd", "pCFo", "iUMc", ...`.

Example usage:
```
./../plantri55/plantri -g n | java Main -c "iCFo"
```
n is the amount of vertices of the generated graphs, this should be a value so that $3 \le n \le 62$.

This will print out the graph6 strings of the graphs and the chromatic number of the coloring for this graph.

### Manual usage

If one wants to manually enter a graph into the program, one can do so by using the flag _-m_, followed by the graph6 string.

Example usage:
```
java Main -m "I|fIJcVFG"
java Main -m "I|fYJCv?w" -c "odd"
```




