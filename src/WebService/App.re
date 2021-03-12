open ReactUtils;

Css.(
  global("*, *:before, *:after", [boxSizing(borderBox)]),
  /* button reset */
  global(
    "button",
    [
      border(`zero, `none, `transparent),
      margin(zero),
      padding(zero),
      width(`auto),
      overflow(visible),
      background(transparent),
      unsafe("color", "inherit"),
      unsafe("font", "inherit"),
      display(block),
      lineHeight(normal),
    ],
  ),
  global(
    "body",
    [
      backgroundColor(Theme.LightTheme.bgColor),
      color(Theme.LightTheme.textColor),
      fontFamilies([`custom("IBM Plex Sans"), `sansSerif]),
      lineHeight(`abs(1.15)),
      margin(zero),
    ],
  ),
  global("h1, h2, h3, h4, h5, h6", [margin(zero)]),
  global(
    "header",
    [display(block), margin3(~h=zero, ~top=rem(1.38), ~bottom=rem(1.38))],
  ),
  /* global("html, body", [height(vh(100.))]), */
  /* global("p", [marginBottom(rem(1.))]), */
  global("h1", [fontSize(rem(2.3)), marginBottom(rem(3.))]),
  global("header > h1 + header > h1", [color(hex("b91919"))]),
  /* global("h2", [fontSize(rem(2.441))]), */
  /* global("h3", [fontSize(rem(1.953))]), */
  /* global("h4", [fontSize(rem(1.563))]), */
  /* global("h5", [fontSize(rem(1.25))]), */
  /* global("h6", [fontSize(rem(0.8))]), */
  global("a", [color(Theme.LightTheme.textColor), fontWeight(semiBold)]),
  global("a:hover", [textDecoration(none)]),
);

[@react.component]
let make = () => {
  <Router />;
};
