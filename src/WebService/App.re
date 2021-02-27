Css.(
  global("*, *:before, *:after", [boxSizing(borderBox)]),
  global("h1, h2, h3, h4, h5, h6", [margin(zero)]),
  global("html, body", [height(pct(100.))]),
  global(
    "body",
    [
      backgroundColor(Theme.LightTheme.bgColor),
      color(Theme.LightTheme.textColor),
      margin(zero),
      fontFamilies([`custom("IBM Plex Sans"), `sansSerif]),
    ],
  ),
  global(".container", [padding(px(20))]),
  global("a", [color(Theme.LightTheme.textColor)]),
  global("a:hover", [textDecoration(none)]),
);

[@react.component]
let make = () => <Page />;
