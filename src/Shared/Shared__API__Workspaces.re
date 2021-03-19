open Relude.Globals;

module Decode = Decode.AsResult.OfParseError;

module File = {
  type t = {
    name: string,
    mtimeMs: float,
  };

  let make = (name, mtimeMs) => {name, mtimeMs};

  let encodeJson =
    Json.Encode.(
      ({name, mtimeMs}) =>
        object_([
          ("name", string(name)),
          ("mtimeMs", Json.Encode.float(mtimeMs)),
        ])
    );

  let decodeJson = json =>
    Decode.Pipeline.(
      succeed(make)
      |> field("name", string)
      |> field("mtimeMs", floatFromNumber)
      |> run(json)
    );
};

module Workspaces = {
  type t = list((string, array(File.t)));

  let encodeJson = xs => {
    Json.Encode.(
      xs
      |> List.toArray
      |> Array.map(((workspace, files)) =>
           object_([
             ("name", string(workspace)),
             ("files", files |> array(File.encodeJson)),
           ])
         )
    );
  };

  let decodeOne = Decode.(tuple(string, array(File.decodeJson)));
  let decodeMany = Decode.(list(decodeOne));
};
