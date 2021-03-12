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

let showMain = (~id, ~header) => {
  <>
    <aside className=Styles.sidebar> <Files /> </aside>
    <article className=Styles.main>
      {switch (id) {
       | Some(_) => <PageComponent id header />
       | _ => <div> {"No file selected" |> s} </div>
       }}
    </article>
  </>;
};

[@react.component]
let make = () => {
  open Webapi.Url;
  let url = ReasonReactRouter.useUrl();
  let params = URLSearchParams.make(url.search);
  let header = params |> URLSearchParams.get("header");

  <main className=Styles.root>
    {switch (url.path) {
     | ["file", id] => showMain(~id=Some(id), ~header)
     | _ => showMain(~id=None, ~header)
     }}
  </main>;
};
