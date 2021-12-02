/*
 * Name: pacsketch.cpp
 * Description: Main file for pacsketch project.
 * Project: This file is part of pacsketch repo.
 * 
 * Author: Omar Ahmed
 * Date: October 17, 2021
 */

#include <iostream>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <pacsketch.h>
#include <hash.h>
#include <minhash.h>
#include <hll.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <numeric>
#include <random>
#include <tuple>
#include <array>

bool is_file(const char* file_path) {
    /* Checks if the path is a valid file-path */
    std::ifstream test_file(file_path, std::ifstream::in);
    if (test_file.fail()) {return 0;}
    
    test_file.close();
    return 1;   
}

int pacsketch_usage() {
    /* Prints out the usage for pacsketch (if no sub-command, or incorrect sub-command is used) */
    std::fprintf(stderr, "\npacsketch has different sub-commands to run which can be used as follows:\n");
    std::fprintf(stderr, "Usage: pacsketch <command> [options]\n\n");

    std::fprintf(stderr, "Commands:\n");
    std::fprintf(stderr, "\t%-12sbuilds the sketches for packet traces (different sketches can be used)\n", "build");
    std::fprintf(stderr, "\t%-12scompares sketches and computes similarity measures between them\n", "dist");
    std::fprintf(stderr, "\t%-12ssimulate windows of packets from two sources and compare them\n\n", "simulate");
    return 1;
}

int pacsketch_build_usage() {
    /* Prints out the usage information for pacsketch build sub-command */
    std::fprintf(stderr, "\npacsketch build - builds the skeches for a given input dataset.\n");
    std::fprintf(stderr, "Usage: pacsketch build [options]\n\n");

    std::fprintf(stderr, "Options:\n");
    std::fprintf(stderr, "\t%-10sprints this usage message\n", "-h");
    std::fprintf(stderr, "\t%-10spath to input file that has index built for it\n", "-i [FILE]");
    std::fprintf(stderr, "\t%-10sinput data is in FASTA format (used for dev)\n", "-f");
    std::fprintf(stderr, "\t%-10sbuild a MinHash sketch from input data\n", "-M");
    std::fprintf(stderr, "\t%-10sbuild a HyperLogLog sketch from input data\n", "-H");
    std::fprintf(stderr, "\t%-10soutput the cardinality of the sketch after building\n\n", "-c");

    std::fprintf(stderr, "MinHash specific options:\n");
    std::fprintf(stderr, "\t%-10snumber of hashes to keep in sketch\n\n", "-k [arg]");

    std::fprintf(stderr, "HyperLogLog specific options:\n");
    std::fprintf(stderr, "\t%-10snumber of bits to use for choosing registers\n\n", "-b [arg]");
    return 1;
}

int pacsketch_dist_usage() {
    /* Prints out the usage information for pacsketch build sub-command */
    std::fprintf(stderr, "\npacsketch dist - computes the jaccard between two sketches.\n");
    std::fprintf(stderr, "Usage: pacsketch dist -i file1 -i file2 [options]\n\n");

    std::fprintf(stderr, "Options:\n");
    std::fprintf(stderr, "\t%-10sprints this usage message\n", "-h");
    std::fprintf(stderr, "\t%-10spath to input file that has index built for it\n", "-i [FILE]");
    std::fprintf(stderr, "\t%-10sinput data is in FASTA format (used for dev)\n", "-f");
    std::fprintf(stderr, "\t%-10sbuild a MinHash sketch from input data\n", "-M");
    std::fprintf(stderr, "\t%-10sbuild a HyperLogLog sketch from input data\n\n", "-H");

    std::fprintf(stderr, "MinHash specific options:\n");
    std::fprintf(stderr, "\t%-10snumber of hashes to keep in sketch\n\n", "-k [arg]");

    std::fprintf(stderr, "HyperLogLog specific options:\n");
    std::fprintf(stderr, "\t%-10snumber of bits to use for choosing registers\n\n", "-b [arg]");
    return 1;
}

