module Component = {
  [@bs.module "./FullCalendar.Wrapper.js"] [@react.component]
  external make: unit => React.element = "default";
};
