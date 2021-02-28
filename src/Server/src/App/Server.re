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
       | Ok(xs) => {
           Js.log(xs);
           Response.sendString(
             Array.String.joinWith(", \n", xs) |> (x => {j|$x foo|j}),
           );
         }
       | Error(Api.ReadDirectoryError(_)) =>
         Response.sendStatus(Response.StatusCode.NotFound)
       | _ => Response.sendStatus(Response.StatusCode.NotFound),
     )
  /* |> IO.unsafeRunAsync( */
  /*      fun */
  /*      | Ok(_) => res |> Response.sendString("Yup") */
  /*      | Error(e) => res |> Response.sendStatus(Response.StatusCode.NotFound), */
  /*    ) */
  /* res |> Response.sendString("Yup"); */
  /* |> Json.Encode.stringArray; */
  /* Json.Encode.(object_([("files", files: Js.Json.t)])) |> Response.sendJson; */
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
