let combokeys: ref(option(Combokeys.t)) = ref(None);

let getCombokeys = keys =>
  switch (keys) {
  | None =>
    let keys =
      Combokeys.init(
        Webapi.Dom.document |> Webapi.Dom.Document.documentElement,
      )
      |> Combokeys.PausePlugin.initPlayPausePlugin;
    combokeys := Some(keys);
    keys;
  | Some(x) => x
  };

let get = () => getCombokeys(combokeys^);
