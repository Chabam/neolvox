library(lidR)
library(lvoxR)

mls <- readLAS("~/Documents/Données lidar/PRF133/MLS/MLS-PRF133.laz")
traj <- read.csv("~/Documents/Données lidar/PRF133/MLS/MLS-PRF133-traj.txt", sep = " ", skip = 1)
names(traj) <- c("gpstime", "X", "Y", "Z", "Q0", "Q1", "Q2", "Q3", "Red", "Green", "Blue", "NormalX", "NormalY", "NormalZ", "Pitch", "Roll", "Azimuth")

grid <- lvoxComputeMLS(mls, traj, threadCount = 12)
hp <- computeHeightProfile(grid)
hp
plot(hp)
