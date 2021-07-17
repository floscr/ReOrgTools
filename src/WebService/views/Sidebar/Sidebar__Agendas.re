open ReactUtils;
open Relude.Globals;
open State;

[@react.component]
let make = () => {
  module Styles = Sidebar__Utils.Styles;

  <div className=Styles.headerWrapper>
    <header className=Styles.header> {"Agendas" |> s} </header>
    <IconButton
      style=Styles.plusButton
      id="add"
      onClick={_ => ReasonReactRouter.replace("/agenda/new")}
    />
  </div>;
};
