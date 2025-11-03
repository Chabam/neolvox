\name{lvoxR-package}
\alias{lvoxR-package}
\alias{lvoxR}
\docType{package}
\title{
  lvoxR is a voxel-based estimator for the Plant Area Density (PAD) using
  lidar data acquired in forest plots.
}
\description{
  LVox computes the path for every points in a point cloud to find which
  voxels intersects with it. Every important metrics concerning the
  paths and the count of returns per voxel is computed. With these
  metrics, it is then able to estimate the PAD for each voxel. The PAD
  can be used better understand the distribution of forest elements in 3D.

  It supports two scanner types:
  - terrestrial laser scanners (TLS)
  - Mobile laser scanners (MLS)

  The inputs of LVox are points cloud and the output is a sparse matrix
  containing the voxel data and it's related metadata (voxel size, grid
  dimensions, minimal coordinates, etc.)
}
\details{
  lvoxR provides two main functions:

  - lvoxComputeMLS
  - lvoxComputeTLS

  For point cloud representation, it is compatible with the LAS objects
  from the lidR package. Otherwise, it also accepts an R list object
  with these following named elements:

  - X: a vector of double values representing the X coordinates for each
  points in the point cloud
  - Y: a vector of double values representing the Y coordinates for each
  points in the point cloud
  - Z: a vector of double values representing the Z coordinates for each
  points in the point cloud
  - gpstime: a vector of double values representing the gps time for each
  points in the point cloud



}
\author{
  Maintainer: Félix Chabot <fchabot1337@gmail.com>
}
\references{
  \itemize{
    \item \href{https://doi.org/10.1016/j.rse.2022.113115}{First
    implementation of LVox in Computree (Nguyen et al. 2022)}

    \item \href{https://doi.org/10.3390/rs11131580}{Mathematical
    framework used by LVox (Pimont et al. 2019)}

    \item \href{https://doi.org/10.2312/egtp.19871000}{Path tracing
    algorithm used by LVox (Amanatides et Woo 1987)}
}
}
\keyword{ package, lvox, PAD }
\seealso{
}
\examples{
  \dontrun{
library(lidR)
library(lvoxR)

# MLS processing ===============================================

# lvoxR can work with LAS objects
mls <- readLAS("/path/to/a/las_file.las")
traj <- read.csv("/path/to/a/traj_file.txt", skip = 1, sep = " ")

# Otherwise you can provide it as list with these specific names
names(traj) <- c("gpstime", "X", "Y", "Z")

# See the documentation for the available computation options
pad <- lvoxComputeMLS(mls, traj, voxelSize = 0.4)

# TLS processing (with multi-scans) ============================
readASCIIPointCloud <- function(filename)
{
  read.csv(filename, skip = 1, sep = " ")
}

# Creating a list of scans with the appropriate format
scans <- lapply(
  list("/path/to/a/ascii_point_cloud_scan_0.txt",
       "/path/to/a/ascii_point_cloud_scan_1.txt",
       "/path/to/a/ascii_point_cloud_scan_2.txt",
       "/path/to/a/ascii_point_cloud_scan_3.txt",
       "/path/to/a/ascii_point_cloud_scan_4.txt"),
 function(filename) {
   pc <- read.csv(filename, skip = 1, sep = "\t")
   names(pc) <- c("ID", "X", "Y", "Z")
   pc$gpstime <- 0
   pc
}

)

# To trace the path of the rays, LVox needs the origin of the
# scanner. This list must be the same size as the amount of scans.
scanner_origins <- list(
  c(0, 0, 94.131491),
  c(0.506694, 12.121184, 93.97213),
  c(11.452984, -1.91865, 93.727236),
  c(-0.821686, -10.803126, 94.414889),
  c(-11.340925, 2.670768, 94.75565)
)

pad <- lvoxComputeTLS(scans, scanner_origins)
  }
}
