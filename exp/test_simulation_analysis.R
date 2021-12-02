###############################################################
#
# test_simulation_analysis.R - Analyzes the estimates produced
#                              by the pacsketch method, and
#                              a sampling approach.
#                   
#                  
# Author: Omar Ahmed
#
###############################################################

# Needed libraries
library(ggplot2)
library(ggpubr)

# Data paths ...
data_path <- "/Users/omarahmed/Downloads/pacsketch_workspace/simulate_exp_test/estimated_attack_ratios.csv"

#############################################################################
#
# This plot will compare the estimated attack ratio from two methods: our
# method called pacsketch and a sampling based approach where we assume we
# have oracle knowledge. It will be box-and-whisker plot, and the y-axis
# will the absolute difference between true attack ratio, and estimated
# difference.
#
#############################################################################

df <- read.csv(data_path, header=TRUE)
plot1<- ggplot(df) +
        geom_boxplot(aes(x=as.factor(approach), y=abs(est_attack_ratio-true_attack_ratio)*100, 
                         color=as.factor(approach)), outlier.shape=4) +
        theme_classic() +
        theme(plot.title = element_text(hjust = 0.5, size=14, face="bold"),
              axis.title.x = element_text(size =14,vjust=0),
              axis.title.y=element_text(size=14),
              legend.position = "none", 
              legend.text = element_text(size=10),
              axis.text=element_text(size=14, color="black")) +
        labs(x="Method",
             y="|Est. Attack % - True Attack %|",
             title="Comparing Pacsketch to Sampling in Estimating Anomalous Percentage")  +
        scale_y_continuous(breaks=seq(0, 30, 5)) +
        scale_color_discrete(labels=c("Pacsketch", "Sampling Oracle")) +
        scale_x_discrete(labels=c("Pacsketch", "Sampling Oracle"))
plot1

output_name1 <- "/Users/omarahmed/downloads/simulation_test_boxplot.jpeg"
ggsave(output_name1, plot=plot1, dpi=800, device="jpeg", width=7.5, height=6)
