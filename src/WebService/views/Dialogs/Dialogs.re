open ReactUtils;
open Relude.Globals;
open State;

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
};

[@react.component]
let make = () => {
  open State__Dialogs;
  let dispatch = Store.useDispatch();
  let dialogs = Store.useSelector(Selector.Dialogs.dialogs);

  let close = _ => dispatch(DialogsAction(CloseDialogs));

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
    Keys.get() |> Combokeys.pause();
    getCombokeys()
    |> Combokeys.bindArray(
         [|"escape"|],
         _ => {
           close();
           false;
         },
       );
  };
  let detachShortcuts = () => {
    Keys.get() |> Combokeys.unpause();
    getCombokeys() |> Combokeys.detach();
    combokeys := None;
  };
  React.useEffect1(
    _ => {
      switch (dialogs) {
      | [||] => detachShortcuts()
      | _ => bindShortcuts() |> ignore
      };
      Some(() => detachShortcuts());
    },
    [|Array.length(dialogs)|],
  );

  Option.some(dialogs)
  |> Option.filter(Array.isNotEmpty)
  |> Option.flatMap(_ =>
       Webapi.Dom.(Document.querySelector("body", document))
     )
  |> Option.foldLazy(
       () => React.null,
       ReactDOMRe.createPortal(
         <div className=Styles.root onClick={_ => close()}>
           {dialogs
            |> Array.mapWithIndex((x, i) =>
                 (
                   switch (x) {
                   | FilePicker =>
                     Some((
                       Dialogs__FilePicker.id,
                       <Dialogs__FilePicker close />,
                     ))
                   | CommandsMenu =>
                     Some((
                       Dialogs__CommandsMenu.id,
                       <Dialogs__CommandsMenu close />,
                     ))
                   | Lightbox({items}) =>
                     Some((
                       Dialogs__Lightbox.id,
                       <Dialogs__Lightbox images=items />,
                     ))
                   }
                 )
                 |> Option.map(((id, x)) =>
                      <React.Fragment key={id ++ (i |> Int.toString)}>
                        x
                      </React.Fragment>
                    )
                 |> Option.getOrElse(React.null)
               )
            |> React.array}
         </div>,
       ),
     );
};
