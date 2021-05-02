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

  let encodeLoginJWTJson =
    Json.Encode.(
      ({username}) => object_([("username", string(username))])
    );

  let decodeLoginJson = json =>
    Decode.Pipeline.(
      succeed(make)
      |> field("username", string)
      |> field("password", string)
      |> run(json)
    );
};

module Token = {
  type t = {token: string};

  let make = token => {token: token};

  let encodeUser =
    Json.Encode.(
      ({username}: User.t) => object_([("username", string(username))])
    );

  let encode =
    Json.Encode.(({token}) => object_([("token", string(token))]));

  let decode = json =>
    Decode.Pipeline.(succeed(make) |> field("token", string) |> run(json));
};
