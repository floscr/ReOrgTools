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
};

module Store = {
  type state = {filesCache: StringMap.t(File.t)};

  let initialState = {filesCache: StringMap.make()};

  type action =
    | FetchProgress(string)
    | FetchSuccess(string, int, Shared__API__File.File.t)
    | FetchFailure(string, ReludeFetch.Error.t(string));

  let reducer = (state, action) => {
    switch (action) {
    | FetchSuccess(id, workspaceIndex, {text, mtimeMs}) =>
      let file =
        File.Fetched({
          id,
          text,
          ast: Org.parseOrga(text, ReOrga.Org.defaultOptions),
          workspace: workspaceIndex,
          mtimeMs,
        });

      Localforage.Localforage_IO.set(
        API__Routes.Routes.fileUrl(~workspaceIndex, ~id),
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
              | File.Fetched(_x) => Some(file)
              | _ => Some(file),
            ),
            state.filesCache,
          ),
      };
    | FetchFailure(error, _) =>
      Js_console.error(error);
      state;
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
