library(lidR)
library(lvoxR)

tls <- list(
  readLAS("~/Documents/Lidar/PRF133/TLS/Petawawa_133_Scan_000.laz")## ,
  ## readLAS("~/Documents/Lidar/PRF133/TLS/Petawawa_133_Scan_001.laz"),
  ## readLAS("~/Documents/Lidar/PRF133/TLS/Petawawa_133_Scan_002.laz"),
  ## readLAS("~/Documents/Lidar/PRF133/TLS/Petawawa_133_Scan_003.laz"),
  ## readLAS("~/Documents/Lidar/PRF133/TLS/Petawawa_133_Scan_004.laz")
)

origins <- list(
  c(0, 0, 94.131491)## ,
  ## c(0.506694, 12.121184, 93.97213),
  ## c(11.452984, -1.91865, 93.727236),
  ## c(-0.821686, -10.803126, 94.414889),
  ## c(11.340925, 2.670768, 94.75565)
)
grid <- lvoxComputeTLS(tls, origins)
dense_grid <- as.matrix(grid)
dense_grid[is.nan(dense_grid)] <- 0
dense_grid[is.infinite(dense_grid)] <- 0
profile_real_height <- seq(0, grid$Dimensions[3] - 1) * grid$VoxelSize + grid$MinimalCoords[3]

profile <- apply(dense_grid, MARGIN = c(1,2), FUN = mean)
grid <- lvoxComputeTLS(tls, origins, threadCount = 12)
hp <- computeHeightProfile(grid)
hp
plot(hp)
