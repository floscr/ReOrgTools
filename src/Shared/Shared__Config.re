open Relude.Globals;

let stringifyExn = v => Js.Json.stringifyAny(v) |> Option.getOrThrow;

module Logger = {
  let level = 3;
  type msgf('a) =
    (format4('a, Format.formatter, unit, string) => 'a) => string;

  let _base: msgf('a) => string =
    msgf => msgf(fmt => Format.asprintf(fmt ^^ "@."));

  let trace = k => level > 4 ? _base(k) |> Js.Console.log : ();
  let debug = k => level > 3 ? _base(k) |> Js.Console.log : ();
  let log = debug;
  let info = k => level > 2 ? _base(k) |> Js.Console.info : ();
  let warn = k => level > 1 ? _base(k) |> Js.Console.warn : ();
  let err = k => level > 0 ? _base(k) |> Js.Console.error : ();
};

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

  let isProduction =
    nodeEnv
    |> (
      fun
      | PRODUCTION => true
      | _ => false
    );
};

module Defaults = {
  let frontendPort = 8080;
  let backendPort = 4000;
};

let frontendPort =
  Env.get("FRONTEND_PORT")
  |> Option.flatMap(String.toInt)
  |> Option.getOrElse(Defaults.frontendPort);

let backendPort =
  Env.get("BACKEND_PORT")
  |> Option.flatMap(String.toInt)
  |> Option.getOrElse(Defaults.backendPort);

let frontendUrl =
  switch (Env.nodeEnv) {
  | _ =>
    ReludeURL.(
      URI.makeWithLabels(
        ~scheme=Scheme("http"),
        ~authority=
          Authority.fromHostnameAndPort(
            Hostname.make("localhost"),
            Port.make(frontendPort),
          ),
      )
    )
  };

let backendUrl =
  switch (Env.nodeEnv) {
  | _ =>
    ReludeURL.(
      URI.makeWithLabels(
        ~scheme=Scheme("http"),
        ~authority=
          Authority.fromHostnameAndPort(
            Hostname.make("localhost"),
            Port.make(backendPort),
          ),
      )
    )
  };

let intToBool = x =>
  String.toInt(x)
  |> Option.map(
       fun
       | 1 => true
       | _ => false,
     );

let shouldLog =
  Env.get("ENABLE_LOGGING")
  |> Option.flatMap(intToBool)
  |> Option.getOrElse(false);

let log = shouldLog ? Js.log : ignore;
