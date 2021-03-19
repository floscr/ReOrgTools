type dialog =
  | FilePicker;

type state = {dialogs: array(dialog)};

type action =
  | OpenDialog(dialog)
  | CloseDialogs;

let initial = {dialogs: [|FilePicker|]};

let reducer = (state, action) => {
  switch (action) {
  | OpenDialog(x) => {...state, dialogs: Array.append([|x|], state.dialogs)}
  | CloseDialogs => {dialogs: [||]}
  };
};
