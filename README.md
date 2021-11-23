# Pacsketch :running_man:

Pacsketch is a software tool that uses probabilistic sketch data-structures to determine if packet traces contain anomalous behavior. The data-structures that are available to be used are MinHash and HyperLogLog. The key idea of this approach is that if we build a sketch over the incoming network data (packet header/connection data) then we can compare it with existing sketches to see what traffic pattern it is most similar to in order to identify anomalous sets of data.

# Build
In order to build pacsketch, clone the repository and use cmake.

```sh
git clone --recursive https://github.com/oma219/pacsketch.git
cd pacsketch 

mkdir build && cd build
cmake ..
make install
```
The submodule can be updated by using the following command: `git submodule update --init --recursive`

# Utility Programs

***generate_fasta***

This utility program just generates a FASTA file which the file format for DNA sequences. It was used as an approach to generate basic test data to test out the implementation of the data structures. The usage of the program is shown below, where it generates one sequence with a length of 30,000 bp. It also outputs the cardinality of the sequence in terms of 31-mers.

```sh
./generate_fasta -k 31 -n 1 -l 30000
```

***generate_pair***

This utility program generates a pair of FASTA files, as well as outputting the exact cardinality for each file for a certain kmer length. In addition, it outputs the exact jaccard similarity between the two FASTA files. The usage of the program is shown below, where this command will generate two FASTA files which are each 1,000,000 bp long along with the exact cardinality of the multi-set of 31-mers in each file. These cardinalities along with the exact jaccard similarity will be stored in a file with the suffix "stats.txt". All of these files will be stores using the provided output file prefix with -o option.

```sh
./generate_pair -k 31 -l 1000000 -o /Users/output_dir/prefix
```

*** ***

# Analysis Scripts

***Experiment 1:** Test the Cardinality and Jaccard Estimates of Each Data-Structure*

This experiment will generate numerous sets of data in pairs with known jaccard similarity, and then will try to compute the cardinality and jaccard for each set of data. This experiment can be run using the command shown below, the only parameter that needs to be provided is the output directory for all the datasets and results. Then after running this command, using the `jaccard_exp_analysis.R` can be used to generate the plots for this experiment.

```sh
bash jaccard_exp.sh <output_dir>
```

***Experiment 2:** Comparing NSL-KDD features across normal and attack records*

This experiment was focused on better understand the features available in the NSL-KDD dataset, and visually comparing their distributions to ensure they are different (since we want to use them for classification). The first step was to parse the dataset using the `analysis_dataset.py` script in the `util/` folder. This script will generate various files that include data such as the class breakdown, mean/standard deviation of each feature across normal or attack records, as well as modified csv file to use for the next step. 

The following command takes in the csv file produced by the python script to generate a couple of violin plots comparing features across the normal and attack connections.

```sh
python3 analyze_dataset.py -i /path/to/KDDTrain+.txt --kdd -o /path/to/output/nsl_kdd
# Run kdd_feature_analysis.R in the exp/ folder
```
