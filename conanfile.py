from conan import ConanFile

class NeolvoxRecipe(ConanFile):

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeConfigDeps"

    default_options = {
        "boost/*:without_cobalt": True,
        "boost/*:without_stacktrace": True,
        "proj/*:build_executables": False,
    }

    def requirements(self):
        self.requires("pdal/2.10.1")
        self.requires("eigen/5.0.1")
        self.requires("hdf5/1.14.6")
        self.requires("gtest/1.18.0")
        self.requires("benchmark/1.9.5")

    def tool_requires(self):
        self.requires("!cmake/*: cmake/[>=3 <4]")
