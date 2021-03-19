open ReactUtils;
open Relude.Globals;

module Styles = {
  open Css;
  let innerSpacing = FixedTheme.Spacing.xlarge;

  let root =
    style([
      position(fixed),
      top(zero),
      left(zero),
      right(zero),
      bottom(zero),
      overflow(auto),
      display(grid),
      gridTemplateColumns([vw(20.), auto]),
    ]);

  let sidebar =
    style([
      position(fixed),
      width(vw(20.)),
      height(vh(100.)),
      display(flexBox),
      flexDirection(column),
    ]);

  let main = style([gridColumnStart(2), padding(innerSpacing)]);
};

let showMain = (~id=?, ~header, ~workspaceIndex=0, ()) => {
  <>
    <aside className=Styles.sidebar> <Sidebar workspaceIndex id /> </aside>
    <article className=Styles.main>
      {switch (id) {
       | Some(id) => <Controller__OrgDocument id header workspaceIndex />
       | _ => React.null
       }}
    </article>
  </>;
};

[@react.component]
let make = () => {
  let dispatch = State.Store.useDispatch();

  dispatch(
    State.DialogsAction(
      State__Dialogs.OpenDialog(
        State__Dialogs.Lightbox({
          index: None,
          items: [|
            "http://localhost:4000/attachments/6d/cfa2c7-caee-481b-9d3d-7eb1f5696d6c/_20210406_12103331e7bd307e09c6ae7b8809ff6f49f3e4.png",
          |],
        }),
      ),
    ),
  );

  let openFilePicker = _ =>
    dispatch(
      State.DialogsAction(
        State__Dialogs.OpenDialog(State__Dialogs.FilePicker),
      ),
    );

  State__Workspaces.(
    ReludeReact.Effect.useIOOnMount(
      Request.make(),
      data =>
        State.WorkspaceAction(Store.FetchWorkspacesSuccess(data)) |> dispatch,
      error =>
        State.WorkspaceAction(Store.FetchWorkspacesFailure(error))
        |> dispatch,
    )
  );
  open Webapi.Url;
  let url = ReasonReactRouter.useUrl();
  let params = URLSearchParams.make(url.search);
  let header = params |> URLSearchParams.get("header");

  let bindShortcuts = () => {
    Keys.get()
    |> Combokeys.bindArray(
         [|"cmd+k", "ctrl+k"|],
         _ => {
           openFilePicker();
           false;
         },
       );
  };
  let detachShortcuts = () => {
    let c = Keys.get();
    c |> Combokeys.detach();
    Keys.combokeys := None;
  };
  React.useEffect0(_ => {
    bindShortcuts();
    Some(() => detachShortcuts());
  });

  <main className=Styles.root>
    {switch (url.path) {
     | ["file", workspaceIndex, id] =>
       let workspaceIndex =
         workspaceIndex |> String.toInt |> Option.getOrElse(0);
       showMain(~id, ~header, ~workspaceIndex, ());
     | _ => showMain(~header, ())
     }}
    <Dialogs />
  </main>;
};
