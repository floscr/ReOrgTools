open Relude.Globals;
include Shared__Config;

let dotEnvPath =
  switch (Env.nodeEnv) {
  | TESTING => Some("./__tests__/.env")
  | _ => None
  };
let cfg: Dotenv.config = {path: dotEnvPath, encoding: None};
Dotenv.config(~cfg, ());

let orgDir = Env.unsafeGet("ORG_DIR");

let workspaces =
  Env.get("WORKSPACES")
  |> Option.map(String.splitList(~delimiter=","))
  |> Option.getOrElse(List.empty);
