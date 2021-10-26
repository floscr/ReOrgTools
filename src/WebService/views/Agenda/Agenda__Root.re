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
  let make = (~agenda: Types__Agendas.Agenda.t) => {
    let {files, timerange, tags, fields, reverse}: Types__Agendas.Agenda.t = agenda;

    let layoutType =
      fields
      |> Array.find(
           fun
           | Types__Agendas.Field.Layout(_) => true,
         )
      |> Option.map(
           fun
           | Types__Agendas.Field.Layout(x) => x,
         );

    <Controller__OrgDocument
      identifiers=files
      ?timerange
      ?tags
      ?layoutType
      ?reverse
    />;
  };
};

module Component = {
  [@react.component]
  let make = (~id: Types__Agendas.Agenda.idT) => {
    let agendas = Store.useSelector(Selector.Settings.agendas);

    Js.log(agendas);
    Js.log(id);

    agendas
    |> Array.find((agenda: Types__Agendas.Agenda.t) => agenda.id === id)
    |> Option.fold(React.null, agenda =>
         <div className=Styles.root> <OrgWrapper agenda /> </div>
       );
  };
};
