open ReactUtils;
open Relude.Globals;

module Styles = {
  open Css;

  let root =
    style([
      position(fixed),
      backgroundColor(rgba(0, 0, 0, `num(0.8))),
      top(zero),
      left(zero),
      right(zero),
      bottom(zero),
      display(`flex),
      justifyContent(center),
      alignItems(center),
      overflow(hidden),
      padding(FixedTheme.Spacing.large),
    ]);

  let dialog =
    style([
      position(absolute),
      backgroundColor(white),
      borderRadius(FixedTheme.BorderRadius.small),
      padding(FixedTheme.Spacing.medium),
    ]);
};

type dialog =
  | FilePicker;

type state = {dialogs: list(dialog)};

let initialState = {dialogs: [FilePicker]};

type action =
  | OpenDialog(dialog)
  | CloseDialogs;

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | OpenDialog(x) =>
    Update({...state, dialogs: List.append(x, state.dialogs)})
  | CloseDialogs => Update({dialogs: []})
  | _ => NoUpdate
  };

let renderDialog =
  fun
  | FilePicker => <Dialogs__FilePicker />;

[@react.component]
let make = () => {
  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);

  let combokeys: ref(option(Combokeys.t)) = ref(None);
  let getCombokeys = () =>
    switch (combokeys^) {
    | None =>
      let keys =
        Combokeys.init(
          Webapi.Dom.document |> Webapi.Dom.Document.documentElement,
        );
      combokeys := Some(keys);
      keys;
    | Some(x) => x
    };
  let bindShortcuts = () => {
    getCombokeys()
    |> Combokeys.bind("esc", _ => {
         send(CloseDialogs);
         false;
       });
  };
  let detachShortcuts = () => {
    let c = getCombokeys();
    c |> Combokeys.detach();
    combokeys := None;
  };

  React.useEffect1(
    _ => {
      switch (state.dialogs) {
      | [||] => detachShortcuts() |> (_ => None)
      | _ => bindShortcuts() |> (_ => None)
      };
      Some(() => detachShortcuts());
    },
    state.dialogs |> List.toArray,
  );

  Option.some(state.dialogs)
  |> Option.filter(List.isNotEmpty)
  |> Option.flatMap(_ =>
       Webapi.Dom.(Document.querySelector("body", document))
     )
  |> Option.foldLazy(
       () => React.null,
       ReactDOMRe.createPortal(
         <div className=Styles.root>
           {state.dialogs
            |> List.mapWithIndex((x, i) =>
                 (
                   switch (x) {
                   | FilePicker => <Dialogs__FilePicker />
                   | _ => React.null
                   }
                 )
                 |> (x => <div className=Styles.dialog> x </div>)
               )
            |> List.toArray
            |> React.array}
         </div>,
       ),
     );
};
