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
           |> User.decodeLoginJson
           |> Result.mapError(_ => UserError.InvalidJson)
           |> IO.pure
         )
      |> Utils.log
      |> resolve(
           res,
           fun
           | Ok(x) => Response.sendStatus(Response.StatusCode.Unauthorized)
           | _ => Response.sendStatus(Response.StatusCode.Unauthorized),
         )
    )
  });
