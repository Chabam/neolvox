{
  description = "Computree";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs =
    { nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
      scriptDeps = pkgs.python3.withPackages (pkgs: with pkgs; [
              napari
              matplotlib
              numpy
              h5py
              anyqt
            ]);
    in
      {
        devShells.x86_64-linux.default = pkgs.mkShell {
          nativeBuildInputs = with pkgs; [
            # Tooling
            gcc
            gdb
            cmake
            valgrind

            # Dependencies
            eigen
            pdal
            gtest
            hdf5-cpp

            # Scripts
            scriptDeps
            qt5.qtbase
          ];
          QT_QPA_PLATFORM_PLUGIN_PATH = with pkgs; "${qt5.qtbase.bin}/lib/qt-${qt5.qtbase.version}/plugins";
        };
      };
}
