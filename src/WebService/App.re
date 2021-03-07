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
      selector(
        "&::-webkit-scrollbar",
        [width(px(8)), backgroundColor(rgba(0, 0, 0, `num(0.2)))],
      ),
      selector(
        "&::-webkit-scrollbar-thumb",
        [
          unsafe("boxShadow", "inset 0 0 6px rgba(0,0,0,0.3)"),
          unsafe("borderRadius", "10px"),
          backgroundColor(rgba(0, 0, 0, `num(0.1))),
        ],
      ),
      selector(
        "&::-webkit-scrollbar-track",
        [
          unsafe("boxShadow", "inset 0 0 6px rgba(0,0,0,0.3)"),
          unsafe("borderRadius", "10px"),
          backgroundColor(rgba(0, 0, 0, `num(0.1))),
        ],
      ),
      position(fixed),
      width(vw(20.)),
      overflowY(scroll),
      height(vh(100.)),
      padding(innerSpacing),
    ]);

  let main = style([gridColumnStart(2), padding(innerSpacing)]);
};

let showMain = (~id) =>
  <>
    <aside className=Styles.sidebar> <Files /> </aside>
    <article className=Styles.main> <Page id /> </article>
  </>;

[@react.component]
let make = () => {
  let url = ReasonReactRouter.useUrl();

  <main>
    {switch (url.path) {
     | ["file", id] => showMain(~id)
     | _ => showMain(~id=None)
     }}
  </main>;
};
