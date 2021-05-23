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

module Actions = {
  module Archive = {
    type t = {
      filename: string,
      workspace: int,
      mtimeMs: float,
      position: (int, int),
    };

    type action =
      | Archive(t);

    let make = (filename, workspace, mtimeMs, position) => {
      filename,
      workspace,
      mtimeMs,
      position,
    };

    let encodeJson =
      Json.Encode.(
        ({filename, workspace, mtimeMs, position}) => {
          let (positionFrom, positionTo) = position;

          object_([
            ("filename", string(filename)),
            ("workspace", int(workspace)),
            ("mtimeMs", Json.Encode.float(mtimeMs)),
            ("position", Json.Encode.tuple2(int, int, position)),
          ]);
        }
      );

    let decodeJson = json =>
      Decode.Pipeline.(
        succeed(make)
        |> field("filename", string)
        |> field("workspace", intFromNumber)
        |> field("mtimeMs", floatFromNumber)
        |> field("position", tuple2(intFromNumber, intFromNumber))
        |> run(json)
      );
  };
};
