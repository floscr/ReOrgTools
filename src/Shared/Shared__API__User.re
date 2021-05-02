open Relude.Globals;

module Decode = Decode.AsResult.OfParseError;

module UserError = {
  type t =
    | InvalidJson
    | InvalidCredentials;
};

module User = {
  type t = {
    username: string,
    password: string,
  };

  let make = (username, password) => {username, password};

  let encodeLoginJson =
    Json.Encode.(
      ({username, password}) =>
        object_([
          ("username", string(username)),
          ("password", string(password)),
        ])
    );

  let decodeLoginJson = json =>
    Decode.Pipeline.(
      succeed(make)
      |> field("username", string)
      |> field("password", string)
      |> run(json)
    );
};
