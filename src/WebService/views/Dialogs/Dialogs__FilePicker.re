open ReactUtils;

open Relude.Globals;

module Styles = {
  open Css;
};

let id = "Dialogs__FilePicker";

type state = {
  options: array(string),
  query: string,
};

let initialState = {options: [||], query: ""};

type action =
  | ChangeQuery(string)
  | NoOp;

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | ChangeQuery(query) => Update({...state, query})
  | NoOp => NoUpdate
  };

[@react.component]
let make = () => {
  let ({query}: state, send) =
    ReludeReact.Reducer.useReducer(reducer, initialState);

  let onChange = event => {
    send(ChangeQuery(event->ReactEvent.Form.target##value));
  };

  <div>
    <input
      autoFocus=true
      name=id
      value=query
      onChange
      placeholder="Pick File"
    />
  </div>;
};
