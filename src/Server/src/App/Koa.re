module Koa = {
  type t;
  type ctx = {. body: ref(string)};

  let default = "koa";

  [@bs.module "koa"] [@bs.new "default"] external make: unit => t = "koa";
  [@bs.send] external listen: (t, int) => unit = "listen";
  [@bs.send] external use: (t, [@bs.uncurry] (ctx => unit)) => unit = "use";
};

let koa = Koa.make;

let listen = (~port=3000, app) => Koa.listen(app, port);

let use = Koa.use;

module KoaRouter = {
  type t;

  [@bs.module "koa-router"] [@bs.new "default"]
  external make: unit => t = "koa-router";

  [@bs.send] external get: (t, [@bs.uncurry] ()) => unit = "get";
};

let koaRouter = KoaRouter.make;

Js_global.decodeURIComponent
