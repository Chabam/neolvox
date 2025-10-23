library(hdf5r)
library(ggplot2)

create_profile <- function(file) {
  grp <- file[["lvox"]]

  dims <- h5attr(grp, "Dimensions")
  vox_size <- h5attr(grp, "Voxel size")

  x <- readDataSet(grp[["x"]])
  y <- readDataSet(grp[["y"]])
  z <- readDataSet(grp[["z"]])
  pad <- readDataSet(grp[["pad"]])

  df <- data.frame(x = x, y = y, z = z, pad = pad)

  df$pad[is.nan(df$pad)] <- 0

  profile <- sapply(seq(0, dims[3] - 1), function(z)
    mean((df$pad[df$z == z]))
    )

  profile_real_height <- sapply(seq(0, dims[3] - 1), function(h) h * vox_size)
  df <- data.frame(height = profile_real_height, pad = profile)
  df$voxel_size <- as.factor(vox_size)
  df
}

profile_50cm <- create_profile(h5file("cook_bcmle_50CM.h5", mode = "r"))
profile_30cm <- create_profile(h5file("cook_bcmle_30CM.h5", mode = "r"))

profile_df <- rbind(profile_30cm, profile_50cm)

ggplot(profile_df, aes(x = height, y = pad, color = voxel_size)) +
  geom_line() +
  scale_x_continuous(labels = function(x) paste0(x, "m")) +
  labs(title = "PAD for a given plot",
       x = "Height",
       y = "PAD (m³ . m²)") +
  coord_flip()
