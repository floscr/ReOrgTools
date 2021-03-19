open FilesTypes;
open ReactUtils;
open Relude.Globals;
open ReactUtils;

module Styles = {
  open Css;
  let innerSpacing = Theme.Spacing.large;

  let root =
    style([
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
};

type mode =
  | FileBrowser
  | Outline;

type state = {mode};

let initialState = {mode: FileBrowser};

type action =
  | SwitchMode(mode);

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | SwitchMode(mode) => Update({...state, mode})
  | _ => NoUpdate
  };

[@react.component]
let make = (~page, ~id) => {
  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);

  let onFileClick = () => send(SwitchMode(Outline));
  let onBackClick = _ => send(SwitchMode(FileBrowser));

  <div className=Styles.root>
    <header className=Styles.header> {"Files" |> s} </header>
    {switch (state.mode, page) {
     | (Outline, State.FetchedPage({ast})) =>
       <>
         <button onClick=onBackClick> {"<- Go back" |> s} </button>
         <Outline ast />
       </>
     | _ => <Files onFileClick />
     }}
  </div>;
};
