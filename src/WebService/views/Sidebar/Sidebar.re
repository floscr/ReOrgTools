open ReactUtils;
open Relude.Globals;
open ReactUtils;

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
      backgroundColor(Theme.BaseTheme.sidebarBgColor),
      overflowY(scroll),
      flexGrow(1.),
      flexShrink(1.),
    ]);

  let header =
    style([
      fontWeight(medium),
      color(Theme.BaseTheme.grey3),
      padding(Theme.Spacing.large),
      fontSize(rem(1.)),
    ]);

  let backIcon =
    style([position(absolute), top(innerSpacing), right(innerSpacing)]);
};

type mode =
  | Workspaces
  | Outline;

type state = {mode};

let initialState = {mode: Workspaces};

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
  let theme = React.useContext(CssTheme.Context.themeContext);

  let onFileClick = () => send(SwitchMode(Outline));
  let onBackClick = _ => send(SwitchMode(Workspaces));

  <div className=Styles.root>
    <header className=Styles.header> {"Files" |> s} </header>
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
