open ReactUtils;
open Relude.Globals;
open State;

module Styles = {
  open Css;
  open FixedTheme;

  let root = style([]);
};

let id = "Dialogs__AgendaCreate";

type state = {type_: string};

let initialState = {type_: ""};

type action =
  | NoOp;

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | NoOp => NoUpdate
  };

[@react.component]
let make = () => {
  let (state, _send) = ReludeReact.Reducer.useReducer(reducer, initialState);

  let bindings = [||];

  <ComboKeysWrapper bindings>
    <div className=Styles.root />
  </ComboKeysWrapper>;
};
