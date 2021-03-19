let s = React.string;

[@bs.send] external performanceNow: Webapi__Performance.t => float = "now";

/* Logs the time (in ms) it takes to run a function, using the DOM Performance API. */
[@live]
let measure = f => {
  let performance = Webapi.Dom.window |> Webapi.Dom.Window.performance;
  let t0 = performanceNow(performance);
  let v = f();
  let t1 = performanceNow(performance);
  Js.log("Measurement: " ++ (t1 -. t0 |> Js.Float.toString) ++ "ms.");
  v;
};

module Icon = {
  [@react.component]
  let make = (~id) => {
    <span className="material-icons"> {id |> s} </span>;
  };
};

module Styles = {
  open Css;
  let iconButton =
    style([
      lineHeight(`abs(1.)),
      selector(
        ":focus",
        [outlineStyle(none), color(Theme.BaseTheme.purple)],
      ),
    ]);
};

module IconButton = {
  let buttonClassName = Styles.iconButton;

  [@react.component]
  let make = (~id, ~style=?) => {
    let className =
      switch (style) {
      | Some(x) => buttonClassName ++ " " ++ x
      | _ => buttonClassName
      };
    <button className> <Icon id /> </button>;
  };
};
