open Relude.Globals;
open Express;

let app = express();

App.use(app, Middleware.json());
App.use(app, Middleware.urlencoded(~extended=false, ()));

let resolve = (res, handler, x) =>
  x
  |> Relude.Js.Promise.fromIOWithResult
  |> Js.Promise.(then_(data => res |> handler(data) |> resolve));

App.get(app, ~path="/files") @@
PromiseMiddleware.from((_next, _req, res) => {
  Api.getDirFiles(Config.orgDir)
  |> resolve(
       res,
       fun
       | Ok(xs) => Response.sendArray(xs)
       | _ => Response.sendStatus(Response.StatusCode.NotFound),
     )
});

let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    Js.log(e);
    Node.Process.exit(1);
  | _ =>
    Js.log("Listening at http://localhost:" ++ Config.port->string_of_int)
  };

App.listen(app, ~port=Config.port, ~onListen, ());
