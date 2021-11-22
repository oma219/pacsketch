###############################################################
#
# jaccard_exp_analysis.R - Analyzes the percent error for
#                              HLL and MinHash with varying 
#                              size of the data-structure.
#                   
#                  
# Author: Omar Ahmed
#
###############################################################

# Needed libraries
library(ggplot2)
library(ggpubr)

# Data paths ...
hll_card_path <- "/Users/omarahmed/Downloads/test_dir/total_hll_cardinality_results.csv"
hll_jaccard_path <- "/Users/omarahmed/Downloads/test_dir/total_hll_jaccard_results.csv"
minhash_card_path <- "/Users/omarahmed/Downloads/test_dir/total_minhash_cardinality_results.csv"
minhash_jaccard_path <- "/Users/omarahmed/Downloads/test_dir/total_minhash_jaccard_results.csv"

#############################################################################
#
# This section of the script generates plots that look at the cardinality 
# error for the HLL and MinHash. There are first two point plots, folllowed
# by two box-and-whisker plots.
#
#############################################################################

# Plot 1 - Looks at percent error for different sizes of MinHashes
minhash_card_df <- read.csv(minhash_card_path, header=TRUE)
plot1<- ggplot(minhash_card_df) +
        geom_point(aes(x=dataset_num, y=percent_error, group=as.factor(k_or_b), color=as.factor(k_or_b))) +
        theme_bw() +
        theme(plot.title=element_text(hjust = 0.5, size=14, face="bold"),
              axis.title.x=element_text(size =14),
              axis.title.y=element_text(size=14),
              legend.position = "bottom", 
              legend.text=element_text(size=10),
              legend.title=element_text(size=10),
              axis.text=element_text(size=14, color="black")) +
        labs(x="Trial (Different Random Datasets)",
             y="Cardinality % Error",
             title="Cardinality Estimation with MinHash with Varying Memory Footprints") +
        ylim(-100, 100) +
        scale_color_discrete(name="Data Structure", labels=c("MinHash (k=5)", "MinHash (k=20)", 
                                                       "MinHash (k=60)","MinHash (k=100)"))
plot1
output_name1 <- "/Users/omarahmed/downloads/minhash_card_errors.jpeg"
ggsave(output_name1, plot=plot1, dpi=800, device="jpeg", width=9, height=6)


# Plot 2- Looks at percent error for different sizes of HLLs
hll_card_df <- read.csv(hll_card_path, header=TRUE)
plot2<- ggplot(hll_card_df) +
               geom_point(aes(x=dataset_num, y=percent_error, group=as.factor(k_or_b), color=as.factor(k_or_b))) +
               theme_bw() +
              theme(plot.title=element_text(hjust = 0.5, size=14, face="bold"),
                    axis.title.x=element_text(size =14),
                    axis.title.y=element_text(size=14),
                    legend.position = "bottom", 
                    legend.text=element_text(size=10),
                    legend.title=element_text(size=10),
                    axis.text=element_text(size=14, color="black")) +
               labs(x="Trial (Different Random Datasets)",
                    y="Cardinality % Error",
                    title="Cardinality Estimation with HyperLogLog with Varying Memory Footprints") +
               ylim(-100, 100) +
               scale_color_discrete(name="Data Structure", labels=c("HLL (b=5)", "HLL (b=7)", 
                                                                    "HLL (b=9)","HLL (b=11)"))
plot2
output_name2 <- "/Users/omarahmed/downloads/hll_card_errors.jpeg"
ggsave(output_name2, plot=plot2, dpi=800, device="jpeg", width=9, height=6)


# Plot 3- Looks at percent error for different sizes of MinHash in Box-and-Whisker plot
minhash_card_df <- read.csv(minhash_card_path, header=TRUE)
plot3<- ggplot(minhash_card_df) +
               geom_boxplot(aes(x=as.factor(k_or_b), y=abs(percent_error), color=as.factor(k_or_b)),
                            outlier.shape=4) +
               theme_classic() +
               theme(plot.title = element_text(hjust = 0.5, size=14, face="bold"),
                     axis.title.x = element_text(size =14,vjust=0),
                     axis.title.y=element_text(size=14),
                     legend.position = "none", 
                     legend.text = element_text(size=10),
                     axis.text=element_text(size=14, color="black")) +
               labs(x="Number of Hashes (k)",
                    y="Cardinality % Error",
                    title="Cardinality Estimation with MinHash with Varying Memory Footprints")  +
               scale_y_continuous(breaks=seq(0, 120, 20)) +
               scale_color_discrete(name="Number of Hashes (k):")
