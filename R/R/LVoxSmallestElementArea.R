#' @description Helper function for computing the projected area of a
#'   single needle from a coniferous tree from its dimensions. This
#'   function can be used for the `smallestElementArea` parameter of
#'   `lvoxR::computeTLS` and `lvoxR::computeMLS`
#' @param length The length of the needle expressed in meters
#' @param diameter The diameter of the needle expressed in meters
#' @return The random projected area of the needle
needleProjectedAreaFromDimensions <- function(length, diameter) {
  (2 * pi * length * diameter) / 4
}

#' @description Helper function for computing the projected area of a
#'   single needle from a coniferous tree from its flat area. This
#'   function can be used for the `smallestElementArea` parameter of
#'   `lvoxR::computeTLS` and `lvoxR::computeMLS`
#' @param flatArea The flat area of the needle expressed in meters
#' @return The random projected area of the needle
needleProjectedAreaFromFlatArea <- function(flatArea) {
  (pi * flatArea) / 4
}

#' @description Helper function for computing the projected area of a
#'   broad leaf from a deciduous tree from its dimensions. This
#'   function can be used for the `smallestElementArea` parameter of
#'   `lvoxR::computeTLS` and `lvoxR::computeMLS`
#' @param length The length of the broad leaf expressed in meters
#' @param diameter The diameter of the broad leaf expressed in meters
#' @return The random projected area of the broad leaf
broadLeafProjectedAreaFromDimensions <- function(length) {
  (2 * pi * (length ** 2)) / 4
}

#' @description Helper function for computing the projected area of a
#'   broad leaf from a deciduous tree from its flat area. This
#'   function can be used for the `smallestElementArea` parameter of
#'   `lvoxR::computeTLS` and `lvoxR::computeMLS`
#' @param flatArea The flat area of the broad leaf expressed in meters
#' @return The random projected area of the broad leaf
needleProjectedAreaFromFlatArea <- function(flatArea) {
  (2 * flatArea) / 4
}
