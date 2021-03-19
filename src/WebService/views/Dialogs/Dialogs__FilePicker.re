open ReactUtils;
open Relude.Globals;
open ReductiveStore;

module Styles = {
  open Css;

  let root = style([maxHeight(vh(30.))]);

  let resultsRoot = style([overflowY(scroll)]);

  let resultsItem = (~isSelected) =>
    style([color(isSelected ? red : blue)]);
};

let id = "Dialogs__FilePicker";

type state = {
  options: array(string),
  query: string,
  selection: option(int),
};

let initialState = {options: [||], query: "", selection: None};

type action =
  | ChangeQuery(string)
  | SelectNext(int)
  | SelectPrev(int)
  | NoOp;

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | ChangeQuery(query) => Update({...state, query})
  | SelectNext(bounds) =>
    Update({
      ...state,
      selection:
        state.selection
        |> Option.map(Int.add(1))
        |> Option.reject(a => Int.greaterThan(a, bounds))
        |> Option.orElse(~fallback=Some(0)),
    })
  | SelectPrev(bounds) =>
    Update({
      ...state,
      selection:
        state.selection
        |> Option.map(x => Int.subtract(x, 1))
        |> Option.reject(a => Int.lessThan(a, 0))
        |> Option.orElse(~fallback=Some(bounds)),
    })

  | NoOp => NoUpdate
  };

[@react.component]
let make = (~close) => {
  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);
  let query = state.query |> String.toLowerCase;
  open ReductiveStore__Workspaces;
  let workspaces = Wrapper.useSelector(Selector.WorkspacesStore.workspaces);

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
       );

  let bounds = results |> Array.length |> Int.add(1);
  let selectNext = () => SelectNext(bounds) |> send;
  let selectPrev = () => SelectPrev(bounds) |> send;

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
    let keys = getCombokeys();
    [|
      (
        [|"cmd+k", "ctrl+k", "esc"|],
        _ => {
          close();
          false;
        },
      ),
      (
        [|"cmd+n", "ctrl+n"|],
        _ => {
          selectNext();
          false;
        },
      ),
      (
        [|"cmd+p", "ctrl+p"|],
        _ => {
          selectPrev();
          false;
        },
      ),
    |]
    |> Array.forEach(((a, fn)) => Combokeys.bindGlobalArray(a, fn, keys));
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
      placeholder="Pick File"
    />
    <div className=Styles.resultsRoot>
      <ul>
        {results
         |> Array.mapWithIndex(
              ((a, {name}: Shared__API__Workspaces.File.t), i) => {
              let isSelected =
                state.selection |> Option.filter(Int.eq(i)) |> Option.isSome;
              <li
                key={i |> Int.toString}
                className={Styles.resultsItem(~isSelected)}>
                {name |> s}
              </li>;
            })
         |> React.array}
      </ul>
    </div>
  </div>;
};
