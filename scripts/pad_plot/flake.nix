{
  description = "A Python pip flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs";
  };

  outputs =
    { self, nixpkgs }:
    let
      pkgs = nixpkgs.legacyPackages.x86_64-linux;
    in
    {
      packages.x86_64-linux.default = pkgs.mkShell {
        packages = with pkgs; [
          python313
          python313Packages.h5py
          python313Packages.matplotlib
        ];
      };
    };
}
