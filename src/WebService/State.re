open ReOrga;
open Relude.Globals;

module File = {
  type content = {
    text: string,
    ast: ReOrga.orgAst,
  };

  type status =
    | Fetched(content)
    | InProgress
    | Empty
    | NotFound
    | Forbidden;

  type t = {
    name: string,
    status,
  };
};

type globalState = {files: array(File.t)};

let initialGlobalState = {files: [||]};

type action =
  | FetchPagesProgress
  | FetchPagesSuccess(API__OrgDocument__Types.OrgDocumentType.t)
  | FetchPagesFailure(ReludeFetch.Error.t(string))
  | NoOp;

let actionToName =
  fun
  | FetchPagesProgress => "FetchPagesProgress"
  | FetchPagesSuccess(_) => "FetchPagesSuccess"
  | FetchPagesFailure(_) => "FetchPagesFailure"
  | NoOp => "NoOp";

let reducer =
    (state: globalState, action: action)
    : ReludeReact.Reducer.update(action, globalState) => {
  Js.log2(state, action |> actionToName);
  switch (action) {
  | FetchPagesSuccess({text}) =>
    Update({
      ...state,
      files: [|
        {
          name: "",
          status:
            File.Fetched({
              text,
              ast: Org.parseOrga(text, {todo: Some([|"TODO"|])}),
            }),
        },
      |],
    })
  | FetchPagesProgress =>
    Update({...state, files: [|{name: "", status: File.InProgress}|]})
  | NoOp => NoUpdate
  | _ => NoUpdate
  };
};
