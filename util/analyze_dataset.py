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

def analyze_kdd(input_file, output_prefix):
    """ Analyzes the KDDCup99 and NSL-KDD datasets """
    input_fd = open(input_file, "r")
    lines = input_fd.readlines()
    input_fd.close()

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
    feature_names = grab_kdd_feature_list()
    numeric_feature_indexes = grab_numeric_kdd_features()

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

    # Phase 3: Re-writing csv file to replace labels with either 'normal' or 'attack'
    feature_names = grab_kdd_feature_list()
    numeric_feature_indexes = grab_numeric_kdd_features()

    output_fd = open(output_prefix + "_alternate_dataset.csv", "w")
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

    






def main(args):
    """ main method that executes the analysis of the requested dataset """
    if (args.kdd):
        analyze_kdd(args.input_file, args.output_prefix)

def parse_arguments():
    """ Parse arguments from command-line and returns it """
    parser = argparse.ArgumentParser(description='Parses network datasets to be used for classification/plotting.')
    parser.add_argument("-i", dest="input_file", help="path to input dataset", required=True)
    parser.add_argument("--kdd", action="store_true", dest="kdd", help="indicates the input dataset is either the kdd99 or nsl-kdd dataset.", default=False)
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

if __name__ == "__main__":
    args = parse_arguments()
    check_arguments(args)
    main(args)
