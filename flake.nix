{
  description = "InputActions overlay";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { nixpkgs, ... }:
    let
      inherit (nixpkgs) lib;
      systems = [
        "aarch64-linux"
        "x86_64-linux"
      ];
      perSystem =
        f:
        lib.genAttrs systems (
          system:
          f {
            pkgs = import nixpkgs { inherit system; };
          }
        );
    in
    {
      packages = perSystem (
        { pkgs }:
        rec {
          inputactions-overlay = pkgs.kdePackages.callPackage ./nix/package.nix { };
          default = inputactions-overlay;
        }
      );
    };
}