plot3
output_name3 <- "/Users/omarahmed/downloads/minhash_card_errors_boxplot.jpeg"
ggsave(output_name3, plot=plot3, dpi=800, device="jpeg", width=7.5, height=6)


# Plot 4 - Looks at percent error for different sizes of HLL in Box-and-Whisker plot
hll_card_df <- read.csv(hll_card_path, header=TRUE)
plot4<- ggplot(hll_card_df) +
               geom_boxplot(aes(x=as.factor(k_or_b), y=abs(percent_error), color=as.factor(k_or_b)),
                           outlier.shape=4) +
               theme_classic() +
               theme(plot.title = element_text(hjust = 0.5, size=14, face="bold"),
                     axis.title.x = element_text(size =14, vjust=0),
                     axis.title.y=element_text(size=14),
                     legend.position = "none", 
                     legend.text = element_text(size=10),
                     axis.text=element_text(size=14, color="black")) +
               labs(x="Number of Prefix-Bits (b)",
                    y="Cardinality % Error",
                    title="Cardinality Estimation with HyperLogLog with Varying Memory Footprints")  +
               scale_y_continuous(breaks=seq(0, 125, 25)) +
               scale_color_discrete(name="Number of Prefix-Bits (b):")
plot4
output_name4 <- "/Users/omarahmed/downloads/hll_card_errors_boxplot.jpeg"
ggsave(output_name4, plot=plot4, dpi=800, device="jpeg", width=7.5, height=6)


#############################################################################
#
# This section of the script generates plots that look at the jaccard error
# for the HLL and MinHash. There are two box-and-whisker plots.
#
#############################################################################

# Plot 5- Looks at MinHash error for computing Jaccard in box-and-whisker plot
minhash_jaccard_df <- read.csv(minhash_jaccard_path, header=TRUE)
plot5<- ggplot(minhash_jaccard_df) +
        geom_boxplot(aes(x=as.factor(k_or_b), y=abs(jaccard_error), color=as.factor(k_or_b)),
                     outlier.shape=4) +
        theme_classic() +
        theme(plot.title = element_text(hjust = 0.5, size=14, face="bold"),
              axis.title.x = element_text(size =14, vjust=0),
              axis.title.y=element_text(size=14),
              legend.position = "none", 
              legend.text = element_text(size=10),
              axis.text=element_text(size=14, color="black")) +
        labs(x="Number of Hashes (k)",
             y="|True J - Estimated J|",
             title="Jaccard Error with MinHash of Varying Memory Footprints")  +
        scale_y_continuous(breaks=seq(0, 0.6, 0.1)) +
        scale_color_discrete(name="Number of Hashes (k):")
plot5
output_name5 <- "/Users/omarahmed/downloads/minhash_jaccard_errors_boxplot.jpeg"
ggsave(output_name5, plot=plot5, dpi=800, device="jpeg", width=7.5, height=6)


# Plot 6- Looks at HLL error for computing Jaccard in box-and-whisker plot
hll_jaccard_df <- read.csv(hll_jaccard_path, header=TRUE)
plot6<- ggplot(hll_jaccard_df) +
        geom_boxplot(aes(x=as.factor(k_or_b), y=abs(jaccard_error), color=as.factor(k_or_b)),
                     outlier.shape=4) +
        theme_classic() +
        theme(plot.title = element_text(hjust = 0.5, size=14, face="bold"),
              axis.title.x = element_text(size =14, vjust=0),
              axis.title.y=element_text(size=14),
              legend.position = "none", 
              legend.text = element_text(size=10),
              axis.text=element_text(size=14, color="black")) +
        labs(x="Number of Prefix-Bits (b)",
             y="|True J - Estimated J|",
             title="Jaccard Error for HyperLogLogs of Varying Memory Footprints")  +
        scale_y_continuous(breaks=seq(0, 1.0, 0.1)) +
        scale_color_discrete(name="Number of Prefix-Bits (b):")
plot6
output_name6 <- "/Users/omarahmed/downloads/hll_jaccard_errors_boxplot.jpeg"
ggsave(output_name6, plot=plot6, dpi=800, device="jpeg", width=7.5, height=6)
