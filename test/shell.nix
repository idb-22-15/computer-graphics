with import <nixpkgs> { };

stdenv.mkDerivation {
  name = "term4-lab1";
  buildInputs = [ glfw xorg.libX11 xorg.libXrandr ];
}
