#!/usr/bin/env python3

######################################################################
#
# Name: analyze_dataset.py
# Description: This file will parse the network datasets, and analyze
#              them as well as generating alternate versions of the 
#              data to be used for plotting, and classification.
# Project: This file is part of the pacsketch repo.
#
# Author: Omar Ahmed
# Date: November 22, 2021
#
######################################################################

import argparse
import statistics
import os.path

def grab_kdd_feature_list():
    """ Lists the features available in KDD dataset """
    feature_list = ["duration", "protocol_type", "service", "flag", "src_bytes", "dst_bytes",
                    "land", "wrong_fragments", "urgent", "hot", "num_failed_logins", "logged_in",
                    "num_compromised",  "root_shell", "su_attempted", "num_root", "num_file_creations", "num_shells",
                    "num_access_files", "num_outbound_cmds", "is_host_login", "is_guest_login", "count", "srv_count",
                    "serror_rate", "srv_serror_rate", "rerror_rate", "srv_rerror_rate", "same_srv_rate", "diff_srv_rate",
                    "srv_diff_host_rate", "dst_host_count", "dst_host_srv_count", "dst_host_same_srv_rate", "dst_host_diff_srv_rate", "dst_host_same_src_port_rate",
                    "dst_host_srv_diff_host_rate", "dst_host_serror_rate", "dst_host_srv_serror_rate", "dst_host_rerror_rate", "dst_host_srv_rerror_rate"]
    return feature_list

def grab_numeric_kdd_features():
    """ Stores which features in KDD dataset that are features """
    numeric_list = [0, 4, 5, 7, 8, 9, 10, 
                    12, 13, 14, 15, 16, 17, 18,
                    19, 22, 23, 24, 25, 26, 27,
                    28, 29, 30, 31, 32, 33, 34,
                    35, 36, 37, 38, 39, 40]
    return numeric_list

def convert_real_to_discrete_label(value, mean, stdev):
    """ 
    Converts a real value to a discrete value.
    
    There are four possible labels: 1, 2, 3, and 4. They correspond
    to distances from the mean of the feature. So 2 and 3 are within
    one standard deviation up or below. While 1 and 4 are below or above
    the standard deviation mark.
    """
    if stdev == 0:
        return 0

    z_score = (value-mean)/(stdev)
    if z_score < -1.0:
        return 1
    elif z_score >= -1.0 and z_score < -0.5:
        return 2
    elif z_score >= -0.5 and z_score < -0.25:
        return 3
    elif z_score >= -0.25 and z_score < 0.0:
        return 4
    elif z_score >= 0.0 and z_score < 0.25:
        return 5
    elif z_score >= 0.25 and z_score < 0.5:
        return 6
    elif z_score >= 0.5 and z_score < 1.0:
        return 7
    else: 
        return 8

