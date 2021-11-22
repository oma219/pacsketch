#!/bin/bash

#####################################################################################
#
# jaccard_exp.sh -  Run the experiment to test the jaccard estimates for 
#                   both the MinHash and HyperLogLog data-structure
#
#                   Usage: ./jaccard_exp.sh <output_dir>
#
# Author: Omar Ahmed
# Date: Nov. 20, 2021
#
#####################################################################################

printf "[LOG] jaccard_exp.sh has started!\n"

## Load definitions, and make sure paths are valid
pair_generator="../build/generate_pair"
pacsketch="../build/pacsketch"

if [ ! -f $pair_generator ] || [ ! -f $pacsketch ]; then
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

## Generate various data-sets to use for jaccard test
num_datasets=500
for i in $(seq 1 1 $num_datasets)
do  
    length=$(shuf -i 1000000-12000000 -n 1)
    $pair_generator -k 11 -l $length -o "${output_dir}set${i}" 
done
printf "[LOG] Finished generating various datasets to use for tests.\n"


## Initialize the HLL output file
output_file="${output_dir}total_hll_summary.txt"
printf "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n" "b" "dataset_num" "true_card_a" "true_card_b" "true_card_union" "true_jaccard" \
                                         "est_card_a" "est_card_b" "est_card_union" "est_jaccard" > $output_file

## Iterate through each dataset, and get estimated values
num_datasets=500
for bits in 5 7 9 11
do
    for i in $(seq 1 $num_datasets)
    do
        output=$($pacsketch dist -i "${output_dir}set${i}_1.fa" \
                        -i "${output_dir}set${i}_2.fa" -H -b $bits -f)

        # Grab the estimated values ...
        est_card_a=$(awk '{print $(NF-3)}' <<< $output)
        est_card_b=$(awk '{print $(NF-2)}' <<< $output)
        est_card_union=$(awk '{print $(NF-1)}' <<< $output)
        est_jaccard=$(awk '{print $(NF)}' <<< $output)

        # Grab the true values ...
        stat_file="${output_dir}set${i}_stats.txt"
        true_stats=$(awk 'NR>1' $stat_file)

        true_card_a=$(awk '{print $1}' <<< $true_stats)
        true_card_b=$(awk '{print $2}' <<< $true_stats)
        true_card_union=$(awk '{print $3}' <<< $true_stats)
        true_jaccard=$(awk '{print $4}' <<< $true_stats)
        
        # Print values to summary file ...
        printf "%d,%d,%s,%s,%s,%s,%s,%s,%s,%s\n"  $bits $i $true_card_a $true_card_b $true_card_union $true_jaccard \
                                               $est_card_a  $est_card_b  $est_card_union  $est_jaccard >> $output_file
    done
done
printf "[LOG] Finished saving results using HLL.\n"

## Initialize the MinHash output file
output_file="${output_dir}total_minhash_summary.txt"
printf "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n" "k" "dataset_num" "true_card_a" "true_card_b" "true_card_union" "true_jaccard" \
                                         "est_card_a" "est_card_b" "est_card_union" "est_jaccard" > $output_file

## Iterate through each dataset, and get estimated values
num_datasets=500
for k in 5 20 60 100 
do
    for i in $(seq 1 $num_datasets)
    do
        output=$($pacsketch dist -i "${output_dir}set${i}_1.fa" \
                        -i "${output_dir}set${i}_2.fa" -M -k $k -f)

        # Grab the estimated values ...
        est_card_a=$(awk '{print $(NF-3)}' <<< $output)
        est_card_b=$(awk '{print $(NF-2)}' <<< $output)
        est_card_union=$(awk '{print $(NF-1)}' <<< $output)
        est_jaccard=$(awk '{print $(NF)}' <<< $output)

        # Grab the true values ...
        stat_file="${output_dir}set${i}_stats.txt"
        true_stats=$(awk 'NR>1' $stat_file)

        true_card_a=$(awk '{print $1}' <<< $true_stats)
        true_card_b=$(awk '{print $2}' <<< $true_stats)
        true_card_union=$(awk '{print $3}' <<< $true_stats)
        true_jaccard=$(awk '{print $4}' <<< $true_stats)
        
        # Print values to summary file ...
        printf "%d,%d,%s,%s,%s,%s,%s,%s,%s,%s\n"  $k $i $true_card_a $true_card_b $true_card_union $true_jaccard \
                                                  $est_card_a  $est_card_b  $est_card_union  $est_jaccard >> $output_file
    done
done
printf "[LOG] Finished saving results using MinHash.\n"


## Convert HLL file into separate files for cardinality/jaccard results
for data_structure in "hll" "minhash"
do
    input_file="${output_dir}total_${data_structure}_summary.txt"

    output_file="${output_dir}total_${data_structure}_cardinality_results.csv"
    printf "k_or_b,dataset_num,true_card,est_card,percent_error\n" > $output_file

    awk -F ',' 'BEGIN{dataset_num=1}
                {if(NR>1) { 
                    percent_error_1=(($7-$3+0.0)/($3)) * 100.0;
                    percent_error_2=(($8-$4+0.0)/($4)) * 100.0;
                    printf("%s,%d,%s,%s,%.4f\n",$1,dataset_num,$3,$7,percent_error_1);
                    printf("%s,%d,%s,%s,%.4f\n",$1,(dataset_num+1),$4,$8,percent_error_2);
                    dataset_num += 2;
                    if (dataset_num>1000) {dataset_num = 1;}
                }
                }' $input_file >> $output_file

    output_file="${output_dir}total_${data_structure}_jaccard_results.csv"
    printf "k_or_b,dataset_num,true_jaccard,est_jaccard,jaccard_error\n" > $output_file

    awk -F ',' '{if(NR>1) { 
                    jaccard_diff=($10-$6);
                    printf("%s,%d,%s,%s,%.4f\n",$1,$2,$6,$10,jaccard_diff);
                }
                }' $input_file >> $output_file
done


