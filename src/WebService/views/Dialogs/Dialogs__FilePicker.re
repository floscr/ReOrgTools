open ReactUtils;
open Relude.Globals;
open ReductiveStore;

module Styles = {
  open Css;

  let root = style([maxHeight(vh(30.))]);

  let resultsRoot = style([overflowY(scroll)]);
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
  open ReductiveStore__Workspaces;
  let workspaces = Wrapper.useSelector(Selector.WorkspacesStore.workspaces);

  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);
  let query = state.query |> String.toLowerCase;

  let results =
    workspaces
    |> List.foldLeft(
         (acc, (workspace, files)) => {
           files |> Array.map(x => (workspace, x)) |> Array.concat(acc)
         },
         [||],
       )
    |> Array.filter(((a, {name}: Shared__API__Workspaces.File.t)) =>
         name |> String.toLowerCase |> String.contains(~search=query)
       )
    |> Utils.log;

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
         [|"cmd+k", "ctrl+k", "esc"|],
         _ => {
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

  <div className=Styles.root>
    <input
      autoFocus=true
      autoComplete="false"
      name=id
      value={state.query}
      onChange
      onBlur={e => e->ReactEvent.Synthetic.preventDefault}
      onSubmit={}
      placeholder="Pick File"
    />
    <div className=Styles.resultsRoot>
      <ul>
        {results
         |> Array.mapWithIndex(
              ((a, {name}: Shared__API__Workspaces.File.t), i) =>
              <li key={i |> Int.toString}> {name |> s} </li>
            )
         |> React.array}
      </ul>
    </div>
  </div>;
};
