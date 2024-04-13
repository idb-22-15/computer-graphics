{
  description = "A Nix-flake-based Node.js development environment";

  inputs = { nixpkgs.url = "github:nixos/nixpkgs/nixos-23.11"; };

  outputs = { self, nixpkgs, ... }:
    let
      # system should match the system you are running on
      # system = "x86_64-linux";
      system = "x86_64-darwin";
    in {
      packages."${system}".dev = let
        pkgs = import nixpkgs { inherit system; };
        packages = with pkgs; [ nodejs_20 nodePackages.pnpm nushell ];
      in pkgs.runCommand "dev-shell" {
        # Dependencies that should exist in the runtime environment
        buildInputs = packages;
        # Dependencies that should only exist in the build environment
        nativeBuildInputs = [ pkgs.makeWrapper ];
      } ''
        mkdir -p $out/bin/
        ln -s ${pkgs.nushell}/bin/nu $out/bin/dev-shell
        wrapProgram $out/bin/dev-shell --prefix PATH : ${
          pkgs.lib.makeBinPath packages
        }
      '';
    };
}
