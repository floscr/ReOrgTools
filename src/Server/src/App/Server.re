open Relude.Globals;
open Express;
open ExpressUtils;

let app = express();

App.use(app, Middleware.json());
App.use(app, Middleware.urlencoded(~extended=false, ()));

let _ = Express.App.use(app, Cors.t);

let makeJson = files => {
  Json.Encode.(
    files |> Array.map(name => object_([("name", string(name))]))
  );
};

App.get(app, ~path="/files") @@
PromiseMiddleware.from((_next, _req, res) => {
  Api.getDirFiles(Config.orgDir)
  |> resolve(
       res,
       fun
       | Ok(xs) => xs |> makeJson |> Response.sendArray
       | _ => Response.sendStatus(Response.StatusCode.NotFound),
     )
});

App.get(app, ~path="/file/:id") @@
PromiseMiddleware.from((_next, req, res) => {
  let id = req |> Request.params |> Js.Dict.get(_, "id");
  let path = Node.Path.join([|Config.orgDir, {j|$id|j}|]) |> Utils.log;

  Api.getFile(path)
  |> resolve(
       res,
       fun
       | Ok(x) => Response.sendString(x)
       | _ => Response.sendStatus(Response.StatusCode.NotFound),
     )
  |> Utils.log;
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
