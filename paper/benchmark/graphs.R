library(readODS)
library(lubridate)
library(dplyr)
library(ggplot2)
library(hms)
library(scales)

# Comparaison avec Computree

lire_temps_cmd_time <- function(time_str) {
  if (is.na(time_str)) {
    return(NA)
  }

  parties <- unlist(strsplit(time_str, ":"))

  if (length(parties) == 3) {
# "H:M:S.ms"
    h <- as.numeric(parties[1])
    m <- as.numeric(parties[2])
    s <- as.numeric(parties[3])
  } else if (length(parties) == 2) {
# "M:S.ms"
    h <- 0
    m <- as.numeric(parties[1])
    s <- as.numeric(parties[2])
  } else {
    stop("Invalid time format", time_str)
  }

  dhours(h) + dminutes(m) + dseconds(s)
}

file <- read_ods("./analysis.ods")

temps_lvox_sparse <- sapply(file[[4]][-1], lire_temps_cmd_time)
mem_gb_lvox_sparse <- as.double(file[[5]][-1]) / (1000 * 1000)

temps_lvox_dense <- sapply(file[[7]][-1], lire_temps_cmd_time)
mem_gb_lvox_dense <- as.double(file[[8]][-1]) / (1000 * 1000)

temps_computree <- sapply(file[[10]][-1], lire_temps_cmd_time)
mem_gb_computree <- as.double(file[[11]][-1]) / (1000 * 1000)

taille_voxels <- rep(c(0.5, 0.3, 0.1), 5, each = 3)
placettes_types <- c(
  rep("TLS multi-scan", 9),
  rep("Fake TLS", 27),
  rep("MLS", 9)
)
placettes <- paste(rep(na.omit(file$Placette), each = 9), placettes_types)
programme <- rep(c("Computree", "LVox2 Sparse", "LVox2 Dense"), 15)
temps <- as.vector(rbind(temps_computree, temps_lvox_sparse, temps_lvox_dense))
memoire <- as.vector(
  rbind(
    mem_gb_computree,
    mem_gb_lvox_sparse,
    mem_gb_lvox_dense
  ))

df <- data.frame(
  Plot = placettes,
  VoxelSize = as.factor(taille_voxels),
  Program = programme,
  Time = temps,
  Memory = memoire,
  row.names = NULL
)

df <- df[df$Plot != "MLS_W01 Fake TLS", ]

max_time_df <- df %>%
  group_by(VoxelSize, Program) %>%
  summarise(
    max_time = max(Time),
    mean_time = mean(Time),
    .groups = "drop"
  ) %>%
  filter(max_time > 1800)

format_time <- function(time) {
  sprintf("%02dh%02dm",
          hour(seconds_to_period(time)),
          minute(seconds_to_period(time)))
}

temps_graph <- ggplot(df, aes(x = VoxelSize, y = Time, fill = Program)) +
  geom_boxplot(position = position_dodge(width = 0.8)) +
  coord_cartesian(ylim = c(0, 60 * 30)) +
  geom_hline(yintercept = 1888, linetype = "dashed", color = "red") +
  geom_text(data = max_time_df,
            aes(x = as.numeric(factor(VoxelSize)) + 0.2, y = 1800, group = Program,
                label = paste0("Max: ", format_time(max_time), "\nMean: ", format_time(mean_time))),
            size = 18, size.unit = "pt",
            position = position_dodge(width = 0.8), color = scales::hue_pal()(1)) +
  scale_y_time(
    labels = label_time(format = "%Mm")
  ) +
  labs(
    subtitle = "Computation Time by voxel size\nand program over varied point clouds",
    x = "Voxel size in meters",
    y = "Time"
  )

mem_graph <-
  ggplot(df, aes(x = VoxelSize, y = Memory, fill = Program)) +
  geom_boxplot() +
  scale_y_continuous(
    labels = function(val) {
      paste(val, "GB")
    }
  ) +
  labs(
    title = "Memory consumption by voxel size\nand program over varied point clouds",
    x = "Voxel size in meters",
    y = "Memory used in Gigabytes"
  )

real_size <- 9.5
real_width <- real_size
real_height <- real_size
ggsave(
  "time_plot.png",
  plot = temps_graph
)
ggsave(
  "mem_plot.png",
  plot = mem_graph
)

# Benchmarks
bench_data <- read.csv("./benchmarks.csv", skip = 9, header = TRUE)
traiter_nom_test <- function(nom) {
  parties <- unlist(strsplit(nom, "/"))
  c(parties[1], parties[2], as.integer(parties[3]))
}
def_test <- t(sapply(bench_data$name, traiter_nom_test))
bench_df <- data.frame(cbind(def_test, bench_data$real_time), row.names = NULL)
colnames(bench_df) <- c("Test", "GridType", "Metric", "Time")
bench_df$GridType <- as.factor(bench_df$GridType)
bench_df$Metric <- as.numeric(bench_df$Metric)
bench_df$Time <- dnanoseconds(as.numeric(bench_df$Time))

point_cloud_df <- bench_df[bench_df$Test == "bm_point_cloud_size",]
job_df <- bench_df[bench_df$Test == "bm_job_count", ]

point_cloud_graph <-
  ggplot(
    point_cloud_df,
    aes(x = Metric, y = Time, group = GridType, color = GridType)
  ) +
  geom_smooth(method = "lm", alpha = 0.3) +
  geom_point(color = "black") +
  scale_y_continuous(
    trans = "log10",
    labels = scales::label_timespan()
  ) +
  scale_x_log10(labels = label_number(scale = 1e-3, suffix = "K")) +
  labs(
    title = "Processing time for variable\npoint clouds with 16 threads",
    x = "Points in point cloud",
    y = "Processing time"
  )

thread_graph <-
  ggplot(
    job_df,
    aes(x = Metric, y = Time, group = GridType, color = GridType)
  ) +
  geom_point(color = "black") +
  scale_y_continuous(
    labels = scales::label_timespan()
  ) +
  labs(
    title = "Processing time for a fixed\npoint cloud of one million points",
    x = "Thread count",
    y = "Processing time"
  )

ggsave(
  "point_cloud_graph.png",
  plot = point_cloud_graph
)
ggsave(
  "job_graph.png",
  plot = thread_graph
)
