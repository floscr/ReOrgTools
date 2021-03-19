open Relude.Globals;
Dotenv.config();

module Defaults = {
  let port = 4000;
};

let env = Node.Process.process##env;

let orgDir = Js.Dict.unsafeGet(env, "ORG_DIR");

let workspaces =
  Js.Dict.get(env, "WORKSPACES")
  |> Option.map(String.splitList(~delimiter=","));

let port =
  Js.Dict.get(env, "PORT")
  |> Option.flatMap(String.toInt)
  |> Option.getOrElse(Defaults.port);
