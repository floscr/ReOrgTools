type t;

[@bs.new] [@bs.module] external init: Dom.element => t = "combokeys";

[@bs.module] external initPlayPausePlugin: t => t = "combokeys/plugins/pause";

[@bs.module]
external initGlobalBindPlugin: t => t = "combokeys/plugins/global-bind";

[@bs.send.pipe: t]
external bindGlobal: (string, Dom.keyboardEvent => bool) => unit =
  "bindGlobal";

[@bs.send.pipe: t]
external bindGlobalArray: (array(string), Dom.keyboardEvent => bool) => unit =
  "bindGlobal";

[@bs.send.pipe: t] external pause: unit => unit = "pause";

[@bs.send.pipe: t] external unpause: unit => unit = "unpause";

[@bs.send.pipe: t]
external bind: (string, Dom.keyboardEvent => bool) => unit = "";

[@bs.send.pipe: t]
external bindArray: (array(string), Dom.keyboardEvent => bool) => unit =
  "bind";

[@bs.send.pipe: t] external detach: unit => unit = "";
