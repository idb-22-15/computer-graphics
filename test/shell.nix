{ pkgs ? import <nixpkgs> { } }:

with pkgs;

mkShell {
  name = "term4-lab1";
  buildInputs = [ libGL libGLU freeglut xorg.libX11 xorg.libXrandr ];
  shellHook = "echo term4-lab1";
}
