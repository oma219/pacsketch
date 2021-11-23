#####################################################################
#
# kdd_feature_analysis.R - Analyzes the distributions of features
#                          in the KDD dataset across normal and
#                          attack records.
#                   
#                  
# Author: Omar Ahmed
#
#####################################################################

# Needed libraries
library(ggplot2)
library(ggpubr)

# Data paths ...
nsl_kdd_dataset <- "/Users/omarahmed/Downloads/pacsketch_workspace/dataset_analysis/nsl_kdd_alternate_dataset.csv"

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
# across normal and attack connection records.
#
###############################################################

# Need to run this before making any of plots below ...
nsl_kdd_df <- read.csv(nsl_kdd_dataset, header=TRUE)
nsl_kdd_df$feature_name <- as.factor(nsl_kdd_df$feature_name)
nsl_kdd_df$value <- as.double(nsl_kdd_df$value)

# Plot 1 - Creates grouped violen chart for duration feature ...
nsl_kdd_df_subset <- subset(nsl_kdd_df, nsl_kdd_df$feature_name=="duration" & nsl_kdd_df$value > 0.0)

plot1<- ggplot(nsl_kdd_df_subset, aes(x=feature_name, y=value, fill=label)) +
        geom_violin(alpha=0.5, width=0.5) +
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
             y="log(duration)",
             title="Duration") +
        scale_y_continuous(trans='log10') +
        scale_fill_discrete(name="Connection Type", labels=c("Attack", "Normal")) +
        stat_summary(fun.data=data_summary,color="red", position=position_dodge(1.1))
plot1

output_name1 <- "/Users/omarahmed/downloads/feature_1_violin.jpeg"
ggsave(output_name1, plot=plot1, dpi=800, device="jpeg", width=4, height=4)


# Plot 2 - Creates grouped violen chart for src_bytes feature ...
nsl_kdd_df_subset <- subset(nsl_kdd_df, nsl_kdd_df$feature_name=="dst_bytes" & nsl_kdd_df$value > 0.0)

plot2<- ggplot(nsl_kdd_df_subset, aes(x=feature_name, y=value, fill=label)) +
        geom_violin(alpha=0.5, width=0.5) +
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
             y="log(dst_bytes)",
             title="Destination-to-Source Bytes") +
        scale_y_continuous(trans='log10') +
        scale_fill_discrete(name="Connection Type", labels=c("Attack", "Normal")) +
        stat_summary(fun.data=data_summary,color="red", position=position_dodge(1.1))
plot2

output_name2 <- "/Users/omarahmed/downloads/feature_2_violin.jpeg"
ggsave(output_name2, plot=plot2, dpi=800, device="jpeg", width=4, height=4)

# Plot 3 - Creates grouped violen chart for count feature ...
nsl_kdd_df_subset <- subset(nsl_kdd_df, nsl_kdd_df$feature_name=="count" & nsl_kdd_df$value > 0.0)

plot3<- ggplot(nsl_kdd_df_subset, aes(x=feature_name, y=value, fill=label)) +
        geom_violin(alpha=0.5, width=0.5) +
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
             y="log(count)",
             title="Connections to Destination Host") +
        scale_y_continuous(trans='log10') +
        scale_fill_discrete(name="Connection Type", labels=c("Attack", "Normal")) +
        stat_summary(fun.data=data_summary,color="red", position=position_dodge(1.1))
plot3

output_name3 <- "/Users/omarahmed/downloads/feature_3_violin.jpeg"
ggsave(output_name3, plot=plot3, dpi=800, device="jpeg", width=4, height=4)

# Plot 4 - Creates grouped violen chart for num_file_creations feature ...
nsl_kdd_df_subset <- subset(nsl_kdd_df, nsl_kdd_df$feature_name=="dst_host_srv_count" & nsl_kdd_df$value > 0.0)

plot4<- ggplot(nsl_kdd_df_subset, aes(x=feature_name, y=value, fill=label)) +
        geom_violin(alpha=0.5, width=0.5) +
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
             title="Connections to Same Port") +
        scale_y_continuous(trans='log10') +
        scale_fill_discrete(name="Connection Type", labels=c("Attack", "Normal")) +
        stat_summary(fun.data=data_summary,color="red", position=position_dodge(1.1))
plot4

output_name4 <- "/Users/omarahmed/downloads/feature_4_violin.jpeg"
ggsave(output_name4, plot=plot4, dpi=800, device="jpeg", width=4, height=4)


##############################################################
#
# This second section will combine some of the previous 
# feature plots into a single figure with a single legend.
#
###############################################################

figure <- ggarrange(plot1, plot2, plot3, plot4,
                    labels = c("a", "b", "c" ,"d"),
                    ncol = 2, nrow = 2,
                    common.legend = TRUE, legend = "bottom")
figure

output_name_final <- "/Users/omarahmed/downloads/feature_all_violin.jpeg"
ggsave(output_name_final, plot=figure, dpi=800, device="jpeg", width=9, height=9)