int pacsketch_simulate_usage() {
    /* Prints out the usage information for pacsketch simulate sub-command */
    std::fprintf(stderr, "\npacsketch simulate - simulates windows of packets and compare them with jaccard.\n");
    std::fprintf(stderr, "\nNOTE: An IMPORTANT assumption made is that the first file provided on the command-line\n");
    std::fprintf(stderr, "is the normal records file, and the second one is the attack records file.\n");
    std::fprintf(stderr, "\nUsage: pacsketch dist -i file1 -i file2 [options]\n\n");

    std::fprintf(stderr, "Options:\n");
    std::fprintf(stderr, "\t%-10sprints this usage message\n", "-h");
    std::fprintf(stderr, "\t%-10spath to input file that has index built for it\n", "-i [FILE]");
    std::fprintf(stderr, "\t%-10sinput data is in FASTA format (used for dev)\n", "-f");
    std::fprintf(stderr, "\t%-10sbuild a MinHash sketch from input data\n", "-M");
    std::fprintf(stderr, "\t%-10sbuild a HyperLogLog sketch from input data\n", "-H");
    std::fprintf(stderr, "\t%-10snumber of records to include in time window\n", "-n [arg]");
    std::fprintf(stderr, "\t%-10snumber of windows to simulate\n", "-w [arg]");
    std::fprintf(stderr, "\t%-10sratio of simulated window that are attack records (0.0 <= x <= 1.0)\n", "-a [arg]");
    std::fprintf(stderr, "\t%-10spath to test dataset, if would like to simulate in test mode\n\n", "-t [arg]");

    std::fprintf(stderr, "MinHash specific options:\n");
    std::fprintf(stderr, "\t%-10snumber of hashes to keep in sketch\n\n", "-k [arg]");

    std::fprintf(stderr, "HyperLogLog specific options:\n");
    std::fprintf(stderr, "\t%-10snumber of bits to use for choosing registers\n\n", "-b [arg]");
    return 1;
}

void parse_build_options(int argc, char** argv, PacsketchBuildOptions* opts) {
    /* Parses the command-line options for build sub-command */
    for (int c; (c=getopt(argc, argv, "hi:fMHck:b:")) >= 0;) {
        switch (c) {
            case 'h': pacsketch_build_usage(); std::exit(1);
            case 'i': opts->input_file.assign(optarg); break;
            case 'f': opts->input_fasta = true; break;
            case 'M': opts->use_minhash = true; break;
            case 'H': opts->use_hll = true; break;
            case 'c': opts->print_cardinality = true; break;
            case 'k': opts->k_size = std::max(std::atoi(optarg), 0); break;
            case 'b': opts->bit_prefix = std::max(std::atoi(optarg), 0); break;
            default:  std::exit(1);
        }
    }
}

void parse_dist_options(int argc, char** argv, PacsketchDistOptions* opts) {
    /* Parses the command-line options for dist sub-command */
    for (int c; (c=getopt(argc, argv, "hi:fMHk:b:")) >= 0;) {
        switch (c) {
            case 'h': pacsketch_build_usage(); std::exit(1);
            case 'i': opts->input_files.push_back(optarg); break;
            case 'f': opts->input_fasta = true; break;
            case 'M': opts->use_minhash = true; break;
            case 'H': opts->use_hll = true; break;
            case 'k': opts->k_size = std::max(std::atoi(optarg), 0); break;
            case 'b': opts->bit_prefix = std::max(std::atoi(optarg), 0); break;
            default:  std::exit(1);
        }
    }
}

