open ReactUtils;
open Relude.Globals;
open State;

module Styles = {
  open Css;
  let innerSpacing = FixedTheme.Spacing.xlarge;

  let main =
    style([
      position(fixed),
      top(zero),
      left(zero),
      right(zero),
      bottom(zero),
      overflow(hidden),
    ]);

  let rootWithSidebar = isSidebarOpen =>
    style([
      display(grid),
      gridTemplateColumns(isSidebarOpen ? [vw(20.), auto] : []),
    ]);

  let sidebar =
    style([
      minWidth(vw(20.)),
      maxWidth(px(200)),
      height(vh(100.)),
      display(flexBox),
      flexDirection(column),
    ]);

  let sidebarWrapper =
    style([
      position(absolute),
      top(zero),
      left(zero),
      right(zero),
      bottom(zero),
      flexDirection(row),
      display(flexBox),
    ]);

  let contentWrapper = style([flexGrow(1.), flexShrink(1.)]);
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

module WithSidebar = {
  [@react.component]
  let make = (~id=?, ~isSidebarOpen, ~children) => {
    <div className=Styles.sidebarWrapper>
      {switch (isSidebarOpen) {
       | true => <aside className=Styles.sidebar> <Sidebar id /> </aside>
       | _ => React.null
       }}
      children
    </div>;
  };
};

let showAgenda = (~id: option(string)=None, unit) => {
  <WithSidebar isSidebarOpen=false> <AgendaBuilder__Root ?id /> </WithSidebar>;
};

let showMain = (~id=?, ~queryParams, ~workspaceIndex=0, ~isSidebarOpen, ()) => {
  let {narrowToHeader, layoutType}: Types__URLSearchParams.t = queryParams;

  <WithSidebar ?id isSidebarOpen>
    {switch (id) {
     | Some(id) =>
       let identifier: Types__Files.Identifier.t = {
         id,
         workspace: workspaceIndex,
       };
       <div className=Styles.contentWrapper>
         <OrgDocument__Toolbar workspaceIndex id layoutType />
         <Radix.ScrollArea.Wrapper>
           <Controller__OrgDocument
             identifiers=[|identifier|]
             narrowToHeader
             layoutType
           />
         </Radix.ScrollArea.Wrapper>
       </div>;

     | _ => React.null
     }}
  </WithSidebar>;
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
      |> Option.flatMap(State__Settings.Decode.decodeJson >> Result.toOption)
      |> Option.tap(x =>
           SettingsAction(State__Settings.SaveState(x)) |> dispatch
         )
      |> Option.tap(_ => SettingsLoaded |> send)
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

  <main className=Styles.main ref={ReactDOMRe.Ref.domRef(rootRef)}>
    {switch (state.areSettingsLoaded, url.path) {
     // Agenda
     | (true, ["agenda", "new"]) => showAgenda()
     | (true, ["agenda", "edit", id]) => showAgenda(~id=Some(id), ())
     | (true, ["agendas", id]) =>
       <WithSidebar isSidebarOpen=true>
         <div className=Styles.contentWrapper>
           <Radix.ScrollArea.Wrapper>
             <Agenda__Root.Component id />
           </Radix.ScrollArea.Wrapper>
         </div>
       </WithSidebar>

     // File
     | (true, ["file", workspaceIndex, id]) =>
       let workspaceIndex =
         workspaceIndex |> String.toInt |> Option.getOrElse(0);
       showMain(~id, ~queryParams, ~workspaceIndex, ~isSidebarOpen, ());

     // Loading settings
     | (false, _) => React.null

     // Default
     | _ => showMain(~queryParams, ~isSidebarOpen, ())
     }}
    <Dialogs />
  </main>;
};
