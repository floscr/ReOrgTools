open ReactUtils;

module Styles = {
  open Css;
  open FixedTheme;

  let root =
    style([
      display(flexBox),
      flexDirection(column),
      flexGrow(1.),
      flexShrink(1.),
      justifyContent(center),
      alignItems(center),
    ]);
};
[@react.component]
let make = () => {
  <div className=Styles.root> <Views__Login /> </div>;
};
