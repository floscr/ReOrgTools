open Express;

let app = express();

[@bs.module] external cors: unit => Express.Middleware.t = "cors";

App.use(app, Middleware.json());
App.use(app, Middleware.urlencoded(~extended=false, ()));

App.use(app, cors());

// Api Routes
App.getWithMany(
  app,
  ~path="/api/file/:workspaceIndex/:id.json",
  [|Route__File.t|],
);
App.getWithMany(
  app,
  ~path="/api/workspaces",
  [|Auth.authenticate, Route__Workspaces.t|],
);
App.get(app, ~path="/api/file/*/*", Route__Home.make);
App.useOnPath(
  app,
  ~path="/api/attachments",
  {
    let options = Static.defaultOptions();
    Static.make("/home/floscr/Documents/Org/.attach", options)
    |> Static.asMiddleware;
  },
);

// Static Routes
App.get(app, ~path="/", Route__Home.make);
App.get(app, ~path="/file/:workspaceIndex/:id", Route__Home.make);
App.useOnPath(
  app,
  ~path="/",
  {
    let options = Static.defaultOptions();
    Static.make(Node.Process.cwd() ++ "/public", options)
    |> Static.asMiddleware;
  },
);

let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    Js.log(e);
    Node.Process.exit(1);
  | _ =>
    Js.log(
      "Listening at "
      ++ (
        Shared__Config.backendUrl(~path=ReludeURL.Path.make([]), ())
        |> ReludeURL.URI.show
      ),
    )
  };

App.listen(app, ~port=Config.backendPort, ~onListen, ());
