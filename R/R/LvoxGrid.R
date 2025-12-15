print.LVoxGrid <- function(x, ...) {
  cat("LVoxGrid\n")
  cat("Dimensions (x, y, z): ", paste(x$Dimensions, collapse = "x"), "\n")
  cat("Total voxels: ", prod(x$Dimensions), "\n")
  cat("Voxel size: ", prod(x$VoxelSize), "\n")
}
#' @description Computes the mean PAD value for each vertical layer of the grid
#' @param x A LVoxGrid object
#' @return A LvoxHeightProfile object
computeHeightProfile <- function(lvox_grid) {
  if (!inherits(lvox_grid, "LVoxGrid"))
    error("Input must be an LVoxGrid object")
  pad <- lvox_grid$data$PAD
  pad[is.nan(pad)] <- 0
  pad[is.infinite(pad)] <- 0

  z_count <- lvox_grid$Dimensions[3] - 1

  profile <- sapply(seq(0, z_count), function(z) {
    mean(lvox_grid$data$PAD[lvox_grid$data$Z == z])
  })
  profile_real_height <- seq(0, z_count) * lvox_grid$VoxelSize + lvox_grid$MinimalCoords[3]

  structure(list(Heights = profile_real_height, PAD = profile), class = "LVoxHeightProfile")
}
