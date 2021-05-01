open Relude.Globals;
include Shared__Config;

let dotEnvPath =
  switch (Env.nodeEnv) {
  | TESTING => Some("./__tests__/.env")
  | _ => None
  };
let cfg: Dotenv.config = {path: dotEnvPath, encoding: None};
Dotenv.config(~cfg, ());

let workspaces =
  Env.get("WORKSPACES")
  |> Option.map(String.splitList(~delimiter=","))
  |> Option.getOrElse(List.empty);

let secretToken =
  Env.get("SECRET_TOKEN")
  |> Option.tapNone(_ =>
       Logger.err(x => x("SECRET_TOKEN must be defined in .env"))
     )
  |> Option.getOrThrow;
