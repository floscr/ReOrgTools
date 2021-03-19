let combokeys: ref(option(Combokeys.t)) = ref(None);

let getCombokeys = () =>
  switch (combokeys^) {
  | None =>
    let keys =
      Combokeys.init(
        Webapi.Dom.document |> Webapi.Dom.Document.documentElement,
      )
      |> Combokeys.initPlayPausePlugin;
    combokeys := Some(keys);
    keys;
  | Some(x) => x
  };
