open Relude.Globals;

module File = {
  type t = {
    name: string,
    mtimeMs: float,
  };

  let encodeJson =
    Json.Encode.(
      ({name, mtimeMs}) =>
        object_([
          ("name", string(name)),
          ("mtimeMs", Json.Encode.float(mtimeMs)),
        ])
    );
};

module Workspaces = {
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
};
