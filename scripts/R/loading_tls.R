library(lidR)
library(lvoxR)

tls <- list(readLAS("~/Documents/Données lidar/ALS/SG_IR057.las"))
origins <- list(c(0,0,0))

grid <- lvoxComputeTLS(tls, origins, threadCount = 12)
hp <- computeHeightProfile(grid)
hp
plot(hp)
