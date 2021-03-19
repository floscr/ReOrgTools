open ReactUtils;
open State;
open Relude.Globals;

module Styles = {
  open Css;
  let innerSpacing = FixedTheme.Spacing.xlarge;

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

let showMain = (~id=?, ~header, ~send, ~state, ~workspaceIndex=0, ()) => {
  let file = id |> Option.flatMap(x => StringMap.get(x, state.filesCache));

  <>
    <aside className=Styles.sidebar>
      <Sidebar workspaces={state.workspaces} workspaceIndex file />
    </aside>
    <article className=Styles.main>
      {switch (id) {
       | Some(id) =>
         <Controller__OrgDocument id header file send workspaceIndex />
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

  ReludeReact.Effect.useIOOnMount(
    API__Workspaces.Request.make(),
    data => State.FetchWorkspacesSuccess(data) |> send,
    error => State.FetchWorkspacesFailure(error) |> send,
  );

  <main className=Styles.root>
    {switch (url.path) {
     | ["file", workspaceIndex, id] =>
       let workspaceIndex =
         workspaceIndex |> String.toInt |> Option.getOrElse(0);
       showMain(~id, ~header, ~send, ~state, ~workspaceIndex, ());
     | _ => showMain(~header, ~send, ~state, ())
     }}
  </main>;
};
