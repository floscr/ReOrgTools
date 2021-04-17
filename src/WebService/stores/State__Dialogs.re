type lightBoxProps = {
  index: option(int),
  items: array(string),
};

type dialog =
  | FilePicker
  | CommandsMenu
  | Lightbox(lightBoxProps);

type state = {dialogs: array(dialog)};

type action =
  | OpenDialog(dialog)
  | CloseDialogs;

let initial = {dialogs: [||]};

let reducer = (state, action) => {
  switch (action) {
  | OpenDialog(x) => {...state, dialogs: Array.append([|x|], state.dialogs)}
  | CloseDialogs => {dialogs: [||]}
  };
};
