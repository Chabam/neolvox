library(lvoxR)
library(lidR)

path <- "~/Documents/Ecole/maitrise/courant/article/simulation/fasy_dense_01_survey/"

loadTLS <- function(directory) {
  lapply(seq(0, 4), function(n) {
    filename <- file.path(directory, paste0("leg00", n, "_points.laz"))
    print(cat("Loading", filename))
    readLAS(filename)
  })
}

tls <- loadTLS(file.path(path, "output"))

minCoords <- lapply(tls, function(las) {
  c(
    min(las@data$X),
    min(las@data$Y),
    min(las@data$Z)
    )
})

refData <- read.csv(paste0(path, "area_metadata.csv"))
refData <- data.frame(refData)
refDataAll <- refData[refData$tag == "all",]

toDenseMatrix <- function(df) {
  dense_grid <- array(0, dim = c(32, 31, 27))
  xs <- df$X - lowestPoint[1]
  ys <- df$Y - lowestPoint[2]
  zs <- df$Z - lowestPoint[3]
  dense_grid[cbind(xs, ys, zs)] <- df$area
  dense_grid
}

virtualVoxels <- toDenseMatrix(refDataAll)
origins <- list(
  c(0.0000, 0.0000, -0.0819),
  c(-5.0000, 5.0000, 0.8898),
  c(5.0000, 5.0000, 0.8908),
  c(-5.0000, -5.0000, -1.5257),
  c(5.0000, -5.0000, -0.9616)
)

minimalCoordinates <- function(las) {
  c(
    min(las@data$X),
    min(las@data$Y),
    min(las@data$Z)
  )
}
minCoords <- lapply(tls, minimalCoordinates)

lowestPoint <- c(-16, -13, -2)
highestPoint <- c(13, 12, 24)

grid <- estimatePADForTLS(
  tls,
  origins,
  padEstimator = "BCMLE",
  voxelSize = 1.0,
  smallestElementArea = needleProjectedAreaFromDimensions(0.06, 0.01),
  bounds = list(lowestPoint, highestPoint),
  threadCount = 12
)

hp <- computeHeightProfile(grid)
hp
plot(hp)