void parse_simulate_options(int argc, char** argv, PacsketchSimulateOptions* opts) {
    /* Parses the command-line options for simulate sub-command */
    for (int c; (c=getopt(argc, argv, "hi:fMHk:b:n:w:a:t:")) >= 0;) {
        switch (c) {
            case 'h': pacsketch_build_usage(); std::exit(1);
            case 'i': opts->input_files.push_back(optarg); break;
            case 'f': opts->input_fasta = true; break;
            case 'M': opts->use_minhash = true; break;
            case 'H': opts->use_hll = true; break;
            case 'k': opts->k_size = std::max(std::atoi(optarg), 0); break;
            case 'b': opts->bit_prefix = std::max(std::atoi(optarg), 0); break;
            case 'n': opts->num_records = std::max(0, std::atoi(optarg)); break;
            case 'w': opts->num_windows = std::max(0, std::atoi(optarg)); break;
            case 'a': opts->attack_percent = std::atof(optarg); break;
            case 't': opts->test_files.push_back(optarg); opts->test_mode = true; break;
            default:  std::exit(1);
        }
    }
}

int build_main(int argc, char** argv) {
    /* main method for build sub-command */
    if (argc == 1) {return pacsketch_build_usage();}

    // Grab the build options, and validate they are not missing/don't make sense 
    PacsketchBuildOptions build_opts;
    parse_build_options(argc, argv, &build_opts);
    build_opts.validate();

    // Build the sketch
    if (build_opts.curr_sketch == MINHASH) {
        MinHash data_sketch (build_opts.input_file, build_opts.k_size, build_opts.input_data_type);
        if (build_opts.print_cardinality) {
            std::fprintf(stdout, "Estimated_Cardinality: %lld\n", data_sketch.get_cardinality());
        }
    } else if (build_opts.curr_sketch == HLL) {
        HyperLogLog data_sketch (build_opts.input_file, build_opts.bit_prefix, build_opts.input_data_type);
        if (build_opts.print_cardinality) {
            std::fprintf(stdout, "Estimated_Cardinality: %lld\n", data_sketch.compute_cardinality());
        }
    }
    return 1;
}

int dist_main(int argc, char** argv) {
    /* main method for dist sub-command */
    if (argc == 1) {return pacsketch_dist_usage();}

    // Grab the dist options, and validate they are not missing/don't make sense 
    PacsketchDistOptions dist_opts;
    parse_dist_options(argc, argv, &dist_opts);
    dist_opts.validate();

    // Build the sketches for each input file
    if (dist_opts.curr_sketch == MINHASH) {
        MinHash data_sketch_1 (dist_opts.input_files[0], dist_opts.k_size, dist_opts.input_data_type);
        MinHash data_sketch_2 (dist_opts.input_files[1], dist_opts.k_size, dist_opts.input_data_type);

        uint64_t card_a = data_sketch_1.get_cardinality();
        uint64_t card_b = data_sketch_2.get_cardinality();
        auto jaccard = MinHash::compute_jaccard(data_sketch_1, data_sketch_2);

        std::cout << "Estimated values based on MinHash sketches ...\n";
        std::cout << std::right << std::setw(10) << "|SET(A)|" <<
                     std::right << std::setw(10) << "|SET(B)|" <<
                     std::right << std::setw(15) << "|SET(AUB)|"  <<
                     std::right << std::setw(10) << "J(A,B)" << std::endl;
        std::cout << std::right << std::setw(10) << card_a <<
                     std::right << std::setw(10) << card_b <<
                     std::right << std::setw(15) << "N/A" <<
                     std::right << std::setw(10) << std::setprecision(4) << jaccard << std::endl;

    } else if (dist_opts.curr_sketch == HLL) {
        HyperLogLog data_sketch_1 (dist_opts.input_files[0], dist_opts.bit_prefix, dist_opts.input_data_type);
        HyperLogLog data_sketch_2 (dist_opts.input_files[1], dist_opts.bit_prefix, dist_opts.input_data_type);

        uint64_t card_a = data_sketch_1.compute_cardinality();
        uint64_t card_b = data_sketch_2.compute_cardinality();

        // Generate union sketch, and estimate jaccard
        HyperLogLog union_sketch = data_sketch_1 + data_sketch_2;
        uint64_t card_union = union_sketch.compute_cardinality();

        auto jaccard = std::max(card_a + card_b - card_union + 0.0, 0.0)/(card_union);
        if (card_union > (card_a + card_b)) {jaccard = 0.0;} // Check for overflow

        std::cout << "Estimated values based on HyperLogLog sketches ...\n";
        std::cout << std::right << std::setw(10) << "|SET(A)|" <<
                     std::right << std::setw(10) << "|SET(B)|" <<
                     std::right << std::setw(15) << "|SET(AUB)|"  <<
                     std::right << std::setw(10) << "J(A,B)" << std::endl;
        std::cout << std::right << std::setw(10) << card_a <<
                     std::right << std::setw(10) << card_b <<
                     std::right << std::setw(15) << card_union <<
                     std::right << std::setw(10) << std::setprecision(4) << jaccard << std::endl;
    }

    
    return 1;
}