def analyze_kdd(input_file, output_prefix, test_file):
    """ Analyzes the KDDCup99 and NSL-KDD datasets """
    attack_records = []
    normal_records = []
    test_records = []

    # Read in dataset from training and test set, if provided
    with open(input_file, "r") as input_fd:
        lines = input_fd.readlines()
    
    if test_file is not "":
        with open(test_file, "r") as test_fd:
            test_records = test_fd.readlines()

    feature_names = grab_kdd_feature_list()
    numeric_feature_indexes = grab_numeric_kdd_features()

    attack_records = []
    normal_records = []

    # Phase 1: Analyzes the types of record labels in dataset
    label_set = set()
    label_dict = {}
    for line in lines:
        variables = line.split(",")
        record_type = variables[41]
        normal_records.append(line) if record_type == "normal" else attack_records.append(line)

        if record_type not in label_set:
            label_set.add(record_type)
            label_dict[record_type] = 1
        else:
            label_dict[record_type] += 1
    
    # Writes out the number of items in each class to text file
    output_fd = open(output_prefix + "_class_types.txt", "w")
    output_fd.write("{:20}{:30s}\n".format("Class Name:", "Number of Records:"))

    for key in label_dict:
        output_fd.write("{:20}{:<30d}\n".format(key, label_dict[key]))
    output_fd.close()

    # Phase 2: Compute the statistical parameters for each numerical feature
    numeric_params_list = []
    output_fd = open(output_prefix + "_feature_analysis.txt", "w")
    output_fd.write("{:30s}{:15s}{:15s}{:15s}{:20s}{:20s}\n".format("Feature Name:", "Record Type:", "Mean:", "Std Dev:", "Non-zero Mean:", "Non-zero Std Dev:"))

    for feature_index in numeric_feature_indexes:
        curr_name = feature_names[feature_index]
        curr_distribution = []

        # First analyze all this feature in normal records ...
        for record in normal_records:
            variables = record.split(",")
            curr_distribution.append(float(variables[feature_index]))
        curr_distribution_non_zero = [x for x in curr_distribution if x > 0.0]

        mean = sum(curr_distribution)/len(curr_distribution)
        stdev = statistics.pstdev(curr_distribution)

        if len(curr_distribution_non_zero) > 0:
            mean_nz = sum(curr_distribution_non_zero)/len(curr_distribution_non_zero)
            stdev_nz = statistics.pstdev(curr_distribution_non_zero)
        else:
            mean_nz = 0
            stdev_nz = 0
        output_fd.write("{:30s}{:15s}{:<15.4f}{:<15.4f}{:<20.4f}{:<20.4f}\n".format(curr_name, "normal", mean, stdev, mean_nz, stdev_nz))
        numeric_params_list.append([mean, stdev])

        # Next, analyze this features for attack records ...
        curr_distribution = []
        for record in attack_records:
            variables = record.split(",")
            curr_distribution.append(float(variables[feature_index]))
        curr_distribution_non_zero = [x for x in curr_distribution if x > 0.0]

        mean = sum(curr_distribution)/len(curr_distribution)
        stdev = statistics.pstdev(curr_distribution)
        
        if len(curr_distribution_non_zero) > 0:
            mean_nz = sum(curr_distribution_non_zero)/len(curr_distribution_non_zero)
            stdev_nz = statistics.pstdev(curr_distribution_non_zero)
        else:
            mean_nz = 0
            stdev_nz = 0
        output_fd.write("{:30s}{:15s}{:<15.4f}{:<15.4f}{:<20.4f}{:<20.4f}\n".format(curr_name, "attack", mean, stdev, mean_nz, stdev_nz))
    output_fd.close()

    # Phase 3: Re-writing csv file to replace labels with either 'normal' or 'attack' (for plotting)
    output_fd = open(output_prefix + "_dataset_for_plotting.csv", "w")
    output_fd.write("feature_name,value,label\n")

    for record in lines:
        variables = record.split(",")

        # Replace specific attack labels with 'attack'
        if variables[41] != "normal":
            variables[41] = "attack" 
        variables = variables[:42] # Removes last element

        for curr_index in numeric_feature_indexes:
            curr_feature = feature_names[curr_index]
            output_fd.write(",".join([curr_feature, variables[curr_index], variables[41]]) + "\n")
    output_fd.close()

    # Phase 4: Convert dataset to all discrete features using stdev values
    output_fd = open(output_prefix + "_converted_dataset.csv", "w")

    normal_fd = open(output_prefix + "_converted_normal_dataset.csv", "w")
    attack_fd = open(output_prefix + "_converted_attack_dataset.csv", "w")
    test_fd = open(output_prefix + "_converted_test_dataset.csv", "w")

    for record in lines:
        variables = record.split(",")

        # Go through each numeric feature and replace with discrete label ...
        for i, curr_index in enumerate(numeric_feature_indexes):
            curr_value = float(variables[curr_index])
            curr_mean, curr_stdev = numeric_params_list[i]

            corresponding_label = convert_real_to_discrete_label(curr_value, curr_mean, curr_stdev)
            variables[curr_index] = str(corresponding_label)
        output_fd.write(",".join(variables[:42]) + "\n")

        if variables[41].strip() == "normal":
            normal_fd.write(",".join(variables[:42]) + "\n")
        else:
            attack_fd.write(",".join(variables[:42]) + "\n")
    output_fd.close()
    normal_fd.close()
    attack_fd.close()

    # Write out converted test dataset if provided
    if len(test_records) > 0:
        with open(output_prefix + "_converted_test_dataset.csv", "w") as test_fd:
            for record in test_records:
                variables = record.split(",")

                # Go through each numeric feature and replace with discrete label ...
                for i, curr_index in enumerate(numeric_feature_indexes):
                    curr_value = float(variables[curr_index])
                    curr_mean, curr_stdev = numeric_params_list[i]

                    corresponding_label = convert_real_to_discrete_label(curr_value, curr_mean, curr_stdev)
                    variables[curr_index] = str(corresponding_label)
                test_fd.write(",".join(variables[:42]) + "\n")

    # Phase 5: Analyze the new converted, records and compare across normal and attack records ...
    input_dataset = open(output_prefix + "_converted_dataset.csv", "r")
    converted_records = input_dataset.readlines()
    input_dataset.close()

    normal_converted_records = []
    attack_converted_records = []
    
    # First, separate records into types ...
    for record in converted_records:
        variables = record.split(",")
        record_type = variables[-1].strip()

        if record_type == "normal":
            normal_converted_records.append(record)
        else:
            attack_converted_records.append(record)

    # Now, go through each feature and compare between normal/attack
    output_fd = open(output_prefix + "_converted_dataset_feature_analysis.txt", "w")
    output_fd.write("{:30s}{:15s}{:15s}{:15s}{:15s}{:15s}{:15s}{:15s}\n".format("Feature Name:", "Record Type:", "Label 1:", "Label 2:", "Label 3:", 
                                                                                "Label 4:","Label 5:", "Label 6:", "Label 7:", "Label 8:" ))

    for feature_index in numeric_feature_indexes:
        curr_name = feature_names[feature_index]
        label_counts = [0, 0, 0, 0, 0, 0, 0, 0]

        for record in normal_converted_records:
            curr_label = int(record.split(",")[feature_index])
            label_counts[curr_label-1] += 1
        output_fd.write("{:30s}{:15s}{:<15d}{:<15d}{:<15d}{:<15d}{:<15d}{:<15d}{:<15d}{:<15d}\n".format(curr_name, "normal", label_counts[0], label_counts[1], label_counts[2], 
                                                                                                        label_counts[3], label_counts[4], label_counts[5], label_counts[6], label_counts[7]))

        label_counts = [0, 0, 0, 0, 0, 0, 0, 0]
        for record in attack_converted_records:
            curr_label = int(record.split(",")[feature_index])
            label_counts[curr_label-1] += 1
        output_fd.write("{:30s}{:15s}{:<15d}{:<15d}{:<15d}{:<15d}{:<15d}{:<15d}{:<15d}{:<15d}\n".format(curr_name, "attack", label_counts[0], label_counts[1], label_counts[2], 
                                                                                                        label_counts[3], label_counts[4], label_counts[5], label_counts[6], label_counts[7]))    
    output_fd.close()


