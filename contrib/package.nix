{ stdenv, libxcb, xcbutil, xcbutilwm, libconfig, pcre }:

# this package can be built with
# `nix-build -E 'with import <nixpkgs> {}; callPackage ./package.nix {}'`

stdenv.mkDerivation rec {
  name = "custard-git";

  src = ../.;

  buildInputs = [ libxcb xcbutil xcbutilwm libconfig pcre ];

  makeFlags = [
    "PREFIX="
    "DESTDIR=$(out)"
  ];
}
