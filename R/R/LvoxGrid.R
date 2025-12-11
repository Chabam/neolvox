setClass(
  "LvoxGrid",
  slots = list(
    data = "data.frame",
    VoxelSize = "numeric",
    Dimensions = "numeric",
    MinimalCoords = "numeric"
  )
)

setMethod("show", "LvoxGrid", function(object) {
  cat("LvoxGrid object\n")
  cat("Dimensions (x, y, z): ", paste(object@Dimensions, collapse = "x"), "\n")
  cat("Total voxels: ", prod(object@Dimensions), "\n")
  cat("Voxel size: ", prod(object@VoxelSize), "\n")
})

setGeneric("computeHeightProfile", function(object) {
  standardGeneric("computeHeightProfile")
})

setMethod("computeHeightProfile", "LvoxGrid", function(object) {
  pad <- object@data$PAD
  pad[is.nan(pad)] <- 0
  pad[is.infinite(pad)] <- 0

  z_count <- object@Dimensions[3] - 1

  profile <- sapply(seq(0, z_count), function(z) {
    mean(object@data$PAD[object@data$Z == z])
  })
  profile_real_height <- sapply(seq(0, z_count), function(h) {
    h * object@VoxelSize
  })

  new("LvoxHeightProfile", Heights = profile_real_height, PAD = profile)
})
