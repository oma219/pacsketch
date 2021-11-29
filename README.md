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

# Use

Pacsketch can be used through one of its sub-commands which include: `build`, `dist`, and `simulate`.

* `build` - takes in an input dataset, and can build either the HyperLogLog or MinHash sketch and output the estimated cardinality.
* `dist` - takes in two input datasets, builds the sketches, and outputs the jaccard similarity between the two sketches
* `simulate` - takes in an training and test set, simulates windows of records, and computes jaccard with respect to reference sketches. 

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

***analyze_dataset.py***

This utility program both analyzes the KDD-Cup/NSL-KDD dataset as well as preprocesses the dataset in order to convert all the real features into discrete features. This is a **necessary** step prior to building or comparing sketches involving this network datasets.

This command will generate a series of output files using the provided output prefix (it is a good idea to make a specialized folder). The most important output are the `*converted_*_dataset.csv` files which essentially contain the NSL-KDD dataset but in the form where the features are converted to discrete features.

```sh
python3 analyze_dataset.py -i KDDTrain+.txt --kdd -o /path/to/updated_dataset/nsl_kdd -t KDDTest+.txt
```

# Analysis Scripts

***Experiment 1:** Test the Cardinality and Jaccard Estimates of Each Data-Structure*

This experiment will generate numerous sets of data in pairs with known jaccard similarity, and then will try to compute the cardinality and jaccard for each set of data. This experiment can be run using the command shown below, the only parameter that needs to be provided is the output directory for all the datasets and results. Then after running this command, using the `jaccard_exp_analysis.R` can be used to generate the plots for this experiment.

```sh
bash jaccard_exp.sh <output_dir>
# Run jaccard_exp_analysis.R in the exp/ folder
```

***Experiment 2:** Comparing NSL-KDD features across normal and attack records*

This experiment was focused on better understand the features available in the NSL-KDD dataset, and visually comparing their distributions to ensure they are different (since we want to use them for classification). The first step was to parse the dataset using the `analysis_dataset.py` script in the `util/` folder. This script will generate various files that include data such as the class breakdown, mean/standard deviation of each feature across normal or attack records, as well as modified csv file to use for the next step. 

The following command takes in the csv file produced by the python script to generate a couple of violin plots comparing features across the normal and attack connections.

```sh
python3 analyze_dataset.py -i /path/to/KDDTrain+.txt --kdd -o /path/to/output/nsl_kdd -t /path/to/KDDTest+.txt
# Run kdd_feature_analysis.R in the exp/ folder
```

***Experiment 3:** Comparing Jaccard Distributions of Test Windows Against Reference Sketches*

In this experiment, my goal was to better understand how the jaccard distribution with respect to the normal and attack references would change as the percent of anomalous records in the window is varied. In order to do this, in the main `pacsketch` tool I added a sub-command called `simulate` that will simulate windows of connections records with certain percentages of anomalous records and then computes the jaccard with respect to the two reference sketches. 

An example command is shown below that generates 1000 test windows each containing 10,000 records stored in it where 40% of each window is composed of attack records.

Each of the input `*.csv` files are produced by using the `analyze_dataset.py` utility program. This is important because it discretizes the input features prior inputing them into a sketch.

```sh
./pacsketch simulate -i normal_dataset.csv -i attack_dataset.csv -M -k 100 -n 10000 -w 1000 -a 0.4 > output_data.csv
# Run simulation_analysis.R in exp/ folder
```