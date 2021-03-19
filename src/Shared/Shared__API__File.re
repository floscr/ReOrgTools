open Relude.Globals;

module Decode = Decode.AsResult.OfParseError;

module File = {
  type t = {
    text: string,
    mtimeMs: float,
  };

  let make = (text, mtimeMs) => {text, mtimeMs};

  let encodeJson =
    Json.Encode.(
      ({text, mtimeMs}) =>
        object_([
          ("text", string(text)),
          ("mtimeMs", Json.Encode.float(mtimeMs)),
        ])
    );

  let decodeJson = json =>
    Decode.Pipeline.(
      succeed(make)
      |> field("text", string)
      |> field("mtimeMs", floatFromNumber)
      |> run(json)
    );
};
