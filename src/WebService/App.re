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
      backgroundColor(var(ThemeKeys.bgColor)),
      color(var(ThemeKeys.textColor)),
      fontFamilies([`custom("IBM Plex Sans"), `sansSerif]),
      lineHeight(`abs(1.15)),
      margin(zero),
    ],
  ),
  // Scrollbar
  global("::-webkit-scrollbar", [width(Theme.Spacing.scrollbarWidth)]),
  global(
    "::-webkit-scrollbar-thumb",
    [
      border(px(4), solid, rgba(0, 0, 0, `num(0.0))),
      backgroundClip(paddingBox),
      borderRadius(px(50)),
      backgroundColor(rgba(0, 0, 0, `num(0.15))),
    ],
  ),
  global(
    "::-webkit-scrollbar-button",
    [width(zero), height(zero), display(none)],
  ),
  global("img", [maxWidth(`percent(100.))]),
  global("h1, h2, h3, h4, h5, h6", [margin(zero)]),
  /* global("html, body", [height(vh(100.))]), */
  /* global("p", [marginBottom(rem(1.))]), */
  global("h1", [fontSize(rem(2.3)), marginBottom(rem(3.))]),
  global("header > h1 + header > h1", [color(hex("b91919"))]),
  /* global("h2", [fontSize(rem(2.441))]), */
  /* global("h3", [fontSize(rem(1.953))]), */
  /* global("h4", [fontSize(rem(1.563))]), */
  /* global("h5", [fontSize(rem(1.25))]), */
  /* global("h6", [fontSize(rem(0.8))]), */
  global("a", [color(var(ThemeKeys.textColor)), fontWeight(semiBold)]),
  global("a:hover", [textDecoration(none)]),
);

CssTheme.Dom.setDefaultTheme();

[@react.component]
let make = () => {
  <CssTheme.Provider> <Router /> </CssTheme.Provider>;
};
