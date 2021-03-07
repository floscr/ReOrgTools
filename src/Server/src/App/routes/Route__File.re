open Express;
open ExpressUtils;
open Relude.Globals;

let encode = text => Json.Encode.(object_([("text", string(text))]));

let t =
  PromiseMiddleware.from((_next, req, res) => {
    let id = req |> Request.params |> Js.Dict.get(_, "id");

    let path = Node.Path.join([|Config.orgDir, {j|$id|j}|]) |> Utils.log;

    Api.getFile(path)
    |> resolve(
         res,
         fun
         | Ok(x) => x |> encode |> Response.sendJson
         | _ => Response.sendStatus(Response.StatusCode.NotFound),
       )
    |> Utils.log;
  });