int simulate_main(int argc, char** argv) {
    /* main method for simulate sub-command */
    if (argc == 1) {return pacsketch_simulate_usage();}
    
    // Grab the simulate options, and validate they are not missing/don't make sense 
    PacsketchSimulateOptions sim_opts;
    parse_simulate_options(argc, argv, &sim_opts);
    sim_opts.validate();

    // Memory-map the two input files, and checks for errors
    size_t pagesize = getpagesize();
    size_t input_1_fd = open(sim_opts.input_files[0].data(), O_RDONLY);
    size_t input_2_fd = open(sim_opts.input_files[1].data(), O_RDONLY);

    struct stat s1, s2;
    if (fstat(input_1_fd, &s1) < 0 || fstat(input_2_fd, &s2) < 0) {THROW_EXCEPTION("Error occurred when getting file stats.");}

    char* input_1_data = static_cast<char*>(mmap((caddr_t)0, s1.st_size, PROT_READ, MAP_SHARED, input_1_fd, 0));
    char* input_2_data = static_cast<char*>(mmap((caddr_t)0, s2.st_size, PROT_READ, MAP_SHARED, input_2_fd, 0));

    if (input_1_data == (caddr_t)(-1) || input_2_data == (caddr_t)(-1)) {
        THROW_EXCEPTION("Error occurred, while memory-mapping the input files.");
    }

    // Will hold the files in a vector form for direct access to records
    std::vector<std::string> input_1_feature_vecs;
    std::vector<std::string> input_2_feature_vecs;

    // Determine the number of records in each dataset to simulate from
    char buf[128]; // Assumption: we know each line is less than 128 characters    
    size_t input_1_records = 0;
    size_t input_2_records = 0;

    char* input_1_copy = input_1_data;
    char* input_2_copy = input_2_data;
    while(cgets(buf, sizeof(buf), &input_1_copy) != NULL) {input_1_records++; input_1_feature_vecs.push_back(std::string(buf));}
    while(cgets(buf, sizeof(buf), &input_2_copy) != NULL) {input_2_records++; input_2_feature_vecs.push_back(std::string(buf));}

    // If in test mode, will call a certain function
    // TO DO: when HLL is implemented, make that method templated ...
    if (sim_opts.test_mode && sim_opts.use_minhash) {return simulate_test_main(input_1_feature_vecs, input_2_feature_vecs, sim_opts);}
    if (sim_opts.test_mode && sim_opts.use_hll) {NOT_IMPL("still working on using HLL for simulation mode.");}

    // Build range, that will be shuffled to get random samples
    std::vector<size_t> input_1_range (input_1_records);
    std::vector<size_t> input_2_range (input_2_records);

    std::iota(input_1_range.begin(), input_1_range.end(), 0);
    std::iota(input_2_range.begin(), input_2_range.end(), 0);

    // Determine the number of each record type in "mixed" window
    size_t num_normal_records, num_attack_records;
    std::tie(num_normal_records, num_attack_records) = determine_window_breakdown(sim_opts.num_records, sim_opts.attack_percent);

    // Simulate various windows of packets, and compute the jaccard scores
    std::vector<size_t> input_1_subset, input_2_subset, input_mixed_subset;
    std::fprintf(stdout, "type,attack_ratio,jaccard\n");

    for (size_t curr_window = 0; curr_window < sim_opts.num_windows; curr_window++) {

        // Generate the random shuffles to extract samples
        std::shuffle(input_1_range.begin(), input_1_range.end(), std::mt19937{std::random_device{}()});
        std::shuffle(input_2_range.begin(), input_2_range.end(), std::mt19937{std::random_device{}()});

        // Generates the pure "normal" and "attack" sketches, the indexes at least ...
        std::for_each(input_1_range.begin(), input_1_range.begin()+sim_opts.num_records, 
                     [&](size_t val) {input_1_subset.push_back(val);});
        std::for_each(input_2_range.begin(), input_2_range.begin()+sim_opts.num_records, 
                     [&](size_t val) {input_2_subset.push_back(val);});

        // Shuffle again before sampling indexes for mixed window ...
        std::shuffle(input_1_range.begin(), input_1_range.end(), std::mt19937{std::random_device{}()});
        std::shuffle(input_2_range.begin(), input_2_range.end(), std::mt19937{std::random_device{}()});
        
        // Generates the "mixed" skectch, some normal and some attack records ...
        std::for_each(input_1_range.begin(), input_1_range.begin()+num_normal_records, 
                     [&](size_t val) {input_mixed_subset.push_back(val);});
        std::for_each(input_2_range.begin(), input_2_range.begin()+num_attack_records, 
                     [&](size_t val) {input_mixed_subset.push_back(val);});
                
        auto file_1_records = sample_records_at_indexes(input_1_feature_vecs, input_1_subset);
        auto file_2_records = sample_records_at_indexes(input_2_feature_vecs, input_2_subset);
        auto file_mixed_records = sample_mixed_records_at_indexes(input_1_feature_vecs, num_normal_records,
                                                                  input_2_feature_vecs, num_attack_records,
                                                                  input_mixed_subset);
                                                                  
        // At this point, we have 3 different random samples: 1 "pure" normal, 1 "pure" attack, and 1 "mixed" window
        std::vector<std::string> window_dataset;
        if (sim_opts.curr_sketch == MINHASH) {
            MinHash data_sketch_1 (file_1_records, sim_opts.k_size, sim_opts.input_data_type);
            MinHash data_sketch_2 (file_2_records, sim_opts.k_size, sim_opts.input_data_type);
            MinHash data_sketch_mixed (file_mixed_records, sim_opts.k_size, sim_opts.input_data_type);

            auto jaccard_1_mixed = MinHash::compute_jaccard(data_sketch_1, data_sketch_mixed);
            auto jaccard_2_mixed = MinHash::compute_jaccard(data_sketch_2, data_sketch_mixed);
            double estimated_attack_records = (jaccard_2_mixed + 0.0)/(jaccard_1_mixed + jaccard_2_mixed);
        
            std::fprintf(stdout, "%s,%3.2f,%6.4f\n", "normal_attack", sim_opts.attack_percent, jaccard_1_mixed);
            std::fprintf(stdout, "%s,%3.2f,%6.4f\n", "attack_attack", sim_opts.attack_percent, jaccard_2_mixed);
            std::fprintf(stdout, "%s,%3.2f,%6.4f\n", "est_attack_ratio", sim_opts.attack_percent, estimated_attack_records);
        }
        else if (sim_opts.curr_sketch == HLL) {
            NOT_IMPL("still working on using HLL for simulation ...");
        }

        input_1_subset.clear();
        input_2_subset.clear();
        input_mixed_subset.clear();
    }

       
    if (munmap(input_1_data, s1.st_size) < 0 || munmap(input_2_data, s2.st_size) < 0) {
        perror("Error occurred while unmapping the input files");
        std::exit(1);
    }
    return 1;
}

