open ReactUtils;
open Relude.Globals;
open State;

[@react.component]
let make = () => {
  module Styles = Sidebar__Utils.Styles;

  let dispatch = State.Store.useDispatch();
  let agendas = Store.useSelector(Selector.Settings.agendas);

  <>
    <div className=Styles.headerWrapper>
      <header className=Styles.header> {"Agendas" |> s} </header>
      <IconButton
        style=Styles.plusButton
        id="add"
        onClick={_ => ReasonReactRouter.replace("/agenda/new")}
      />
    </div>
    <ul className=Styles.ul>
      {agendas
       |> Array.map(({id} as agenda: State__Settings.Agenda.t) =>
            <li className=Styles.li key={j|agenda-$id|j}>
              <button
                className=Styles.button
                onClick={_ => ReasonReactRouter.replace({j|/agendas/$id|j})}>
                {id |> s}
                <IconButton
                  style=Styles.optionsButton
                  id="delete_outline"
                  onClick={_ =>
                    SettingsAction(State__Settings.RemoveAgenda(agenda))
                    |> dispatch
                  }
                />
              </button>
            </li>
          )
       |> React.array}
    </ul>
  </>;
};
