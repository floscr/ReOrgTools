open ReactUtils;
open State;
open Relude.Globals;

module Styles = {
  open Css;
  let innerSpacing = Theme.Spacing.xlarge;

  let root = style([display(grid), gridTemplateColumns([vw(20.), auto])]);

  let sidebar =
    style([
      position(fixed),
      width(vw(20.)),
      overflowY(scroll),
      height(vh(100.)),
      padding(innerSpacing),
      paddingRight(
        `calc((`sub, innerSpacing, Theme.Spacing.scrollbarWidth)),
      ),
      backgroundColor(Theme.BaseTheme.sidebarBgColor),
    ]);

  let main =
    style([gridColumnStart(2), padding(innerSpacing), overflow(hidden)]);
};

let showMain = (~id, ~header, ~send, ~state) => {
  <>
    <aside className=Styles.sidebar> <Sidebar page={state.page} id /> </aside>
    <article className=Styles.main>
      <Controller__OrgDocument id header ast={state.page} send />
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
