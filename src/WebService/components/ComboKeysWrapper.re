open Relude.Globals;

[@react.component]
let make = (~bindings=[||], ~children) => {
  let combokeys: ref(option(Combokeys.t)) = ref(None);

  let getCombokeys = () =>
    switch (combokeys^) {
    | None =>
      let keys =
        Combokeys.init(
          Webapi.Dom.document |> Webapi.Dom.Document.documentElement,
        )
        |> Combokeys.GlobalBindPlugin.initGlobalBindPlugin;
      combokeys := Some(keys);
      keys;
    | Some(x) => x
    };

  let bindShortcuts = _ => {
    let keys = getCombokeys();
    bindings
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

  children;
};
