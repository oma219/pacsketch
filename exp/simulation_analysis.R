###############################################################
#
# simulation_analysis.R - Analyzes the jaccard similarity for
#                         windows of connection data from 
#                         different label types.
#                   
#                  
# Author: Omar Ahmed
#
###############################################################

# Needed libraries
library(ggplot2)
library(ggpubr)
library(plyr)

# Data paths ...
simulate_path_100 <-"/Users/omarahmed/Downloads/pacsketch_workspace/simulate_exp/simulate_ratio_100_percent.csv"
simulate_path_70 <- "/Users/omarahmed/Downloads/pacsketch_workspace/simulate_exp/simulate_ratio_70_percent.csv"
simulate_path_40 <- "/Users/omarahmed/Downloads/pacsketch_workspace/simulate_exp/simulate_ratio_40_percent.csv"
simulate_path_10 <- "/Users/omarahmed/Downloads/pacsketch_workspace/simulate_exp/simulate_ratio_10_percent.csv"

#############################################################################
#
# Each plot will consist of two sub-plots: the first one shows the jaccard 
# distributions when comparing a test window against a normal or attack
# sketch. The second will show the ratio of the jaccards as an approach to 
# estimate the percentage of records that are anomalous or attack records
# in the test window.
#
# Each plot will differ based on percentage of records in that window that 
# truly attack records. The tested percentages are 100, 70, 40, and 10.
#
#############################################################################


#############################################################################
# Start with attack percentage of 100% ....
#############################################################################

# Plot 1 - Jaccard Distributions with Attack Percentage = 100%
df_100 <- read.csv(simulate_path_100, header=TRUE)
plot1<- ggplot(subset(df_100, df_100$type == "normal_attack" | df_100$type == "attack_attack")) +
        geom_density(aes(x=jaccard, fill=type, colour=type), alpha=0.4, adjust=12) +
        theme_bw() +
        theme(plot.title=element_text(hjust = 0.5, size=14, face="bold"),
                axis.title.x=element_text(size =14),
                axis.title.y=element_text(size=14),
                legend.position = "bottom", 
                legend.text=element_text(size=10),
                legend.title=element_text(size=10),
                axis.text=element_text(size=14, color="black")) +
        labs(x="Jaccard",
             y="Density",
             title="Jaccard Distributions with Attack Percentage = 100%") +
        scale_x_continuous(limits=c(0, 0.8), breaks=seq(0, 0.8, 0.1)) +
        scale_colour_discrete(name="Comparison Groups:", labels=c("Attack_Attack_100%", "Normal_Attack_100%")) + 
        scale_fill_discrete(name="Comparison Groups:", labels=c("Attack_Attack_100%", "Normal_Attack_100%"))
plot1

# Plot 2 - Ratio of Jaccards with Attack Percentage = 100%
plot2<- ggplot(subset(df_100, df_100$type == "est_attack_ratio")) +
        geom_density(aes(x=jaccard), alpha=0.4, adjust=12, colour="green", fill="lightgreen") +
        theme_bw() +
        theme(plot.title=element_text(hjust = 0.5, size=14, face="bold"),
              axis.title.x=element_text(size =14),
              axis.title.y=element_text(size=14),
              legend.position = "none", 
              legend.text=element_text(size=10),
              legend.title=element_text(size=10),
              axis.text=element_text(size=14, color="black")) +
        labs(x="Attack Jaccard/Sum of Jaccards",
             y="Density",
             title="Estimated Percentage of Attack Records") +
        scale_x_continuous(limits=c(0.9, 1.0), breaks=seq(0.9, 1.0, 0.01)) +
        geom_vline(xintercept=1.0, linetype="dashed", color = "blue", size=1.5)
plot2

# Combine plot1 and plot2 into first figure ...
figure1 <- ggarrange(plot1, plot2, labels = c("a", "b"), ncol = 2, nrow = 1, common.legend=TRUE, legend="bottom")
figure1

