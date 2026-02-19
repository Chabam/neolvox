print.LVoxGrid <- function(x, ...) {
  cat("LVoxGrid\n")
  cat("Dimensions (x, y, z): ", paste(x$Dimensions, collapse = "x"), "\n")
  cat("Total voxels: ", prod(x$Dimensions), "\n")
  cat("Voxel size: ", prod(x$VoxelSize), "\n")
}


#' @description Creates a dense matrix for a given LVoxGrid object
#'   (which is sparse)
#' @param x A LVoxGrid object
#' @param gridDataName A string representing the data to get from the
#'   grid ("PAD" by default, look at `colnames(x$data)` for the
#'   available data types)
#' @return A LvoxHeightProfile object
as.matrix.LVoxGrid <- function(x, gridDataName = "PAD", ...) {
  dense_grid <- array(0, dim = x$Dimensions)
  xs <- x$data$X + 1 - x$MinimalIndices[1]
  ys <- x$data$Y + 1 - x$MinimalIndices[2]
  zs <- x$data$Z + 1 - x$MinimalIndices[3]
  dense_grid[cbind(xs, ys, zs)] <- x$data[[gridDataName]]
  dense_grid
}

#' @description Computes the mean PAD value for each vertical layer of the grid
#' @param x A LVoxGrid object
#' @return A LvoxHeightProfile object
computeHeightProfile <- function(lvox_grid) {
  if (!inherits(lvox_grid, "LVoxGrid"))
    error("Input must be an LVoxGrid object")
  dense_grid <- as.matrix(lvox_grid)
  dense_grid[is.nan(dense_grid)] <- 0
  dense_grid[is.infinite(dense_grid)] <- 0
  profile_real_height <- seq(0, lvox_grid$Dimensions[3] - 1) * lvox_grid$VoxelSize + lvox_grid$MinimalCoords[3]
  profile <- apply(dense_grid, MARGIN = 3, FUN = mean)

  structure(list(Heights = profile_real_height, PAD = profile), class = "LVoxHeightProfile")
}
