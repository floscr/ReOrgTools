open ReactUtils;
open Relude.Globals;
open State;

module Styles = {
  open Css;
  open FixedTheme;

  let root = style([]);
};

let id = "Dialogs__Lightbox";

type state = {index: option(int)};

let initialState = {index: Some(0)};

type action =
  | SelectNext(int)
  | SelectPrev(int)
  | NoOp;

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | SelectNext(bounds) =>
    Update(
      {
        index:
          state.index
          |> Option.map(Int.add(1))
          |> Option.reject(a => Int.greaterThan(a, bounds))
          |> Option.orElse(~fallback=Some(0)),
      }: state,
    )
  | SelectPrev(bounds) =>
    Update(
      {
        index:
          state.index
          |> Option.map(x => Int.subtract(x, 1))
          |> Option.reject(a => Int.lessThan(a, 0))
          |> Option.orElse(~fallback=Some(bounds)),
      }: state,
    )
  | NoOp => NoUpdate
  };

[@react.component]
let make = (~images: array(string)) => {
  let (state, _send) = ReludeReact.Reducer.useReducer(reducer, initialState);

  let bindings = [|
    (
      [|"ctrl+p", "left"|],
      _ => {
        Js.log("Prev");
        false;
      },
    ),
    (
      [|"ctrl+n", "right"|],
      _ => {
        Js.log("Next");
        false;
      },
    ),
  |];

  <ComboKeysWrapper bindings>
    <div className=Styles.root>
      {state.index
       |> Option.flatMap(i => Array.at(i, images))
       |> Option.map(src => <img src />)
       |> Option.getOrElse(React.null)}
    </div>
  </ComboKeysWrapper>;
};