int simulate_test_main(std::vector<std::string> normal_records, std::vector<std::string> attack_records, PacsketchSimulateOptions sim_opts) {
    /* main method of simulate sub-command when test-mode is turned on */

    // Open test files, and memory map them
    struct stat s1, s2;
    size_t test_normal_fd = open(sim_opts.test_files[0].data(), O_RDONLY);
    size_t test_attack_fd = open(sim_opts.test_files[1].data(), O_RDONLY);

    if (fstat(test_normal_fd, &s1) < 0) {THROW_EXCEPTION("Error occurred when getting test file stats.");}
    if (fstat(test_attack_fd, &s2) < 0) {THROW_EXCEPTION("Error occurred when getting test file stats.");}
    char* test_normal_data = static_cast<char*>(mmap((caddr_t)0, s1.st_size, PROT_READ, MAP_SHARED, test_normal_fd, 0));
    char* test_attack_data = static_cast<char*>(mmap((caddr_t)0, s2.st_size, PROT_READ, MAP_SHARED, test_attack_fd, 0));

    // Parse the both test files into feature vectors
    char buf[128];
    std::vector<std::string> test_normal_feature_vecs, test_attack_feature_vecs;

    char* test_normal_copy = test_normal_data;
    char* test_attack_copy = test_attack_data;
    while(cgets(buf, sizeof(buf), &test_normal_copy) != NULL) {test_normal_feature_vecs.push_back(std::string(buf));}
    while(cgets(buf, sizeof(buf), &test_attack_copy) != NULL) {test_attack_feature_vecs.push_back(std::string(buf));}

    // Build a range of indexes that could be selected from test set
    std::vector<size_t> test_normal_set_range (test_normal_feature_vecs.size());
    std::vector<size_t> test_attack_set_range (test_attack_feature_vecs.size());
    std::iota(test_normal_set_range.begin(), test_normal_set_range.end(), 0);
    std::iota(test_attack_set_range.begin(), test_attack_set_range.end(), 0);

    // Build the overall "normal" and "attack" sketches, based on training set
    // IMPORTANT: when this function is templated, the "MinHash" will be "T"
    MinHash normal_sketch (normal_records, sim_opts.k_size, sim_opts.input_data_type);
    MinHash attack_sketch (attack_records, sim_opts.k_size, sim_opts.input_data_type);

    // Set up the confusion matrix, to be able to compute classification metrics
    std::array<size_t, 2> true_normal_row = {0, 0}; // TP, FN
    std::array<size_t, 2> true_attack_row = {0, 0}; // FP, TN
    auto increment_confusion = [&](double est_val, double true_val) {if (true_val >= 0.50) {
                                                                        if (est_val < 0.50) {true_attack_row[0]++;}
                                                                        else {true_attack_row[1]++;}
                                                                      } else {
                                                                        if (est_val < 0.50) {true_normal_row[0]++;}
                                                                        else {true_normal_row[1]++;}
                                                                      } 
                                                                    };

    // Simulate the requested number of windows
    std::srand(time(NULL));
    std::vector<size_t> test_set_subset;
    std::fprintf(stdout, "approach,true_attack_ratio,jaccard_normal,jaccard_attack,est_attack_ratio\n");

    for (size_t curr_window = 0; curr_window < sim_opts.num_windows; curr_window++) {

        // Generate the random shuffles, and extract the random indexes
        std::shuffle(test_normal_set_range.begin(), test_normal_set_range.end(), std::mt19937{std::random_device{}()});
        std::shuffle(test_attack_set_range.begin(), test_attack_set_range.end(), std::mt19937{std::random_device{}()});

        // Randomly decide what percentage of attack records do you want
        double attack_ratio = ((double) std::rand())/RAND_MAX;
        attack_ratio = std::round(attack_ratio * 1000.0)/1000.0;
        double normal_ratio = 1 - attack_ratio;

        size_t num_normal_records, num_attack_records;
        std::tie(num_normal_records, num_attack_records) = determine_window_breakdown(sim_opts.num_records, attack_ratio);

        // Generates the "test window" sketch, some normal and some attack records ...
        std::for_each(test_normal_set_range.begin(), test_normal_set_range.begin()+num_normal_records, 
                     [&](size_t val) {test_set_subset.push_back(val);});
        std::for_each(test_attack_set_range.begin(), test_attack_set_range.begin()+num_attack_records, 
                     [&](size_t val) {test_set_subset.push_back(val);});

        auto test_mixed_records = sample_mixed_records_at_indexes(test_normal_feature_vecs, num_normal_records,
                                                                  test_attack_feature_vecs, num_attack_records,
                                                                  test_set_subset);

        // Extracts the true ratios (rounding could have affected it)
        double true_normal_percent, true_attack_percent;
        std::tie(true_normal_percent, true_attack_percent) = analyze_record_labels_in_window(test_mixed_records);

        if (sim_opts.curr_sketch == MINHASH) {
            MinHash test_sketch (test_mixed_records, sim_opts.k_size, sim_opts.input_data_type);

            auto jaccard_normal = MinHash::compute_jaccard(test_sketch, normal_sketch);
            auto jaccard_attack = MinHash::compute_jaccard(test_sketch, attack_sketch);

            double estimated_attack_jaccard = (jaccard_attack + 0.0)/(jaccard_attack + jaccard_normal);
            double estimated_attack_sampler = estimate_attack_ratio_with_sampling(test_mixed_records);
            increment_confusion(estimated_attack_jaccard, true_attack_percent);

            std::fprintf(stdout, "%s,%6.4f,%6.4f,%6.4f,%6.4f\n",
                                "pacsketch", true_attack_percent, jaccard_normal, 
                                jaccard_attack, estimated_attack_jaccard);
            std::fprintf(stdout, "%s,%6.4f,%6.4f,%6.4f,%6.4f\n",
                                "sampling", true_attack_percent, jaccard_normal, 
                                jaccard_attack, estimated_attack_sampler);

        }
        else if (sim_opts.curr_sketch == HLL) {
            NOT_IMPL("still working on implementing this method for HLLs ...");
        }
        
        test_set_subset.clear();
    }
    
    // Print confusion matrix to stderr ...
    std::fprintf(stderr, "Pacsketch Confusion Matrix on Test-Data ...\n");
    std::fprintf(stderr, "\tTP = %d, FN = %d\n", true_normal_row[0], true_normal_row[1]);
    std::fprintf(stderr, "\tFP = %d, TN = %d\n", true_attack_row[0], true_attack_row[1]);

   if (munmap(test_normal_data, s1.st_size) < 0 || munmap(test_attack_data, s2.st_size) < 0) {
        perror("Error occurred while unmapping the input files");
        std::exit(1);
    }
    return 1;
}

