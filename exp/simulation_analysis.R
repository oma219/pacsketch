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
simulate_path_1 <- "/Users/omarahmed/Downloads/pacsketch_workspace/simulate_exp/simulate_100_percent.csv"

#############################################################################
#
# This section of the script generates plots that will plot the density
# curves of the jaccard similarities for different types of comparisons
# including normal-normal, normal-attack, and attack-attack.
#
#############################################################################

# Plot 1 - Looks at percent error for different sizes of MinHashes
mu <- ddply(df_1, "type", summarise, grp.mean=mean(jaccard))
head(mu)

df_1 <- read.csv(simulate_path_1, header=TRUE)
plot1<- ggplot(df_1) +
        geom_density(aes(x=jaccard, fill=type, colour=type), alpha=0.4, adjust=15) +
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
             title="Jaccard Distributions When Comparing Different Network Sketches") +
        scale_x_continuous(limits=c(0, 0.8), breaks=seq(0, 0.8, 0.1)) +
        scale_colour_discrete(name="Comparison Groups:", labels=c("Attack_Attack", "Normal_Attack","Normal_Normal")) + 
        scale_fill_discrete(name="Comparison Groups:", labels=c("Attack_Attack", "Normal_Attack","Normal_Normal"))
plot1

output_name1 <- "/Users/omarahmed/downloads/simulate_100_percent_window.jpeg"
ggsave(output_name1, plot=plot1, dpi=800, device="jpeg", width=8, height=6)
