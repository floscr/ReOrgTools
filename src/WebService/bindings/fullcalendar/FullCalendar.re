open ReOrga;

module Event = {
  type t = {
    title: string,
    date: Js.Date.t,
    orgAst: OrgTypes.Headline.t,
  };
};

module Events = {
  type t = array(Event.t);
};

module Component = {
  [@bs.module "./FullCalendar.Wrapper.js"] [@react.component]
  external make: (~events: Events.t) => React.element = "default";
};
