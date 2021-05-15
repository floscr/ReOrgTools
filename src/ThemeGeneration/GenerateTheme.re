open Relude.Globals;

[@bs.val] external __dirname: string = "";

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
    Array.foldLeft(
      (acc, {varName, cssVarName}) => {
        let (i, acc) = acc;
        let i = Int.add(1, i);
        (i, String.concat(acc, {j|let $varName$i = "$cssVarName-$i";\n|j}));
      },
      (0, ""),
      xs,
    )
    |> (((_, xs)) => xs);

  let makeThemeAssignments = xs =>
    Array.foldLeft(
      (acc, {varName, value}) => {
        let (i, acc) = acc;
        let i = Int.add(1, i);
        (
          i,
          String.concat(
            acc,
            {j|let $varName$i = (ThemeKeys.$varName$i, "$value");\n|j},
          ),
        );
      },
      (0, ""),
      xs,
    )
    |> (((_, xs)) => xs);
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

let gray =
  theme.colors.baseGray
  |> Array.reverse
  |> makeColorPalette(
       ~varName="baseGray",
       ~cssVarName=String.concat("--theme-"),
     )
  |> (xs => (Palette.makeThemeKeys(xs), Palette.makeThemeAssignments(xs)));

let themeKeysFile =
  [gray] |> List.map(((x, _)) => x) |> List.String.joinWith("\n");
let lightThemeFile =
  [gray] |> List.map(((_, x)) => x) |> List.String.joinWith("\n");

let doNotEditHeader = "/* This is a generated file, do not edit! */";

let themeKeysFileContent = {j|$doNotEditHeader

let bgColor = "--bg-color";
let textColor = "--text-color";

let blue = "--accent-blue";

$themeKeysFile
let accentMain = "--accent-main";
let focus = "--focus";
|j};

let lightThemeFileContent = {j|$doNotEditHeader

let key = "light";

let bgColor = (ThemeKeys.bgColor, "#fff");
let textColor = (ThemeKeys.textColor, "#333");

$lightThemeFile
let focus = (ThemeKeys.focus, "#127bf6");

let accentMain = (ThemeKeys.accentMain, "#3d97dd");

let blue = (ThemeKeys.blue, "#3d97dd");
|j};

let makeThemesFile = x =>
  Array.concat([|__dirname, "../WebService/themes/"|], [|x|]);

let tapLog2 = (x, y) => {
  Js.log2(x, y);
  y;
};

Node_fs.writeFileSync(
  Node.Path.join("ThemeKeys.re" |> makeThemesFile)
  |> tapLog2("Writing to file: "),
  themeKeysFileContent,
  `utf8,
);

Node_fs.writeFileSync(
  Node.Path.join("LightTheme.re" |> makeThemesFile)
  |> tapLog2("Writing to file: "),
  lightThemeFileContent,
  `utf8,
);
