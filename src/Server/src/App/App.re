open Koa;

let app = koa();
let port = 3000;

let middleware = ctx => {
  Js.log(ctx);
  ctx#body := "hello";
};

use(app, middleware);

listen(~port, app);