output_name1 <- "/Users/omarahmed/downloads/simulate_100_percent_window_figure.jpeg"
ggsave(output_name1, plot=figure1, dpi=800, device="jpeg", width=12, height=6)


#############################################################################
# Next is an attack percentage of 70% ....
#############################################################################

# Plot 3 - Jaccard Distributions with Attack Percentage = 70%
df_70 <- read.csv(simulate_path_70, header=TRUE)
plot3<- ggplot(subset(df_70, df_70$type == "normal_attack" | df_70$type == "attack_attack")) +
        geom_density(aes(x=jaccard, fill=type, colour=type), alpha=0.4, adjust=10) +
        theme_bw() +
        theme(plot.title=element_text(hjust = 0.5, size=14, face="bold"),
              axis.title.x=element_text(size =14),
              axis.title.y=element_text(size=14),
              legend.position = "bottom", 
              legend.text=element_text(size=10),
              legend.title=element_text(size=10),
              axis.text=element_text(size=14, color="black")) +
        labs(x="Jaccard",
             y="Density",
             title="Jaccard Distributions with Attack Percentage = 70%") +
        scale_x_continuous(limits=c(0, 0.8), breaks=seq(0, 0.8, 0.1)) +
        scale_colour_discrete(name="Comparison Groups:", labels=c("Attack_Attack_70%", "Normal_Attack_70%")) + 
        scale_fill_discrete(name="Comparison Groups:", labels=c("Attack_Attack_70%", "Normal_Attack_70%"))
plot3

# Plot 4 - Ratio of Jaccards with Attack Percentage = 70%
plot4<- ggplot(subset(df_70, df_70$type == "est_attack_ratio")) +
        geom_density(aes(x=jaccard), alpha=0.4, adjust=12, colour="green", fill="lightgreen") +
        theme_bw() +
        theme(plot.title=element_text(hjust = 0.5, size=14, face="bold"),
              axis.title.x=element_text(size =14),
              axis.title.y=element_text(size=14),
              legend.position = "none", 
              legend.text=element_text(size=10),
              legend.title=element_text(size=10),
              axis.text=element_text(size=14, color="black")) +
        labs(x="Attack Jaccard/Sum of Jaccards",
             y="Density",
             title="Estimated Percentage of Attack Records") +
        scale_x_continuous(limits=c(0.0, 1.0), breaks=seq(0.0, 1.0, 0.1)) +
        geom_vline(xintercept=0.70, linetype="dashed", color = "blue", size=1.5)
plot4

# Combine plot3 and plot4 into second figure ...
figure2 <- ggarrange(plot3, plot4, labels = c("a", "b"), ncol = 2, nrow = 1, common.legend=TRUE, legend="bottom")
figure2

output_name2 <- "/Users/omarahmed/downloads/simulate_70_percent_window_figure.jpeg"
ggsave(output_name2, plot=figure2, dpi=800, device="jpeg", width=12, height=6)


#############################################################################
# Next is an attack percentage of 40% ....
#############################################################################

# Plot 5 - Jaccard Distributions with Attack Percentage = 40%
df_40 <- read.csv(simulate_path_40, header=TRUE)
plot5<- ggplot(subset(df_40, df_40$type == "normal_attack" | df_40$type == "attack_attack")) +
        geom_density(aes(x=jaccard, fill=type, colour=type), alpha=0.4, adjust=10) +
        theme_bw() +
        theme(plot.title=element_text(hjust = 0.5, size=14, face="bold"),
              axis.title.x=element_text(size =14),
              axis.title.y=element_text(size=14),
              legend.position = "bottom", 
              legend.text=element_text(size=10),
              legend.title=element_text(size=10),
              axis.text=element_text(size=14, color="black")) +
        labs(x="Jaccard",
             y="Density",
             title="Jaccard Distributions with Attack Percentage = 40%") +
        scale_x_continuous(limits=c(0, 0.8), breaks=seq(0, 0.8, 0.1)) +
        scale_colour_discrete(name="Comparison Groups:", labels=c("Attack_Attack_40%", "Normal_Attack_40%")) + 
        scale_fill_discrete(name="Comparison Groups:", labels=c("Attack_Attack_40%", "Normal_Attack_40%"))
