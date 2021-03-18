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
