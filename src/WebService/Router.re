open ReactUtils;
open State;
open Relude.Globals;

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

let showMain = (~id, ~header, ~send, ~state) => {
  <>
    <aside className=Styles.sidebar> <Sidebar page={state.page} id /> </aside>
    <article className=Styles.main>
      <PageComponent id header page={state.page} send />
    </article>
  </>;
};

[@react.component]
let make = () => {
  let (state, send) =
    ReludeReact.Reducer.useReducer(reducer, initialGlobalState);
  open Webapi.Url;
  let url = ReasonReactRouter.useUrl();
  let params = URLSearchParams.make(url.search);
  let header = params |> URLSearchParams.get("header");

  <main className=Styles.root>
    {switch (url.path) {
     | ["file", id] => showMain(~id=Some(id), ~header, ~send, ~state)
     | _ => showMain(~id=None, ~header, ~send, ~state)
     }}
  </main>;
};
