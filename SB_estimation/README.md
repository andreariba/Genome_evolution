This section includes the code to fit the tail of a distribution of distances with a Stick-Breaking with examples from the Alu distances in the directory ./sample/.

### Installing

To compile the program you have to run the following commands:

```

cd SB_estimation

g++ -O4 -o SB_estimation.exe SB_estimation.cpp

```

### Running

Run the simulation for a specific set of distances typing the following command

```

./SB_estimation.exe <distance file> <cut-off lenght>

```

### Output

1. number of breaks over lenght ratio 
2. distance at which the tail looks like a Stick-Breaking
3. fraction of segments that follow the Stick-Breaking

