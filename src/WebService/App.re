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

  let root = style([display(grid), gridTemplateColumns([vw(20.), auto])]);

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

  let main =
    style([gridColumnStart(2), padding(innerSpacing), overflow(hidden)]);
};

let showMain = (~id) =>
  <>
    <aside className=Styles.sidebar> <Files /> </aside>
    <article className=Styles.main>
      {switch (id) {
       | Some(_) => <PageComponent id />
       | _ => <div> {"No file selected" |> s} </div>
       }}
    </article>
  </>;

[@react.component]
let make = () => {
  let url = ReasonReactRouter.useUrl();

  <main className=Styles.root>
    {switch (url.path) {
     | ["file", id] => showMain(~id=Some(id))
     | _ => showMain(~id=None)
     }}
  </main>;
};
