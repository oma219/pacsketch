#####################################################################
#
# attack_feature_analysis.R - Analyzes the distributions of features
#                             different attack types to see if it
#                          .  reasonable to assume we can classify
#                   
#                  
# Author: Omar Ahmed
#
#####################################################################

# Needed libraries
library(ggplot2)
library(ggpubr)

# Data paths ...
nsl_kdd_dataset <- "/Users/omarahmed/Downloads/pacsketch_workspace/dataset_analysis/nsl_kdd_select_attacks_dataset_for_plotting.csv"

# Summary function ...
data_summary <- function(x) {
  m <- mean(x)
  ymin <- m-sd(x)
  ymax <- m+sd(x)
  return(c(y=m,ymin=ymin,ymax=ymax))
}

##############################################################
#
# This first section will focus on creating grouped violin 
# charts for individual features to compare the distribution
# across neptune, smurf, and ipsweep attack records.
#
###############################################################

# Need to run this before making any of plots below ...
nsl_kdd_df <- read.csv(nsl_kdd_dataset, header=TRUE)
nsl_kdd_df$feature_name <- as.factor(nsl_kdd_df$feature_name)
nsl_kdd_df$value <- as.double(nsl_kdd_df$value)

# Plot 1 - Creates grouped violen chart for src_count...
nsl_kdd_df_subset <- subset(nsl_kdd_df, nsl_kdd_df$feature_name=="srv_count" & nsl_kdd_df$value > 0.0)

plot1<- ggplot(nsl_kdd_df_subset, aes(x=feature_name, y=value, fill=label)) +
        geom_violin(alpha=0.5, width=0.7) +
        theme_bw() +
        theme(plot.title=element_text(hjust = 0.5, size=14, face="bold"),
              axis.title.x=element_blank(),
              axis.ticks.x=element_blank(),
              axis.text.x=element_blank(),
              axis.title.y=element_text(size=14),
              legend.position = "bottom", 
              legend.text=element_text(size=10),
              legend.title=element_text(size=10),
              axis.text.y=element_text(size=14, color="black")) +
        labs(x="",
             y="log(srv_count)",
             title="Number of Connections to Same Port") +
        scale_y_continuous(trans='log10') +
        scale_fill_discrete(name="Connection Type", labels=c("IPsweep", "Neptune", "Smurf")) +
        stat_summary(fun.data=data_summary,color="red", position=position_dodge(0.7))
plot1

output_name1 <- "/Users/omarahmed/downloads/feature_1_violin.jpeg"
ggsave(output_name1, plot=plot1, dpi=800, device="jpeg", width=6, height=6)

# Plot 2 - Creates grouped violen chart for ...
nsl_kdd_df_subset <- subset(nsl_kdd_df, nsl_kdd_df$feature_name=="dst_host_srv_count" & nsl_kdd_df$value > 0.0)

plot2<- ggplot(nsl_kdd_df_subset, aes(x=feature_name, y=value, fill=label)) +
        geom_violin(alpha=0.5, width=0.7) +
        theme_bw() +
        theme(plot.title=element_text(hjust = 0.5, size=14, face="bold"),
              axis.title.x=element_blank(),
              axis.ticks.x=element_blank(),
              axis.text.x=element_blank(),
              axis.title.y=element_text(size=14),
              legend.position = "bottom", 
              legend.text=element_text(size=10),
              legend.title=element_text(size=10),
              axis.text.y=element_text(size=14, color="black")) +
        labs(x="",
             y="log(dst_host_srv_count)",
             title="Number of Connections Having Same Port Number") +
        scale_y_continuous(trans='log10') +
        scale_fill_discrete(name="Connection Type", labels=c("IPsweep", "Neptune", "Smurf")) +
        stat_summary(fun.data=data_summary,color="red", position=position_dodge(0.7))
plot2

output_name2 <- "/Users/omarahmed/downloads/feature_2_violin.jpeg"
ggsave(output_name2, plot=plot2, dpi=800, device="jpeg", width=6, height=6)
