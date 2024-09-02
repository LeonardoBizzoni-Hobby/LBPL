{ pkgs ? import <nixpkgs> { } }:

pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    man-pages
    man-pages-posix

    gnumake
    cmake
    seer
    clang-tools
    cmake-language-server
    tree-sitter-grammars.tree-sitter-cpp
    tree-sitter-grammars.tree-sitter-c
  ];

  shellHook = ''
    ${pkgs.onefetch}/bin/onefetch
  '';
}
