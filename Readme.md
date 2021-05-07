# To compile please run below commands:
make

# TO run the file please run below command:
./tpga [inputfile] [-s outputfile] [-p petrinetfile]
inputfile is the input file describing the task precedence graph.  
If absent,tpga reads the task precedence graph from standard input.
[-s outputfile]is an optional file to output the state space information.  
If absent,tpgadoes not perform state space analysis (thus does not output any state space information).
[-p petrinetfile]is an optional file to output a Petri net equivalent to the task precedencegraph, in SmArT format.  
If absent,tpga does not perform the translation to a Petri net(thus does not generate any Petri net information).

# input file description
    All of lines are connected edges


