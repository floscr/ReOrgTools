module Identifier = {
  type t = {
    id: string,
    workspace: int,
  };

  let make = (id, workspace): t => {id, workspace};
};

module File = {
  type content = {
    identifier: Identifier.t,
    text: string,
    ast: ReOrga.orgAst,
    mtimeMs: float,
  };

  type t =
    | Cached(content)
    | Fetched(content)
    | InProgress
    | Empty
    | NotFound
    | Forbidden;
};

module Files = {
  type t = array(File.t);
};
