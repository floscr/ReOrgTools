open ReactUtils;
open Relude.Globals;

module Styles = {
  open Css;
  let innerSpacing = Theme.Spacing.large;

  let root =
    style([
      position(relative),
      padding(innerSpacing),
      paddingRight(
        `calc((`sub, innerSpacing, Theme.Spacing.scrollbarWidth)),
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
  | _ => NoUpdate
  };

[@react.component]
let make = (~file, ~workspaces, ~workspaceIndex) => {
  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);

  let onFileClick = () => send(SwitchMode(Outline));
  let onBackClick = _ => send(SwitchMode(Workspaces));

  <div className=Styles.root>
    <IconButton style=Styles.backIcon id="arrow_back" />
    {switch (state.mode, file) {
     | (Outline, Some(State.File.Fetched({ast}))) =>
       <>
         <button onClick=onBackClick> {"<- Go back" |> s} </button>
         <Outline ast />
       </>
     | _ => <Workspaces__Component workspaces workspaceIndex onFileClick />
     }}
  </div>;
};
