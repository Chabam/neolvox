print.LVoxHeightProfile <- function(x, ...) {
  print(cbind(paste0(rev(x$Heights), "m"), rev(x$PAD)))
}

plot.LVoxHeightProfile <- function(x, y = NULL, ..., main = NULL) {
  plot(x$PAD, x$Heights,
       type = "l",
       xlab = "PAD (m^3 . m^2)",
       ylab = "Height",
       main = "PAD for a given plot",
       axes = FALSE)

  axis(2, at = x$Heights, labels = paste0(x$Heights, "m"))
  axis(1)
  box()
}
