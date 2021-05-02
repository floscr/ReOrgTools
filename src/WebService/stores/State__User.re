open Relude.Globals;
open ReactUtils;

module Decode = Decode.AsResult.OfParseError;

module User = {
  type t = {
    jwt: Shared__API__User.Token.t,
    username: string,
  };

  let make = (jwt, username) => {jwt, username};

  let decodeLoginJson = (json, username) =>
    Shared__API__User.Token.decode(json)
    |> Result.map(jwt => {jwt, username});
};

type user =
  | LoggedInUser(User.t)
  | Unauthorized;

type state = {user};

let initialState = {user: Unauthorized};

type action =
  | Login(User.t)
  | Logout;

let reducer = (state, action) => {
  switch (action) {
  | Login(user) => {user: LoggedInUser(user)}
  | Logout => {user: Unauthorized}
  };
};

module LoginResponse = {
  let decode = (~url, ~user, json) =>
    User.decodeLoginJson(json, user)
    |> Relude.IO.fromResult
    |> Relude.IO.mapRight(e => {
         let readableError = Decode.ParseError.failureToDebugString(e);
         ReludeFetch.Error.decodeError({url, innerError: readableError});
       });
};

module type LoginRequest = {
  let make:
    Shared__API__User.User.t =>
    Relude.IO.t(User.t, ReludeFetch.Error.t(string));
};

module LoginRequest: LoginRequest = {
  open Relude.IO;
  module Headers = ReludeFetch.Headers;

  let url =
    ReludeURL.(
      Shared__Config.backendUrl(
        ~path=
          Path.make([PathSegment.make("api"), PathSegment.make("login")]),
        (),
      )
    )
    |> ReludeURL.URI.show;

  let make = (user: Shared__API__User.User.t) => {
    ReludeFetch.post(
      ~headers=Headers.contentTypeJson |> Headers.combine(Headers.acceptJson),
      ~body=
        Shared__API__User.User.encodeLoginJson(user)
        |> (x => ReludeFetch.Body.Content.Json(x)),
      url,
    )
    >>= ReludeFetch.Response.StatusCode.ensure2xx
    >>= ReludeFetch.Response.json
    >>= LoginResponse.decode(~url, ~user=user.username);
  };
};
