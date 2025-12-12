print.LVoxGrid <- function(x, ...) {
  cat("LVoxGrid\n")
  cat("Dimensions (x, y, z): ", paste(x$Dimensions, collapse = "x"), "\n")
  cat("Total voxels: ", prod(x$Dimensions), "\n")
  cat("Voxel size: ", prod(x$VoxelSize), "\n")
}

computeHeightProfile <- function(x) UseMethod("computeHeightProfile", x)

computeHeightProfile.default <- function(x) {
  warning("Must be used with an LVoxGrid x")
  x
}

#' @description Computes the mean PAD value for each vertical layer of the grid
#' @param x A LVoxGrid object
#' @return A LvoxHeightProfile object
computeHeightProfile.LVoxGrid <- function(grid) {
  pad <- grid$data$PAD
  pad[is.nan(pad)] <- 0
  pad[is.infinite(pad)] <- 0

  z_count <- grid$Dimensions[3] - 1

  profile <- sapply(seq(0, z_count), function(z) {
    mean(grid$data$PAD[grid$data$Z == z])
  })
  profile_real_height <- seq(0, z_count) * grid$VoxelSize + grid$MinimalCoords[3]

  structure(list(Heights = profile_real_height, PAD = profile), class = "LVoxHeightProfile")
}
