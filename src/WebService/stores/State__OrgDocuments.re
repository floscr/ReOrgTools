open ReOrga;
open Relude.Globals;

module File = {
  type content = {
    id: string,
    text: string,
    ast: ReOrga.orgAst,
    workspace: int,
    mtimeMs: float,
  };

  type t =
    | Cached(content)
    | Fetched(content)
    | InProgress
    | Empty
    | NotFound
    | Forbidden;

  let makeForageId = (~id, ~workspace) => {j|$workspace/$id|j};
};

module Store = {
  type state = {filesCache: StringMap.t(File.t)};

  let initialState = {filesCache: StringMap.make()};

  type action =
    | FetchProgress(string)
    | FetchSuccess(string, int, Shared__API__File.File.t)
    | FetchFailure(string, ReludeFetch.Error.t(string))
    | Cache(string);

  let reducer = (state, action) => {
    switch (action) {
    | FetchSuccess(id, workspaceIndex, {text, mtimeMs}) =>
      let file =
        File.Fetched({
          id,
          text,
          ast: Org.parseOrga(text, {todo: Some([|"TODO"|])}),
          workspace: workspaceIndex,
          mtimeMs,
        });

      Localforage.Localforage_IO.set(
        File.makeForageId(~id, ~workspace=workspaceIndex),
        text,
      )
      |> IO.unsafeRunAsync(ignore);

      {
        filesCache:
          StringMap.update(
            id,
            Option.foldLazy(
              _ => Some(file),
              fun
              | File.Fetched(x) => Some(file)
              | _ => Some(file),
            ),
            state.filesCache,
          ),
      };
    | FetchProgress(id) => {
        filesCache:
          StringMap.update(
            id,
            Option.foldLazy(
              _ => Some(File.InProgress),
              _ => Some(File.InProgress),
            ),
            state.filesCache,
          ),
      }
    };
  };
};