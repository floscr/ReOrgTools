open Relude.Globals;
open ReOrga;
open ReactUtils;
open OrgDocument__Utils;

module Styles = {
  open Css;
  open FixedTheme;
  let root =
    style([
      position(sticky),
      top(zero),
      padding(FixedTheme.Spacing.large),
      borderBottom(px(1), solid, var(ThemeKeys.grey10)),
      flexShrink(1.),
      flexGrow(1.),
      backgroundColor(var(ThemeKeys.bgColor)),
    ]);
};

[@react.component]
let make = (~ast: ReOrga.orgAst, ~queryParams: Types__URLSearchParams.t) => {
  <header className=Styles.root> {"Hello" |> s} </header>;
};
