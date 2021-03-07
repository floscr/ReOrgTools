Css.(
  global("*, *:before, *:after", [boxSizing(borderBox)]),
  global("h1, h2, h3, h4, h5, h6", [margin(zero)]),
  global("html, body", [height(vh(100.))]),
  global(
    "body",
    [
      backgroundColor(Theme.LightTheme.bgColor),
      color(Theme.LightTheme.textColor),
      margin(zero),
      fontFamilies([`custom("IBM Plex Sans"), `sansSerif]),
    ],
  ),
  global("a", [color(Theme.LightTheme.textColor), fontWeight(semiBold)]),
  global("a:hover", [textDecoration(none)]),
);

module Styles = {
  open Css;
  let innerSpacing = rem(2.);

  let root =
    style([
      display(grid),
      gridTemplateColumns([vw(20.), auto]),
      gridGap(rem(1.)),
    ]);

  let sidebar =
    style([
      position(fixed),
      width(vw(20.)),
      overflowY(scroll),
      height(vh(100.)),
      padding(innerSpacing),
    ]);

  let main = style([gridColumnStart(2), padding(innerSpacing)]);
};

[@react.component]
let make = () =>
  <main>
    <aside className=Styles.sidebar> <Files /> </aside>
    <article className=Styles.main> <Page /> </article>
  </main>;
