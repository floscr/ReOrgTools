open Express;
open ExpressUtils;
open Relude.Globals;

let t =
  PromiseMiddleware.from((_next, _req, res) => {
    Api.getWorkspaces()
    |> resolve(
         res,
         fun
         | Ok(xs) =>
           xs
           |> Shared__API__Workspaces.Workspaces.encodeJson
           |> Response.sendArray
         | _ => Response.sendStatus(Response.StatusCode.NotFound),
       )
  });
