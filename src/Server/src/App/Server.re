open Express;

let app = express();

App.use(app, Middleware.json());
App.use(app, Middleware.urlencoded(~extended=false, ()));

App.get(app, ~path="/files") @@
Middleware.from((_next, _req) => {
  let files = Api.getFiles() |> Json.Encode.stringArray;

  Json.Encode.(object_([("files", files: Js.Json.t)])) |> Response.sendJson;
});

let port = 4000;
let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    Js.log(e);

    Node.Process.exit(1);

  | _ => Js.log("Listening at http://localhost:" ++ port->string_of_int)
  };

App.listen(app, ~port, ~onListen, ());
