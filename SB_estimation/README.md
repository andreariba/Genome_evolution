### Installing

To compile the program you have to run the following commands:

```

git clone git@github.com:andreariba/Genome_evolution.git

cd Genome_evolution/SB_estimation

g++ -O4 -o SB_estimation.exe SB_estimation.cpp

```

## Running

Run the simulation for a specific ORF typing the following command

```

./SB_estimation.exe <distance file> <cut-off lenght>

```
Arguments:

* -c: tsv file containing an elongation rate for each codon
* -f: double multiplying the codon speeds in the input file in c
* -n: name of the output file
* -s: ORF sequence to simulate

the output includes a file \"\<ORFname\>.dat\" containing six columns:

1. factor multiplying the codon speeds (argument -f)
2. initiation rate
3. protein synthesis rate
4. ribosome density
5. fraction of queuing events
6. vector of ribosome density per codon

The file examples.txt includes the list of commands to simulate the model on the yeast translatome

