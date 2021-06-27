open ReactUtils;
open Relude.Globals;
open State;

module Styles = {
  open Css;
  let innerSpacing = FixedTheme.Spacing.xlarge;

  let root = isSidebarOpen =>
    style([
      position(fixed),
      top(zero),
      left(zero),
      right(zero),
      bottom(zero),
      display(grid),
      gridTemplateColumns(isSidebarOpen ? [vw(20.), auto] : []),
    ]);

  let sidebar =
    style([
      position(fixed),
      width(vw(20.)),
      height(vh(100.)),
      display(flexBox),
      flexDirection(column),
    ]);

  let main = isSidebarOpen =>
    style([
      gridColumnStart(isSidebarOpen ? 2 : 1),
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

type state = {
  isDocumentScrolled: bool,
  areSettingsLoaded: bool,
};

let initialState = {isDocumentScrolled: false, areSettingsLoaded: false};

type action =
  | SetScrollState(int)
  | SettingsLoaded
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
  | SettingsLoaded => Update({...state, areSettingsLoaded: true})
  | NoOp => NoUpdate
  };

module MainWrapper = {
  [@react.component]
  let make = (~id, ~isSidebarOpen, ~children) => {
    <>
      {switch (isSidebarOpen) {
       | true => <aside className=Styles.sidebar> <Sidebar id /> </aside>
       | _ => React.null
       }}
      <article className={Styles.main(isSidebarOpen)}> children </article>
    </>;
  };
};

let showAgenda = (~isSidebarOpen, ()) => {
  <MainWrapper id=None isSidebarOpen=false> {"New Agenda" |> s} </MainWrapper>;
};

let showMain = (~id=?, ~queryParams, ~workspaceIndex=0, ~isSidebarOpen, ()) => {
  <MainWrapper id isSidebarOpen>
    {switch (id) {
     | Some(id) => <Controller__OrgDocument id queryParams workspaceIndex />
     | _ => React.null
     }}
  </MainWrapper>;
};

[@react.component]
let make = () => {
  let dispatch = State.Store.useDispatch();
  let isSidebarOpen = Store.useSelector(Selector.Settings.isSidebarOpen);
  let user = Store.useSelector(Selector.User.loggedInUser);

  let rootRef: React.ref(Js.Nullable.t(Dom.element)) =
    React.useRef(Js.Nullable.null);

  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);

  let openFilePicker = _ =>
    dispatch(
      DialogsAction(State__Dialogs.OpenDialog(State__Dialogs.FilePicker)),
    );
  let openCommandsMenu = _ =>
    dispatch(
      DialogsAction(State__Dialogs.OpenDialog(State__Dialogs.CommandsMenu)),
    );

  let url = ReasonReactRouter.useUrl();
  let params = Webapi.Url.URLSearchParams.make(url.search);
  let queryParams = Types__URLSearchParams.make(params);
  let isHomePage = url.path |> List.isEmpty;

  ReludeReact.Effect.useIOOnMount(
    Localforage.Localforage_IO.get(Types__LocalStorage.settings),
    data =>
      data
      |> Js.Nullable.toOption
      |> Option.tap(_ => SettingsLoaded |> send)
      |> Option.flatMap(State__Settings.Decode.decodeJson >> Result.toOption)
      |> Option.tap(x =>
           SettingsAction(State__Settings.SaveState(x)) |> dispatch
         )
      |> Option.filter(_ => isHomePage)
      |> Option.flatMap(({lastViewedFile}: State__Settings.state) =>
           lastViewedFile
         )
      |> Option.tap(ReasonReactRouter.replace)
      |> ignore,
    error => {
      Js_console.error(error);
      SettingsLoaded |> send;
    },
  );

  React.useEffect1(
    () => {
      user
      |> Option.tap(user =>
           State__Workspaces.Request.make(user)
           |> Relude.IO.unsafeRunAsync(
                fun
                | Ok(a) =>
                  WorkspaceAction(
                    State__Workspaces.Store.FetchWorkspacesSuccess(a),
                  )
                  |> dispatch

                | Error(e) =>
                  WorkspaceAction(
                    State__Workspaces.Store.FetchWorkspacesFailure(e),
                  )
                  |> dispatch,
              )
         )
      |> ignore;
      None;
    },
    [|user |> Option.isSome|],
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
    Keys.get()
    |> Combokeys.bindArray(
         [|"ctrl+shift+p"|],
         _ => {
           openCommandsMenu();
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

  <main
    className={Styles.root(isSidebarOpen)}
    ref={ReactDOMRe.Ref.domRef(rootRef)}>
    {switch (state.areSettingsLoaded, url.path) {
     | (true, ["agenda", "new"]) => showAgenda(~isSidebarOpen, ())
     | (true, ["file", workspaceIndex, id]) =>
       let workspaceIndex =
         workspaceIndex |> String.toInt |> Option.getOrElse(0);
       showMain(~id, ~queryParams, ~workspaceIndex, ~isSidebarOpen, ());
     | (false, _) => React.null
     | _ => showMain(~queryParams, ~isSidebarOpen, ())
     }}
    <Dialogs />
  </main>;
};
