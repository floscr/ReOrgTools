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
let make = (~close) => {
  let ({query}: state, send) =
    ReludeReact.Reducer.useReducer(reducer, initialState);

  let combokeys: ref(option(Combokeys.t)) = ref(None);
  let getCombokeys = () =>
    switch (combokeys^) {
    | None =>
      let keys =
        Combokeys.init(
          Webapi.Dom.document |> Webapi.Dom.Document.documentElement,
        )
        |> Combokeys.initGlobalBindPlugin;
      combokeys := Some(keys);
      keys;
    | Some(x) => x
    };

  let bindShortcuts = _ => {
    getCombokeys()
    |> Combokeys.bindGlobalArray(
         [|"cmd+k", "ctrl+k"|],
         _ => {
           Js.log("What");
           close();
           false;
         },
       );
  };

  let detachShortcuts = () => {
    getCombokeys() |> Combokeys.detach();
    combokeys := None;
  };

  React.useEffect0(_ => {
    bindShortcuts();
    Some(() => detachShortcuts());
  });

  let onChange = event => {
    send(ChangeQuery(event->ReactEvent.Form.target##value));
  };

  <div>
    <input
      autoFocus=true
      name=id
      value=query
      onChange
      onBlur={_ => close() |> ignore}
      placeholder="Pick File"
    />
  </div>;
};
