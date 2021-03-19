open Express;
open ExpressUtils;
open Relude.Globals;

let encode = text => Json.Encode.(object_([("text", string(text))]));

let t =
  PromiseMiddleware.from((_next, req, res) => {
    let file =
      req
      |> Request.params
      |> Js.Dict.get(_, "id")
      |> Option.flatMap(Js.Json.decodeString);
    let workspacePath =
      req
      |> Request.params
      |> Js.Dict.get(_, "workspaceIndex")
      |> Option.flatMap(Js.Json.decodeString)
      |> Option.flatMap(String.toInt)
      |> Option.flatMap(x => List.at(x, Config.workspaces));
    let path =
      List.Option.traverse(x => x, [workspacePath, file])
      |> Option.map(List.toArray)
      |> Option.map(Node.Path.join);

    path
    |> IO.fromOption(() => Api.ReadWordkspaceError)
    |> IO.flatMap(Api.getFile)
    |> resolve(
         res,
         fun
         | Ok(x) => x |> encode |> Response.sendJson
         | _ => Response.sendStatus(Response.StatusCode.NotFound),
       )
    |> Utils.log;
  });
