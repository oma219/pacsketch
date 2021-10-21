#!/bin/bash

#####################################################################################
#
# cardinality_exp.sh -  Run the experiment to test the cardinality estimates for 
#                       both the MinHash and HyperLogLog data-structure
#
#                       Usage: ./cardinality_exp.sh <output_dir>
#
# Author: Omar Ahmed
# Date: Oct. 20, 2021
#
#####################################################################################

printf "[LOG] cardinality_exp.sh has started!\n"

## Load definitions, and make sure paths are valid
fasta_generator="../build/generate_fasta"
pacsketch="../build/pacsketch"

if [ ! -f $fasta_generator ] || [ ! -f $pacsketch ]; then
    printf "Error: Executables cannot be found, make sure they are built.\n"
    exit 1
fi

## Grab command-line argument for output directory
output_dir="$1"

if [ ! -d $output_dir ]; then
    printf "Error: ${output_dir} is not a valid directory.\n"
    exit 1
fi

if [ "${output_dir: -1}" != "/" ]; then
  output_dir="${output_dir}/"
fi

num_datasets=1000

## Generate various data-sets to use for cardinality test
rm "${output_dir}true_cardinalities_all_datasets.txt"
for i in $(seq 1 1 $num_datasets)
do  
    printf "${i} " >> "${output_dir}true_cardinalities_all_datasets.txt"
    $fasta_generator -k 11 -n 1 -l 1000000 > "${output_dir}dataset_${i}.fa" 2>> "${output_dir}true_cardinalities_all_datasets.txt"
done
printf "[LOG] Finished generating various datasets to use for tests.\n"

## Compute cardinalities when using MinHash
rm "${output_dir}est_cardinalities_all_datasets_with_minhash.txt"
for i in $(seq 1 1 $num_datasets)
do
    for k in 1 10 100 400
    do
        input_file="${output_dir}dataset_${i}.fa"
        output=$($pacsketch build -i $input_file -f -M -c -k $k)
        printf "${i} minhash_k${k} ${output}\n" >> "${output_dir}est_cardinalities_all_datasets_with_minhash.txt"
    done
done
printf "[LOG] Finished generating cardinalities for all datasets using various MinHash settings.\n"

## Compute cardinalities when using HLL
rm "${output_dir}est_cardinalities_all_datasets_with_hll.txt"
for i in $(seq 1 1 $num_datasets)
do
    for b in 3 6 9 12 
    do
        input_file="${output_dir}dataset_${i}.fa"
        output=$($pacsketch build -i $input_file -f -H -c -b $b)
        printf "${i} hll_b${b} ${output}\n" >> "${output_dir}est_cardinalities_all_datasets_with_hll.txt"
    done
done
printf "[LOG] Finished generating cardinalities for all datasets using various HLL settings.\n"


true_card_file="${output_dir}true_cardinalities_all_datasets.txt"
est_card_file1="${output_dir}est_cardinalities_all_datasets_with_minhash.txt"
est_card_file2="${output_dir}est_cardinalities_all_datasets_with_hll.txt"

# Use join to combine these files
join -j 1 -o 1.1,1.4,2.2,2.4  $true_card_file $est_card_file1 | awk '{printf("%s,%s,%s,%s\n", $1, $2, $3, $4)}' > "${output_dir}temp_output_file1.csv"
join -j 1 -o 1.1,1.4,2.2,2.4  $true_card_file $est_card_file2 | awk '{printf("%s,%s,%s,%s\n", $1, $2, $3, $4)}' > "${output_dir}temp_output_file2.csv"
cat "${output_dir}temp_output_file1.csv" "${output_dir}temp_output_file2.csv" > "${output_dir}output_file.csv"
rm "${output_dir}temp_output_file1.csv" "${output_dir}temp_output_file2.csv"

# Add the error rate to data-file
awk -F ',' 'BEGIN {error=0.0} {error=(($4-$2)/$2 * 100); printf("%s,%s,%s,%s,%f\n",$1,$2,$3,$4,error);}' "${output_dir}output_file.csv" > "${output_dir}final_output_file.csv"
rm "${output_dir}output_file.csv" 

printf "[LOG] Finished analyzing the results.\n"