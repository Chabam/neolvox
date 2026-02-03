---
title: 'Neolvox: a portable high performance library for estimating vegetation 3D distribution from point clouds'
tags:
  - C++
  - R
  - Forestry
  - Lidar
  - Python
authors:
  - name: Félix Chabot
    equal-contrib: true
    affiliation: 1
  - name: Richard A. Fournier
    affiliation: 2
  - name: Toby Dylan Hocking
    affiliation: 1
  - name: Johannie Lemelin
    affiliation: 2
  - name: Camille Rouet
    affiliation: 2
  - name: Amélie Juckler
    affiliation: 2
affiliations:
 - name: Université de Sherbrooke, Département d'informatique, Sherbrooke, Québec, Canada
   index: 1
 - name: Université de Sherbrooke, Département de géomatique appliquée, Sherbrooke, Québec, Canada
   index: 2

date: TBD
bibliography: paper.bib
---

# Summary
Neolvox is the evolution of an existing software called
LVox[nguyen2022]. Its goal is to provide an accurate estimation of the
3D distribution of vegetation elements from lidar data. This data is
obtained from terrestrial (TLS) or mobile lidar (MLS) in forest
plots. It uses a theoretical framework[pimont2019] that offers
unbiased estimations based on the inversion of Beer-Lambert's law and
the contact frequency. This approach is empirical, the points in the
point clouds are viewed as statistical events in a discrete volume. To
do the actual computation, we need to find out how much a voxel was
explored compared to how many points it contains. To do that, we trace
back the path that the lasers took within the voxelized scene,
computing the total laser length per voxel. This approach is
fundamental to obtain estimations that are less affected by the
occlusion of the laser beams, a major problem when working in forest
environment. The input of Neolvox are point clouds and its output is a
3D voxel grid of a metric called plant area density (PAD), the amount
of vegetation surface area inside a volume. This metric can then be
used to better understand the spatial distribution of the forest.

# Statement of need
The original version of LVox was developed in C++ as a plugin inside
the Computree[computree] platform. While this platform is great for
research, it is not really suited for operational use. Computree uses
a graphical user interface, this makes it difficult to process large
amount of forest plots. Users have to do a lot of manual operations to
select the point cloud files and fill in the proper parameters within
the interface. This is why the forestry field leans more towards the
use of programming language libraries to define pipelines for handling
large datasets. The main one being lidR[roussel2020], an R package
offering various algorithms for processing point clouds. Although,
unlike Neolvox, its main area of focus is airborne lidar data. The
goal of Neolvox is to provide the same capabilities of LVox, but as an
easy-to-use library that can be called from a programming language.

# State of the field
Few other software solutions exist that provides a simple and
efficient interface to process MLS and TLS data for PAD
estimation. The main ones being the original version of LVox,
AMAPvox[amapvox], LidarForFuel[martin2025] and VoxLAD-RT[CITE].

À Faire:
| Software     | Performance | Simplicity | Interoperability |
|--------------|-------------|------------|------------------|
| Neolvox      |             |            |                  |
| Lvox         |             |            |                  |
| LidarForFuel |             |            |                  |
| VoxLAD-RT    |             |            |                  |

The key difference that Neolvox offers is that it is aimed for
operational use first. It is based on established theoretical methods,
but simplifies the process of using them greatly by fixing default
parameters while still offering the option to tweak them. Secondly, it
is also meant to be used within other processing pipelines, it doesn't
try to reimplement features that could be a part of other
software. Neolvox is very portable, it can trivially be ported to
other languages that offer `C++` bindings. Additionally, the library
uses parallel processing to greatly reduce computing times when
compared to LVox and the other solutions, which is a big benefit when
processing large amount of point clouds.

# Software Design
An explanation of the trade-offs you weighed, the design/architecture
you chose, and why it matters for your research application. This
should demonstrate meaningful design thinking beyond a superficial
code structure description.

# Research Impact Statement
Evidence of realized impact (publications, external use, integrations)
or credible near-term significance (benchmarks, reproducible
materials, community-readiness signals). The evidence should be
compelling and specific, not aspirational.

# AI usage disclosure
Transparent disclosure of any use of generative AI in the software
creation, documentation, or paper authoring. If no AI tools were used,
state this explicitly. If AI tools were used, describe how they were
used and how the quality and correctness of AI-generated content was
verified.
