open ReactUtils;
open Relude.Globals;
open State;
open Dialogs__Style;

let id = "Dialogs__FilePicker";

type state = {
  options: array(string),
  query: string,
  selection: option(int),
};

let initialState = {options: [||], query: "", selection: Some(0)};

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
      selection: Menu.selectNext(~bounds, ~index=state.selection),
    })
  | SelectPrev(bounds) =>
    Update({
      ...state,
      selection: Menu.selectPrev(~bounds, ~index=state.selection),
    })

  | NoOp => NoUpdate
  };

[@react.component]
let make = (~close) => {
  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);
  let query = state.query |> String.toLowerCase;
  let workspaces = Store.useSelector(Selector.Workspaces.workspaces);

  let results =
    workspaces
    |> List.foldLeft(
         (acc, (workspace, files)) => {
           let index =
             acc
             |> Array.last
             |> Option.map(((i, _, _)) => Int.add(1, i))
             |> Option.getOrElse(0);

           files
           |> Array.map(x => (index, workspace, x))
           |> Array.concat(acc);
         },
         [||],
       )
    |> Array.filter(((_, _, {name}: Shared__API__Workspaces.File.t)) =>
         name |> String.toLowerCase |> String.contains(~search=query)
       );

  let onSubmit = (~index=None, results) => {
    results
    |> Array.at(index |> Option.getOrElse(0))
    |> Option.orElse(~fallback=results |> Array.head)
    |> Option.flatMap(
         ((_, workspaceName, {name}: Shared__API__Workspaces.File.t)) =>
         workspaces
         |> List.map(((a, _)) => a)
         |> List.indexOfBy(String.eq, workspaceName)
         |> Option.map(i => (i, name))
       )
    |> Option.tap(((i, name)) => {
         let name = Filename.chop_extension(name);
         close();
         ReasonReactRouter.push({j|/file/$i/$name|j});
       });
  };

  let onChange = event => {
    send(ChangeQuery(event->ReactEvent.Form.target##value));
  };
  let onKeyDown = event => {
    switch (event |> ReactEvent.Keyboard.key) {
    | "Enter" =>
      onSubmit(~index=state.selection, results) |> ignore;
      ReactEvent.Keyboard.preventDefault(event);
    | "Escape" => close()
    | _ => ()
    };
  };

  let bindings = [|
    (
      [|"ctrl+k", "ctrl+shift+k", "esc"|],
      _ => {
        close();
        false;
      },
    ),
    (
      [|"ctrl+n", "down"|],
      _ => {
        SelectNext(results |> Array.length) |> send;
        false;
      },
    ),
    (
      [|"ctrl+p", "up"|],
      _ => {
        SelectPrev(results |> Array.length) |> send;
        false;
      },
    ),
  |];

  <RoundedDialogWrapper bindings key={results |> Array.length |> Int.toString}>
    <Menu onChange onKeyDown placeholder="Pick File" value={state.query}>
      {results
       |> Array.mapWithIndex(
            ((_, _, {name}: Shared__API__Workspaces.File.t), i) => {
            let isSelected =
              state.selection |> Option.filter(Int.eq(i)) |> Option.isSome;
            <React.Fragment key={i |> Int.toString}>
              <Menu.Item
                isSelected
                onClick={_ => onSubmit(~index=Some(i), results) |> ignore}
                value={name |> Filename.chop_extension}
              />
            </React.Fragment>;
          })
       |> React.array}
    </Menu>
  </RoundedDialogWrapper>;
};