double estimate_attack_ratio_with_sampling(std::vector<std::string> window_records) {
    /* 
     * Assuming the sampler is an Oracle, meaning that it can 100% accurately
     * predict whether a certain connection record is anomalous or not. Then,
     * what would be the percentage of this sample that is actually
     * attack records.
     */
    size_t num_samples = (size_t) (window_records.size() * SAMPLING_RATE);
    
    std::vector<size_t> sample_range (window_records.size());
    std::iota(sample_range.begin(), sample_range.end(), 0);
    std::shuffle(sample_range.begin(), sample_range.end(), std::mt19937{std::random_device{}()});

    std::vector<size_t> sample_subset;
    std::for_each(sample_range.begin(), sample_range.begin()+num_samples, 
                     [&](size_t val) {sample_subset.push_back(val);});
    auto sample_records = sample_records_at_indexes(window_records, sample_subset);

    double sample_normal_percent, sample_attack_percent;
    std::tie(sample_normal_percent, sample_attack_percent) = analyze_record_labels_in_window(sample_records);
    return sample_attack_percent;
}

std::tuple<double, double> analyze_record_labels_in_window(std::vector<std::string> window_records) {
    /* 
     * Returns the following tuple: <normal percent, attack percent> of the provided
     * window of data records, the two values should add up to 1.
     */
    
    size_t num_normal = 0;
    for (auto record: window_records) {
        // Grab label, trim whitespace, and check it
        auto word_list = split(record, ',');
        std::string label = word_list.back();
        label.erase(std::remove_if(label.begin(), label.end(), isspace), label.end());

        if (!label.compare("normal")) {num_normal++; }
    }

    double normal_ratio, attack_ratio;
    normal_ratio = (num_normal+0.0)/window_records.size();

    normal_ratio = std::round(normal_ratio * 1000.0)/1000.0;
    attack_ratio = 1.0 - normal_ratio;
    return std::make_tuple(normal_ratio, attack_ratio);
}

