open ReactUtils;

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

type state =
  | Loaded(loadedState)
  | Loading
  | Initial;

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
