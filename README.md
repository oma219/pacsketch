## Pacsketch - Network Anomaly Detection

Pacsketch is a software package that uses probabilistic sketch data-structures to determine if packet traces contain anomalous behavior.


## Utility Programs

The first program that is included in this repo as a utility is *generate_fasta* which just generates a FASTA file which the file format for DNA sequences. It was used as an approach to generate basic test data to test out the implementation of the data structures. The usage of the program is shown below, where it generates one sequence with a length of 30,000 bp. It also outputs the cardinality of the sequence in terms of 31-mers.

```sh
generate_fasta -k 31 -n 1 -l 30000
```