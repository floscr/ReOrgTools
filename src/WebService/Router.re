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

  let main =
    style([
      gridColumnStart(2),
      display(`flex),
      flexDirection(column),
      overflow(auto),
      selector(
        "::-webkit-scrollbar",
        [
          backgroundColor(hex("00000008")),
          borderLeft(px(1), solid, hex("e2e2e2")),
          boxShadow(
            Shadow.box(
              ~x=px(1),
              ~blur=px(5),
              ~inset=true,
              rgba(0, 0, 0, pct(8.)),
            ),
          ),
        ],
      ),
    ]);
};

type state = {isDocumentScrolled: bool};

let initialState = {isDocumentScrolled: false};

type action =
  | SetScrollState(int)
  | NoOp;

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | SetScrollState(scrollTop) =>
    Update({
      ...state,
      isDocumentScrolled:
        scrollTop
        |> Option.some
        |> Option.reject(Int.eq(Int.zero))
        |> Option.foldLazy(_ => false, _ => true),
    })
  | NoOp => NoUpdate
  };

let showMain = (~id=?, ~queryParams, ~workspaceIndex=0, ()) => {
  <>
    <aside className=Styles.sidebar> <Sidebar workspaceIndex id /> </aside>
    <article className=Styles.main>
      {switch (id) {
       | Some(id) => <Controller__OrgDocument id queryParams workspaceIndex />
       | _ => React.null
       }}
    </article>
  </>;
};

[@react.component]
let make = () => {
  let dispatch = State.Store.useDispatch();

  let rootRef: React.ref(Js.Nullable.t(Dom.element)) =
    React.useRef(Js.Nullable.null);

  let (_state, _send) =
    ReludeReact.Reducer.useReducer(reducer, initialState);

  let openFilePicker = _ =>
    dispatch(
      State.DialogsAction(
        State__Dialogs.OpenDialog(State__Dialogs.FilePicker),
      ),
    );
  open Webapi.Url;

  let url = ReasonReactRouter.useUrl();
  let params = URLSearchParams.make(url.search);
  let queryParams = Types__URLSearchParams.make(params);
  let isHomePage = url.path |> List.isEmpty;

  ReludeReact.Effect.useIOOnMount(
    Localforage.Localforage_IO.get(Types__LocalStorage.settings),
    data =>
      data
      |> Js.Nullable.toOption
      |> Option.flatMap(State__Settings.Decode.decodeJson >> Result.toOption)
      |> Option.tap(x =>
           State.SettingsAction(State__Settings.SaveState(x)) |> dispatch
         )
      |> Option.filter(_ => isHomePage)
      |> Option.flatMap(({lastViewedFile}: State__Settings.state) =>
           lastViewedFile
         )
      |> Option.tap(ReasonReactRouter.replace)
      /* |> Option.filter() */
      /* |> Option.tap(ReasonReactRouter.url) */
      |> ignore,
    error => Js_console.error(error),
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

  let bindShortcuts = () => {
    Keys.get()
    |> Combokeys.bindArray(
         [|"cmd+k", "ctrl+k", "ctrl+shift+k"|],
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

  <main className=Styles.root ref={ReactDOMRe.Ref.domRef(rootRef)}>
    {switch (url.path) {
     | ["file", workspaceIndex, id] =>
       let workspaceIndex =
         workspaceIndex |> String.toInt |> Option.getOrElse(0);
       showMain(~id, ~queryParams, ~workspaceIndex, ());
     | _ => showMain(~queryParams, ())
     }}
    <Dialogs />
  </main>;
};
