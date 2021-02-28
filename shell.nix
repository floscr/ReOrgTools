
let
  pkgs = (import ./nix/sources.nix {});
  ocamlPackages = pkgs.ocaml-ng.ocamlPackages_4_06;
in
  with pkgs;

  mkShell {
    buildInputs = with ocamlPackages; [
      bs-platform
      nodejs-14_x
      yarn
      merlin
      reason
      python3
      ocamlformat
      httpie
      entr
      jq
    ];

    BSB_PATH =
      if stdenv.isDarwin
      then "${bs-platform}/darwin"
      else "${bs-platform}/linux";
  }

