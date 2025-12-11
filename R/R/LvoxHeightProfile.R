setClass(
  "LvoxHeightProfile",
  slots = list(
    Heights = "numeric",
    PAD = "numeric"
  )
)

setMethod("show", "LvoxHeightProfile", function(object) {
  cbind(paste0(rev(object@Heights), "m"), rev(object@PAD))
})

setMethod("plot", "LvoxHeightProfile", function(x, y = NULL, ..., main = NULL) {
  plot(x@PAD, x@Heights,
       type = "l",
       xlab = "PAD (m³ . m²)",
       ylab = "Height",
       main = "PAD for a given plot",
       axes = FALSE)

  axis(2, at = x@Heights, labels = paste0(x@Heights, "m"))
  axis(1)
  box()
})