def main(args):
    """ main method that executes the analysis of the requested dataset """
    if (args.kdd):
        analyze_kdd(args.input_file, args.output_prefix, args.test_file)

def parse_arguments():
    """ Parse arguments from command-line and returns it """
    parser = argparse.ArgumentParser(description='Parses network datasets to be used for classification/plotting.')
    parser.add_argument("-i", dest="input_file", help="path to input dataset", required=True)
    parser.add_argument("--kdd", action="store_true", dest="kdd", help="indicates the input dataset is either the kdd99 or nsl-kdd dataset.", default=False)
    parser.add_argument("-t", dest="test_file", help="path to test dataset to convert all the numerical features.", default="")
    parser.add_argument("-o", dest="output_prefix", help="output prefix for analysis files.", required=True)
    args=parser.parse_args()
    return args

def check_arguments(args):
    """ Checks arguments to make sure they are valid before moving forward """
    num_types = int(args.kdd)
    if num_types != 1:
        print("Error: Exactly one type of dataset needs to be specified.")
        exit(1)
    if not os.path.exists(args.input_file):
        print("Error: The dataset path is not valid.")
        exit(1)
    if not os.path.isdir(os.path.dirname(args.output_prefix)):
        print("Error: The provided output-prefix path is not valid.")
        exit(1)
    if not os.path.exists(args.test_file):
        print("Error: path to test file is not valid.")
        exit(1)

if __name__ == "__main__":
    args = parse_arguments()
    check_arguments(args)
    main(args)