inline std::tuple<size_t, size_t> determine_window_breakdown(size_t total_num, double attack_ratio) {
    /* 
     * Returns a tuple saying how many normal and attack records to include in the simulated 
     * window of TCP connections from KDD datasets. Here is the return structure <normal, attack>.
     */
    size_t num_attack = (size_t) (total_num * attack_ratio);
    size_t num_normal = total_num - num_attack;
    return std::make_tuple(num_normal, num_attack);
}

std::vector<std::string> sample_mixed_records_at_indexes(std::vector<std::string> dataset_1_vecs, size_t num_dataset_1,
                                                         std::vector<std::string> dataset_2_vecs, size_t num_dataset_2,
                                                         std::vector<size_t> index_list) {
    /* Extract the data records from the two input datasets, it is a mixture of normal and attack records */
    std::vector<std::string> dataset_sample;
    size_t total_num_records = num_dataset_1 + num_dataset_2;
    for (size_t i = 0; i < num_dataset_1; i++) {dataset_sample.push_back(dataset_1_vecs.at(index_list.at(i)));}
    for (size_t i = num_dataset_1; i < total_num_records; i++) {dataset_sample.push_back(dataset_2_vecs.at(index_list.at(i)));}
    return dataset_sample;
}

std::vector<std::string> sample_records_at_indexes(std::vector<std::string> dataset_vecs, std::vector<size_t> index_list) {
    /* Extracts the data records at provided indexes */
    std::vector<std::string> dataset_sample;
    for (size_t curr_index: index_list) {dataset_sample.push_back(dataset_vecs.at(curr_index));}
    return dataset_sample;
}

char* cgets(char* buf, int max, char** data) {
    /* 
     * Fills a c-string buffer until newline is encountered or max characters read 
     * This method is adapted from this code: https://stackoverflow.com/questions/20299987/loop-through-char-array-line-by-line
     */
    size_t i = 0;
    if(**data == '\0') {return NULL;}

    for(i = 0; i < max-1; ++i, ++(*data)){
        buf[i] = **data;
        if(**data == '\n') {buf[i+1]='\0'; ++(*data); break;} // Hit newline
        if(**data == '\0') {break;} // input data has ended
    }
    if(i == max-1) {buf[i] = '\0';}
    return buf;
}


int main(int argc, char** argv){
    /* main method for pacsketch package */
    
    if (argc > 1) {
        if (std::strcmp(argv[1], "build") == 0)
            return build_main(argc-1, argv+1);
        if (std::strcmp(argv[1], "dist") == 0)
            return dist_main(argc-1, argv+1);
        if (std::strcmp(argv[1], "simulate") == 0)
            return simulate_main(argc-1, argv+1);
    }
    return pacsketch_usage();
}
