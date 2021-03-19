open Express;
open ExpressUtils;
open Relude.Globals;

let makeJson = xs => {
  Json.Encode.
    (
      xs
      |> List.toArray
      |> Array.map(((workspace, files)) =>
           object_([
             ("name", string(workspace)),
             (
               "files",
               files
               |> array(({name, mtimeMs}: Api.file) =>
                    object_([
                      ("name", string(name)),
                      ("mtimeMs", Json.Encode.float(mtimeMs)),
                    ])
                  ),
             ),
           ])
         )
    );
    /* [| */
    /*   workspace, */
    /*   Array.map( */
    /*     ({name, mtimeMs}: Api.file) => */
    /*       object_([("name", string(name))]), */
    /*     files, */
    /*   ), */
    /* |] */
};

let t =
  PromiseMiddleware.from((_next, _req, res) => {
    Api.getWorkspaces()
    |> resolve(
         res,
         fun
         | Ok(xs) => xs |> makeJson |> Response.sendArray
         | _ => Response.sendStatus(Response.StatusCode.NotFound),
       )
  });
