print.LVoxGrid <- function(x, ...) {
  cat("LVoxGrid\n")
  cat("Dimensions (x, y, z): ", paste(x$Dimensions, collapse = "x"), "\n")
  cat("Total voxels: ", prod(x$Dimensions), "\n")
  cat("Voxel size: ", prod(x$VoxelSize), "\n")
}

as.matrix.LVoxGrid <- function(x, ...) {
  dense_grid <- array(0, dim = x$Dimensions)
  xs <- x$data$X + 1
  ys <- x$data$Y + 1
  zs <- x$data$Z + 1
  dense_grid[cbind(xs, ys, zs)] <- x$data$PAD
  dense_grid
}

#' @description Computes the mean PAD value for each vertical layer of the grid
#' @param x A LVoxGrid object
#' @return A LvoxHeightProfile object
computeHeightProfile <- function(lvox_grid) {
  if (!inherits(lvox_grid, "LVoxGrid"))
    error("Input must be an LVoxGrid object")
  dense_grid <- as.matrix(lvox_grid)
  profile_real_height <- seq(0, lvox_grid$Dimensions[3] - 1) * lvox_grid$VoxelSize + lvox_grid$MinimalCoords[3]
  profile <- apply(dense_grid, MARGIN = 3, FUN = mean)

  structure(list(Heights = profile_real_height, PAD = profile), class = "LVoxHeightProfile")
}
