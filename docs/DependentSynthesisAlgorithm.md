In order to synthesis a strategy for dependent variable we apply the following algorithm:

## Aiger for Next States Subset
**Input:**
- LTL specification $\varphi(I, O \setminus D, D)$, where:
    - $I$ = Input vars
    - $O$ = Output vars
    - $D$ = Dependent Output Vars.
- $A_\varphi'$ - NBA with projected dependent variables.

**Output:**
- AIGER
    - Input = $I \cup O \setminus D$
    - Latches = $\#States(A_\varphi')$
    - Outputs = $\emptyset$

**Algorithm:**
```
aig = new AIG(
    Latches=#States(Aϕ),
    Input=I ∪ O \ D,
    Output=Ø
)

# Next latch assignment is OR on all gates in list
next_latech_to_gate = HashMap<latech_num, vector<var_num>>
bdd_to_gate = HashMap<BDD, var_num>

for state in States(Aϕ):
    for out in outs(state):
        src, bdd, dst = out

        if not (bdd ∈ bdd_to_gate):
            bdd_to_gate[bdd] = aig.bddToGate( bdd )

        next_latech_to_gate[dst].add( bdd_to_gate[bdd] )

for latch, gate_nums ∈ next_latech_to_gate:
    # Possible optimizations: cache the gate_nums vector and use the same gate number if already created
    aig.set_latch_next(
        aig.OR(gate_nums)
    )
```
