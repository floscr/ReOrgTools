open ReactUtils;
open Relude.Globals;
open State;

module Styles = {
  open Css;
  open FixedTheme;

  let root =
    style(
      [] /* width(pct(100.)), */
      /* height(pct(100.)), */
      /* display(`flex), */
      /* children([flexGrow(1.), flex(`num(1.))]), */
    );
};

module OrgWrapper = {
  [@react.component]
  let make =
      (
        ~agenda: State__Settings.Agenda.t,
        ~layoutType=Types__Org.Layout.default,
      ) => {
    let {files, timerange, tags}: State__Settings.Agenda.t = agenda;

    <Controller__OrgDocument identifiers=files ?timerange ?tags layoutType />;
  };
};

module Component = {
  [@react.component]
  let make = (~id: State__Settings.Agenda.agendaId) => {
    let agendas = Store.useSelector(Selector.Settings.agendas);

    Js.log(agendas);
    Js.log(id);

    agendas
    |> Array.find((agenda: State__Settings.Agenda.t) => agenda.id === id)
    |> Option.fold(React.null, agenda =>
         <div className=Styles.root> <OrgWrapper agenda /> </div>
       );
  };
};
