Css.(
  global(
    "body",
    [
      backgroundColor(Theme.LightTheme.bgColor),
      color(Theme.LightTheme.textColor),
      margin(px(0)),
    ],
  ),
  global("a", [color(Theme.LightTheme.textColor)]),
  global("a:hover", [textDecoration(none)]),
);

[@react.component]
let make = () => <Page />;
