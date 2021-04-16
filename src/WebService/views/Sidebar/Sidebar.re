open ReactUtils;
open Relude.Globals;
open State;

module Styles = {
  open Css;
  let innerSpacing = FixedTheme.Spacing.large;

  let root =
    style([
      position(relative),
      padding(innerSpacing),
      paddingRight(
        `calc((`sub, innerSpacing, FixedTheme.Spacing.scrollbarWidth)),
      ),
      backgroundColor(var(ThemeKeys.grey00)),
      overflowY(scroll),
      flexGrow(1.),
      flexShrink(1.),
    ]);

  let backIcon =
    style([position(absolute), top(innerSpacing), right(innerSpacing)]);
};

type mode =
  | Workspaces
  | Outline;

type state = {
  mode,
  x: int,
};

let initialState = {mode: Workspaces, x: 1};

type action =
  | SwitchMode(mode);

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | SwitchMode(mode) => Update({...state, mode})
  };

[@react.component]
let make = (~id, ~workspaceIndex) => {
  let files = Store.useSelector(Selector.OrgDocuments.files);
  let file = id |> Option.flatMap(x => StringMap.get(x, files));

  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);

  let onFileClick = () => send(SwitchMode(Outline));
  let onBackClick = _ => send(SwitchMode(Workspaces));

  <div className=Styles.root>
    <IconButton style=Styles.backIcon id="arrow_back" />
    {switch (state.mode, file) {
     | (Outline, Some(State__OrgDocuments.File.Fetched({ast}))) =>
       <>
         <button onClick=onBackClick> {"<- Go back" |> s} </button>
         <Outline ast />
       </>
     | _ => <Workspaces__Component workspaceIndex onFileClick />
     }}
  </div>;
};
