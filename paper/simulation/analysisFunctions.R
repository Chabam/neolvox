library(lidR)

loadPointClouds <- function(dir, scanCount) {
  lapply(seq(0, scanCount - 1), function(n) {
    readLAS(file.path(dir, paste0("leg00", n, "_points.laz")))
  })
}

loadOrigins <- function(dir, scanCount) {
  lapply(seq(0, scanCount - 1), function(n) {
    rawLine <- readLines(file(file.path(dir, paste0("leg00", n, "_trajectory.txt"))), n=1)
    values <- strsplit(rawLine, " ")
    as.double(unlist(head(values[[1]], n = 3)))
  })
}

findMinCoords <- function(pointClouds) {
  lapply(tls, function(las) {
    c(min(las@data$X),
      min(las@data$Y),
      min(las@data$Z))
  })
}

getBounds <- function(metadataFile) {
  metadata <- read.csv(metadataFile, header = FALSE)
  voxSize <- metadata[metadata$V1 == "voxel_size_m",]$V2
  lowestPoint <- c(
    metadata[metadata$V1 == "first_voxel_x",]$V2,
    metadata[metadata$V1 == "first_voxel_y",]$V2,
    metadata[metadata$V1 == "first_voxel_z",]$V2
  ) * voxSize
  highestPoint <- lowestPoint +
    c(
      metadata[metadata$V1 == "dimension_x",]$V2,
      metadata[metadata$V1 == "dimension_y",]$V2,
      metadata[metadata$V1 == "dimension_z",]$V2) * voxSize
  list(lowestPoint, highestPoint)
}

getReferenceVoxels <- function(voxelValuesFile, metadataFile) {
  metadata <- read.csv(metadataFile, header = FALSE)
  lowestPoint <- c(metadata[metadata$V1 == "first_voxel_x",]$V2,
                   metadata[metadata$V1 == "first_voxel_y",]$V2,
                   metadata[metadata$V1 == "first_voxel_z",]$V2)
  dims <- c(metadata[metadata$V1 == "dimension_x",]$V2,
            metadata[metadata$V1 == "dimension_y",]$V2,
            metadata[metadata$V1 == "dimension_z",]$V2)
  refData <- read.csv(paste0(voxelValuesFile))
  refData <- data.frame(refData)
  refDataAll <- aggregate(list(area = refData$area), FUN = sum, by = refData[c("X", "Y", "Z")])

  dense_grid <- array(0, dim = dims)
  xs <- refDataAll$X + 1 - lowestPoint[1]
  ys <- refDataAll$Y + 1 - lowestPoint[2]
  zs <- refDataAll$Z + 1 - lowestPoint[3]
  dense_grid[cbind(xs, ys, zs)] <- refDataAll$area
  dense_grid
}

computeHeightProfileRef <- function(denseGrid, metadataFile) {
  metadata <- read.csv(metadataFile, header = FALSE)
  minZ <- metadata[metadata$V1 == "first_voxel_z",]$V2
  zDim <- metadata[metadata$V1 == "dimension_z",]$V2
  voxSize <- metadata[metadata$V1 == "voxel_size_m",]$V2

  denseGrid[is.nan(denseGrid)] <- 0
  denseGrid[is.infinite(denseGrid)] <- 0
  profile_real_height <- seq(0, zDim - 1) * voxSize + minZ
  profile <- apply(denseGrid, MARGIN = 3, FUN = mean)

  structure(list(Heights = profile_real_height, PAD = profile), class = "LVoxHeightProfile")
}
