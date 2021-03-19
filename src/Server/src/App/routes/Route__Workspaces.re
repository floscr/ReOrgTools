open Express;
open ExpressUtils;
open Relude.Globals;

let makeJson = files => {
  Json.Encode.(
    files
    |> Array.map(({name, mtimeMs}: Api.file) =>
         object_([("name", string(name))])
       )
  );
};

let t =
  PromiseMiddleware.from((_next, _req, res) => {
    Api.getDirFiles(Config.orgDir)
    |> resolve(
         res,
         fun
         | Ok(xs) => xs |> makeJson |> Response.sendArray
         | _ => Response.sendStatus(Response.StatusCode.NotFound),
       )
  });
