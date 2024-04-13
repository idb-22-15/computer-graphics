with import <nixpkgs> { };

stdenv.mkDerivation {
  name = "term4-lab1";
  src = ./.;

  buildInputs = [ libGL glfw xorg.libX11 xorg.libXrandr ];

  buildPhase = "g++ main.cpp -o main -lglfw -lGL -lX11  -lXrandr";

  installPhase = ''
    mkdir -p $out/bin
    cp main $out/bin/
  '';
}
