open Relude.Globals;

module Env = {
  let env = Node.Process.process##env;

  let get = Js.Dict.get(env);
  let unsafeGet = Js.Dict.unsafeGet(env);

  type nodeEnvT =
    | PRODUCTION
    | TESTING
    | DEVELOPMENT;

  let parseNodeEnv =
    Option.flatMap(
      (
        fun
        | "dev" => DEVELOPMENT
        | "test" => TESTING
        | _ => PRODUCTION
      )
      >> Option.some,
    )
    >> Option.getOrElse(PRODUCTION);

  let nodeEnv = get("NODE_ENV") |> parseNodeEnv;
};

let dotEnvPath =
  switch (Env.nodeEnv) {
  | TESTING => Some("./__tests__/.env")
  | _ => None
  };
let cfg: Dotenv.config = {path: dotEnvPath, encoding: None};
Dotenv.config(~cfg, ());

module Defaults = {
  let port = 4000;
};

let orgDir = Env.unsafeGet("ORG_DIR");

let workspaces =
  Env.get("WORKSPACES") |> Option.map(String.splitList(~delimiter=","));

let port =
  Env.get("PORT")
  |> Option.flatMap(String.toInt)
  |> Option.getOrElse(Defaults.port);
