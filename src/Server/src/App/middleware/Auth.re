open Express;
open Relude.Globals;

module TokenError = {
  type t =
    | MissingJwtHeader
    | InvalidString(string)
    | InvalidToken(string);

  let stringify =
    fun
    | MissingJwtHeader => "No JWT found in Header"
    | InvalidString(x) => "Invalid String : " ++ x
    | InvalidToken(x) => "Invalid Token: " ++ x;

  let encodeJson =
    Json.Encode.(err => object_([("error", string(err |> stringify))]));
};

let authenticate =
  Middleware.from((next, req, res) => {
    Request.get("authorization", req)
    |> Result.fromOption(TokenError.MissingJwtHeader)
    // Verify Header
    // authentication headers are passed like this
    // Bearer eyJhbGcizI1NInRVCJ9.eyJz..SNIP..OjEsImzOKnGk-edQsP9QV1INJmZA
    |> Result.map(String.removeFirst(~search="Bearer "))
    |> ExpressUtils.filterResult(String.isNonEmpty, ~err=x =>
         TokenError.InvalidString(x)
       )
    |> ExpressUtils.filterResult(
         x =>
           JsonWebToken.verify(x, `string(Config.secretToken))
           |> Belt.Result.isOk,
         ~err=x => TokenError.InvalidToken(x),
       )
    |> Result.fold(
         err =>
           res
           |> Response.status(Unauthorized)
           |> Response.sendJson(err |> TokenError.encodeJson),
         token => {
           let payload = JsonWebToken.decode(token);
           Js.Dict.set(Request.asJsonObject(req), "user", payload);
           res |> next(Next.middleware);
         },
       )
  });
