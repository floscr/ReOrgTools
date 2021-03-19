open ReOrga;
open Relude.Globals;

module File = {
  type content = {
    text: string,
    ast: ReOrga.orgAst,
    workspace: string,
  };

  type t =
    | Fetched(content)
    | InProgress
    | Empty
    | NotFound
    | Forbidden;
};

type globalState = {filesCache: StringMap.t(File.t)};

let initialGlobalState = {filesCache: StringMap.make()};

type action =
  | FetchPagesProgress(string)
  | FetchPagesSuccess(string, API__OrgDocument__Types.OrgDocumentType.t)
  | FetchPagesFailure(string, ReludeFetch.Error.t(string))
  | NoOp;

let actionToName =
  fun
  | FetchPagesProgress(_) => "FetchPagesProgress"
  | FetchPagesSuccess(_) => "FetchPagesSuccess"
  | FetchPagesFailure(_) => "FetchPagesFailure"
  | NoOp => "NoOp";

let reducer =
    (state: globalState, action: action)
    : ReludeReact.Reducer.update(action, globalState) => {
  /* Js.log2(state, action |> actionToName); */
  switch (action) {
  | FetchPagesSuccess(id, {text}) =>
    let file =
      File.Fetched({
        text,
        ast: Org.parseOrga(text, {todo: Some([|"TODO"|])}),
        workspace: "",
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
