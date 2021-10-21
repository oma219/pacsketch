###############################################################
#
# cardinality_exp_analysis.R - Analyzes the percent error for
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

# Load the data, and parse out overall dataframe into smaller dfs
file_path <- "/Users/omarahmed/Downloads/test_fa/final_output_file.csv"
df_1 <- read.csv(file_path, header=FALSE)
colnames(df_1) <- c("Trial", "true_card", "data_structure", "est_card", "percent_error")

minhash_df <- df_1[grepl("minhash", df_1$data_structure),]
hll_df <- df_1[grepl("hll", df_1$data_structure),]

# Plot 1 - Looks at percent error for different MinHash data-structures

plot1<- ggplot(minhash_df) +
        geom_point(aes(x=Trial, y=percent_error, group=data_structure, color=data_structure)) +
        theme_bw() +
        theme(plot.title = element_text(hjust = 0.5, size=10, face="italic"),
             axis.title.x = element_text(size =10),
             legend.position = "bottom", legend.text = element_text(size=10)) +
        labs(x="Trial (Different Random Datasets)",
            y="Cardinality % Error",
            title="Cardinality Estimation with MinHash with Varying Memory Footprints") +
        ylim(-100, 100) +
        scale_color_discrete(name="Data Structure", labels=c("MinHash (k=1)", "MinHash (k=10)", 
                                                             "MinHash (k=100)","MinHash (k=400)"))
#plot1  

output_name1 <- "/Users/omarahmed/downloads/minhash_errors.jpeg"
ggsave(output_name1, plot=plot1, dpi=800, device="jpeg", width=9, height=6)


# Plot 2 - Looks at the percent error for different HLL data-structures
plot2<- ggplot(hll_df) +
        geom_point(aes(x=Trial, y=percent_error, group=data_structure, color=data_structure)) +
        theme_bw() +
        theme(plot.title = element_text(hjust = 0.5, size=10, face="italic"),
              axis.title.x = element_text(size =10),
              legend.position = "bottom", legend.text = element_text(size=10)) +
        labs(x="Trial (Different Random Datasets)",
             y="Cardinality % Error",
             title="Cardinality Estimation with HLL with Varying Memory Footprints") +
        ylim(-100, 100) +
        scale_color_discrete(name="Data Structure", labels=c("HLL (b=12)", "HLL (b=3)", 
                                                             "HLL (b=6)","HLL (b=9)"))
#plot2  
output_name2 <- "/Users/omarahmed/downloads/hll_errors.jpeg"
ggsave(output_name2, plot=plot2, dpi=800, device="jpeg", width=9, height=6)

# Plot 3 - Plot the average percent error for each data-structures
file_path <- "/Users/omarahmed/Downloads/test_fa/final_averages_file.csv"
avg_err_df <- read.csv(file_path, header=FALSE)
colnames(avg_err_df) <- c("data_structure", "num_values", "total_error", "average_error")

bar_labels <- c("HLL (b=12)", "HLL (b=3)", 
                "HLL (b=6)","HLL (b=9)",
                "MinHash (k=1)", "MinHash (k=10)",
                "MinHash (k=100)", "MinHash (k=400)")

plot3 <- ggplot(data = avg_err_df) +
         geom_bar(stat="identity", aes(x=data_structure, y = average_error, 
                                       fill=data_structure), colour="black", size=0.75) +
         theme_classic() +
         theme(plot.title = element_text(hjust = 0.5, size=15, face="italic"),
            axis.title.x = element_text(size = 10, margin = margin(t = 10, r = 0, b = 0, l = 0)),
            axis.title.y = element_text(margin = margin(t = 0, r = 10, b = 0, l = 0)),
            legend.position = "none", legend.text = element_text(size=10),
            axis.ticks.x = element_blank()) +
         labs(x="Data Structures",
              y="Absolute Average % Error",
              title="Average Cardinality Error for HLL and MinHash") +
         scale_color_discrete(name="Data Structure", labels=bar_labels) +
         scale_fill_discrete(name="Data Structure", labels=bar_labels) +
         scale_x_discrete(labels=bar_labels) +
         scale_y_continuous(expand = c(0, 0), limits = c(0, 35), breaks=seq(0,35, by=5)) +
         geom_text(x=5, y=1, label="** Too large to show, it was very noisy **", 
                  color="Red", size=5, hjust=0, angle=90)
  
#plot3
output_name3 <- "/Users/omarahmed/downloads/average_hll_minhash_errors.jpeg"
ggsave(output_name3, plot=plot3, dpi=800, device="jpeg", width=9, height=6)


# Plot 4 - Show the average error based on number of registers
hll_fun <- function(x) (1.04/sqrt(x) * 100)

plot4 <- ggplot(data=data.frame(x=0), mapping=aes(x=x)) +
         stat_function(fun=hll_fun, linetype="dashed", size=0.75, color="blue") +
         scale_y_continuous(limits = c(0, 20), breaks=seq(0,20, by=2)) +
         xlim(8, 2048) + 
         theme_bw() +
         labs(x="Number of Registers (m)",
              y="Expected % Error",
              title="Expected Cardinality Error with HLLs with a Varying Number of Registers") +
         theme(plot.title = element_text(hjust = 0.5, size=12, face="italic"),
                axis.title.x = element_text(size = 10, margin = margin(t = 10, r = 0, b = 0, l = 0)),
                axis.title.y = element_text(margin = margin(t = 0, r = 10, b = 0, l = 0)),
                legend.position = "none", legend.text = element_text(size=10))

#plot4
output_name4 <- "/Users/omarahmed/downloads/expected_hll_errors.jpeg"
ggsave(output_name4, plot=plot4, dpi=800, device="jpeg", width=9, height=6)
