type t;

[@bs.module "./Combokeys.Wrapper.js"] external init: Dom.element => t = "init";

module PausePlugin = {
  [@bs.module "./Combokeys.Pause.Wrapper.js"]
  external initPlayPausePlugin: t => t = "init";
};

module GlobalBindPlugin = {
  [@bs.module "./Combokeys.GlobalBind.Wrapper.js"]
  external initGlobalBindPlugin: t => t = "init";
};

[@bs.send.pipe: t]
external bindGlobal: (string, Dom.keyboardEvent => bool) => unit =
  "bindGlobal";

[@bs.send.pipe: t]
external bindGlobalArray: (array(string), Dom.keyboardEvent => bool) => unit =
  "bindGlobal";

[@bs.send.pipe: t] external pause: unit => unit = "pause";

[@bs.send.pipe: t] external unpause: unit => unit = "unpause";

[@bs.send.pipe: t]
external bind: (string, Dom.keyboardEvent => bool) => unit = "bind";

[@bs.send.pipe: t]
external bindArray: (array(string), Dom.keyboardEvent => bool) => unit =
  "bind";

[@bs.send.pipe: t] external detach: unit => unit = "detach";
