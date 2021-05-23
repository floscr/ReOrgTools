open Express;
open ExpressUtils;
open Relude.Globals;

let t =
  PromiseMiddleware.from((_next, req, res) => {
    Shared__API__User.(
      Request.bodyJSON(req)
      |> IO.fromOption(() => UserError.InvalidJson)
      |> IO.flatMap(json =>
           json
           |> Shared__API__File.Actions.Archive.decodeJson
           |> Result.mapError(_ => UserError.InvalidJson)
           |> IO.fromResult
         )
      // Sanitize Input
      |> IO.flatMap(x => {
           let {filename}: Shared__API__File.Actions.Archive.t = x;
           let filename = filename |> Node_path.normalize;

           (
             switch (filename |> Node_path.isAbsolute) {
             | false => Error(UserError.InvalidPath)
             | _ => Ok({...x, filename})
             }
           )
           |> IO.fromResult;
         })
      // Find the file and make sure it was not written to previously
      |> IO.flatMap(x => {
           let {filename, workspace, mtimeMs}: Shared__API__File.Actions.Archive.t = x;

           List.at(workspace, Config.workspaces)
           |> Option.map(workspacePath =>
                Node.Path.join([|workspacePath, filename|])
              )
           |> IO.fromOption(() => UserError.InvalidPath)
           |> IO.flatMap(
                Api.getFile >> IO.mapError(_ => UserError.InvalidPath),
              )
           |> IO.flatMap((file: Shared__API__File.File.t) => {
                (
                  switch (file.mtimeMs > mtimeMs) {
                  | true => Error(UserError.OutdatedFile)
                  | _ => Ok(file)
                  }
                )
                |> IO.fromResult
              });
         })
      |> resolve(
           res,
           fun
           | Ok(_) => Response.sendStatus(Response.StatusCode.Ok)
           | Error(_) =>
             Response.sendStatus(Response.StatusCode.Unauthorized),
         )
    )
  });
