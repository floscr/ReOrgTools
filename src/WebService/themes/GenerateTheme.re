open Relude.Globals;

type colors = {baseGray: array(string)};

type t = {colors};

module Palette = {
  type t = {
    varName: string,
    cssVarName: string,
    value: string,
  };

  let make = (varName, cssVarName, value) => {varName, cssVarName, value};

  let makeThemeKeys = xs =>
    xs
    |> Array.foldLeft(
         (acc, {varName, cssVarName}) => {
           let (i, _) = acc;
           let i = Int.add(1, i);
           (i, {j|let $varName$i = "$cssVarName-$i";\n|j});
         },
         (0, ""),
       )
    |> ((_, xs) => xs);
};

[@bs.module "./theme.js"] external theme: t = "theme";

let makeColorPalette =
    (
      ~varName: string,
      ~cssVarName=String.concat("--theme-"),
      xs: array(string),
    )
    : array(Palette.t) =>
  xs |> Array.map(value => Palette.make(varName, cssVarName(varName), value));

let xs =
  theme.colors.baseGray
  |> makeColorPalette(
       ~varName="baseGray",
       ~cssVarName=String.concat("--theme"),
     );

Js.log(xs);
