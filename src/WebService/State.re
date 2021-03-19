open ReOrga;
open Relude.Globals;

module File = {
  type content = {
    text: string,
    ast: ReOrga.orgAst,
    workspace: string,
    mtimeMs: float,
  };

  type t =
    | Fetched(content)
    | InProgress
    | Empty
    | NotFound
    | Forbidden;
};

type globalState = {
  workspaces: Shared__API__Workspaces.Workspaces.t,
  filesCache: StringMap.t(File.t),
};

let initialGlobalState = {workspaces: [], filesCache: StringMap.make()};

type action =
  | FetchWorkspacesSuccess(Shared__API__Workspaces.Workspaces.t)
  | FetchWorkspacesFailure(ReludeFetch.Error.t(string))
  | FetchPagesProgress(string)
  | FetchPagesSuccess(string, Shared__API__File.File.t)
  | FetchPagesFailure(string, ReludeFetch.Error.t(string))
  | NoOp;

let actionToName =
  fun
  | FetchWorkspacesSuccess(_) => "FetchWorkspacesSuccess"
  | FetchWorkspacesFailure(_) => "FetchWorkspacesFailure"
  | FetchPagesProgress(_) => "FetchPagesProgress"
  | FetchPagesSuccess(_) => "FetchPagesSuccess"
  | FetchPagesFailure(_) => "FetchPagesFailure"
  | NoOp => "NoOp";

let reducer =
    (state: globalState, action: action)
    : ReludeReact.Reducer.update(action, globalState) => {
  /* Js.log2(state, action |> actionToName); */
  switch (action) {
  | FetchWorkspacesSuccess(workspaces) => Update({...state, workspaces})
  | FetchPagesSuccess(id, {text, mtimeMs}) =>
    let file =
      File.Fetched({
        text,
        ast: Org.parseOrga(text, {todo: Some([|"TODO"|])}),
        workspace: "",
        mtimeMs,
      });
    Update({
      ...state,
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
    });
  | FetchPagesProgress(id) =>
    Update({
      ...state,
      filesCache:
        StringMap.update(
          id,
          Option.foldLazy(
            _ => Some(File.InProgress),
            _ => Some(File.InProgress),
          ),
          state.filesCache,
        ),
    })
  | NoOp => NoUpdate
  | _ => NoUpdate
  };
};
