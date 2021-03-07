open Express;
open ExpressUtils;

let app = express();

App.use(app, Middleware.json());
App.use(app, Middleware.urlencoded(~extended=false, ()));

App.use(app, Cors.t);

App.get(app, ~path="/files", Route__Files.t);
App.get(app, ~path="/file/:id", Route__File.t);

let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    Js.log(e);
    Node.Process.exit(1);
  | _ =>
    Js.log("Listening at http://localhost:" ++ Config.port->string_of_int)
  };

App.listen(app, ~port=Config.port, ~onListen, ());
