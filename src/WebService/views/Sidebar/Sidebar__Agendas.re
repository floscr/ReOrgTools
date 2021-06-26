open ReactUtils;
open Relude.Globals;
open State;

[@react.component]
let make = () => {
  let dispatch = Store.useDispatch();

  module Styles = Sidebar__Utils.Styles;

  let onPlusButtonClick = () =>
    State.DialogsAction(
      State__Dialogs.OpenDialog(State__Dialogs.AgendaCreate),
    )
    |> dispatch;

  <>
    <div className=Styles.headerWrapper>
      <header className=Styles.header> {"Agendas" |> s} </header>
      <IconButton
        style=Styles.plusButton
        id="add"
        onClick={_ => onPlusButtonClick()}
      />
    </div>
  </>;
};