plot5

# Plot 6 - Ratio of Jaccards with Attack Percentage = 40%
plot6<- ggplot(subset(df_40, df_40$type == "est_attack_ratio")) +
        geom_density(aes(x=jaccard), alpha=0.4, adjust=12, colour="green", fill="lightgreen") +
        theme_bw() +
        theme(plot.title=element_text(hjust = 0.5, size=14, face="bold"),
              axis.title.x=element_text(size =14),
              axis.title.y=element_text(size=14),
              legend.position = "none", 
              legend.text=element_text(size=10),
              legend.title=element_text(size=10),
              axis.text=element_text(size=14, color="black")) +
        labs(x="Attack Jaccard/Sum of Jaccards",
             y="Density",
             title="Estimated Percentage of Attack Records") +
        scale_x_continuous(limits=c(0.0, 1.0), breaks=seq(0.0, 1.0, 0.1)) +
        geom_vline(xintercept=0.40, linetype="dashed", color = "blue", size=1.5)
plot6

# Combine plot5 and plot6 into third figure ...
figure3 <- ggarrange(plot5, plot6, labels = c("a", "b"), ncol = 2, nrow = 1, common.legend=TRUE, legend="bottom")
figure3


#############################################################################
# Next is an attack percentage of 10% ....
#############################################################################

# Plot 7 - Jaccard Distributions with Attack Percentage = 10%
df_10 <- read.csv(simulate_path_10, header=TRUE)
plot7<- ggplot(subset(df_10, df_10$type == "normal_attack" | df_10$type == "attack_attack")) +
        geom_density(aes(x=jaccard, fill=type, colour=type), alpha=0.4, adjust=10) +
        theme_bw() +
        theme(plot.title=element_text(hjust = 0.5, size=14, face="bold"),
              axis.title.x=element_text(size =14),
              axis.title.y=element_text(size=14),
              legend.position = "bottom", 
              legend.text=element_text(size=10),
              legend.title=element_text(size=10),
              axis.text=element_text(size=14, color="black")) +
        labs(x="Jaccard",
             y="Density",
             title="Jaccard Distributions with Attack Percentage = 10%") +
        scale_x_continuous(limits=c(0, 0.8), breaks=seq(0, 0.8, 0.1)) +
        scale_colour_discrete(name="Comparison Groups:", labels=c("Attack_Attack_10%", "Normal_Attack_10%")) + 
        scale_fill_discrete(name="Comparison Groups:", labels=c("Attack_Attack_10%", "Normal_Attack_10%"))
plot7

# Plot 8 - Ratio of Jaccards with Attack Percentage = 10%
plot8<- ggplot(subset(df_10, df_10$type == "est_attack_ratio")) +
        geom_density(aes(x=jaccard), alpha=0.4, adjust=12, colour="green", fill="lightgreen") +
        theme_bw() +
        theme(plot.title=element_text(hjust = 0.5, size=14, face="bold"),
              axis.title.x=element_text(size =14),
              axis.title.y=element_text(size=14),
              legend.position = "none", 
              legend.text=element_text(size=10),
              legend.title=element_text(size=10),
              axis.text=element_text(size=14, color="black")) +
        labs(x="Attack Jaccard/Sum of Jaccards",
             y="Density",
             title="Estimated Percentage of Attack Records") +
        scale_x_continuous(limits=c(0.0, 1.0), breaks=seq(0.0, 1.0, 0.1)) +
        geom_vline(xintercept=0.10, linetype="dashed", color = "blue", size=1.5)
plot8

# Combine plot7 and plot8 into fourth figure ...
figure4 <- ggarrange(plot7, plot8, labels = c("a", "b"), ncol = 2, nrow = 1, common.legend=TRUE, legend="bottom")
figure4

output_name4 <- "/Users/omarahmed/downloads/simulate_10_percent_window_figure.jpeg"
ggsave(output_name4, plot=figure4, dpi=800, device="jpeg", width=12, height=6)

