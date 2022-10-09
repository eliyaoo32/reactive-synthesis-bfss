# BFSS - React Synthesis 

## Preliminaries
* The benchmarks from [SYNTCOMP](https://github.com/SYNTCOMP/benchmarks/tree/master), we use the [TLSF format](https://arxiv.org/pdf/1604.02284.pdf).
* The benchmarks are converted to the `LTL3BA` (a format of LTL formulas) via [SYFCO](https://github.com/reactive-systems/syfco).
* The platform for LTL and ω-automata manipulation is [Spot](https://spot.lrde.epita.fr/).

## Research: Dependent variables in benchmarks
* For each AP in the benchmark, we check if it's dependent on all other variables.
  * If variables are dependent on a subset then they depend on the set itself.
* For each benchmark we limit the execution time to 40 minutes.
* The CLI tool to search for dependent variables in a benchmark is implemented in the file [find_dependencies.cpp](src/find_dependencies.cpp).


* _Optional:_ we can keep search for all dependent variables with the following algorithm,
where `d` is the found dependent variable and `Variables` is the set of all the variables.
```
Unk = Variables - d     /* Unknown assignment for dependent or dependency */
Dependents = { d }      /* Set of all the dependent */
Dependencies = { }      /* Set of all the dependencies */

while Unk is not empty:
     z = Unk.pop()      /* Pop variable by some heuristc */
     if z is dependent on (Unk ∪ Dependencies):
        Dependents.add(z)
    else:
        Dependencies.add(z)

// the Dependents set is dependent on the set Dependencies
```

### Metrics
- [x] Can spot construct the automaton of the benchmark? If yes, how long it took?
- [x] Did any dependent variable was found? If yes, which one.
- [x] Which variables was checked?
- [x] How long it took for each variable to check if it's dependent.
- [x] Total duration of the process
- [x] Was the process finished on time, or it had a timeout? In case of timeout, at what variable checking was stopped.
