open Express;
open ExpressUtils;
open Relude.Globals;

let makeJson = files => {
  Json.Encode.(
    files |> Array.map(name => object_([("name", string(name))]))
  );
};

let t =
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
