open Express;
open ExpressUtils;
open Relude.Globals;

let make =
  Middleware.from((_, _, res) => {
    res |> Response.sendFile(Node.Process.cwd() ++ "/public/index.html", ())
  });
