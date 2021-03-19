open ReactUtils;

open Relude.Globals;

open ReductiveStore;

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

[@react.component]
let make = () => {
  open ReductiveStore__Dialogs;

  let dispatch = Wrapper.useDispatch();
  let dialogs = Wrapper.useSelector(Selector.DialogsStore.dialogs);

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
    Keys.getCombokeys() |> Combokeys.pause();

    getCombokeys();
    /* |> Combokeys.bind("esc", _ => { */
    /*      close(); */
    /*      false; */
    /*    }); */
  };

  let detachShortcuts = () => {
    Keys.getCombokeys() |> Combokeys.unpause();
    getCombokeys() |> Combokeys.detach();
    combokeys := None;
  };

  React.useEffect1(
    _ => {
      switch (dialogs) {
      | [||] => detachShortcuts() |> (_ => None)
      | _ => bindShortcuts() |> (_ => None)
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
         <div className=Styles.root>
           {dialogs
            |> Array.mapWithIndex((x, i) =>
                 (
                   switch (x) {
                   | FilePicker =>
                     Some((
                       Dialogs__FilePicker.id,
                       <Dialogs__FilePicker close />,
                     ))
                   | _ => None
                   }
                 )
                 |> Option.map(((id, x)) =>
                      <div
                        className=Styles.dialog
                        key={id ++ (i |> Int.toString)}>
                        x
                      </div>
                    )
                 |> Option.getOrElse(React.null)
               )
            |> React.array}
         </div>,
       ),
     );
};
