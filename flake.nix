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
        ];
      };
    };
}
