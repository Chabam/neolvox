library(lidR)
library(lvoxR)

tls <- list(readLAS("~/Documents/Données lidar/ALS/SG_IR057.las"))
origins <- list(c(0,0,0))

data <- lvoxComputeTLS(tls, origins, threadCount = 12)
