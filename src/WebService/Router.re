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
      height(vh(100.)),
      display(flexBox),
      flexDirection(column),
    ]);

  let main =
    style([gridColumnStart(2), padding(innerSpacing), overflow(hidden)]);
};

let showMain = (~id=?, ~header, ~send, ~state, ()) => {
  let file = id |> Option.flatMap(x => StringMap.get(x, state.files));

  <>
    <aside className=Styles.sidebar> <Sidebar file /> </aside>
    <article className=Styles.main>
      {switch (id) {
       | Some(id) => <Controller__OrgDocument id header file send />

       | _ => React.null
       }}
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
     | ["file", id] => showMain(~id, ~header, ~send, ~state, ())
     | _ => showMain(~header, ~send, ~state, ())
     }}
  </main>;
};
