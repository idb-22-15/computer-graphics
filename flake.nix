{
  description = "term4-lab1";

  inputs = { nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable"; };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in {
      devShells.${system}.default = pkgs.mkShell {
        name = "term4-lab1";
        buildInputs = with pkgs; [
          libGL
          libGLU
          freeglut
          xorg.libX11
          xorg.libXrandr
        ];
        shellHook = "echo term4-lab1";
      };

    };
}
