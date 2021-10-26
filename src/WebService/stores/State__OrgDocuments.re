open ReOrga;
open Relude.Globals;

module Store = {
  type state = {filesCache: StringMap.t(Types__Files.File.t)};

  let initialState = {filesCache: StringMap.make()};

  type action =
    | FetchProgress(string)
    | FetchSuccess(string, int, Shared__API__File.File.t)
    | FetchFailure(string, ReludeFetch.Error.t(string));

  let reducer = (state, action) => {
    switch (action) {
    | FetchSuccess(id, workspaceIndex, {text, mtimeMs}) =>
      let file =
        Types__Files.File.Fetched({
          identifier: {
            id,
            workspace: workspaceIndex,
          },
          text,
          ast: Org.parseOrga(text, ReOrga.Org.defaultOptions),
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
              | Types__Files.File.Fetched(_x) => Some(file)
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
              _ => Some(Types__Files.File.InProgress),
              _ => Some(Types__Files.File.InProgress),
            ),
            state.filesCache,
          ),
      }
    };
  };
};
