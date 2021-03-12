open FilesTypes;
open ReactUtils;
open Relude.Globals;

type state = {files: option(array(FilesType.t))};

let initialState = {files: None};

module Styles = {
  open Css;
};

type action =
  | FetchFilesSuccess(array(FilesTypes.FilesType.t))
  | FetchFilesFailure(ReludeFetch.Error.t(string))
  | NoOp;

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | FetchFilesSuccess(files) => Update({...state, files: Some(files)})
  | NoOp => NoUpdate
  | _ => NoUpdate
  };

let makeName = name =>
  name
  |> String.splitAsArray(~delimiter=".org")
  |> Array.head
  |> Option.getOrElse(name);

module Functor = (Request: FilesAPI.FilesRequest) => {
  [@react.component]
  let make = (~optionsUrl: option(string)=?, ~onFileClick) => {
    let ({files}: state, send) =
      ReludeReact.Reducer.useReducer(reducer, initialState);

    ReludeReact.Effect.useIOOnMount(
      Request.getFilesIO(optionsUrl),
      options => FetchFilesSuccess(options)->send,
      error => FetchFilesFailure(error)->send,
    );

    <>
      {switch (files) {
       | None => "No files found" |> s
       | Some(files) =>
         files
         |> Array.reject(({name}: FilesTypes.FilesType.t) =>
              String.contains(~search=".sync-conflict", name)
            )
         |> Array.map(({name}: FilesTypes.FilesType.t) => {
              let base = name |> makeName;
              <button
                key=name
                onClick={_ => {
                  ReasonReactRouter.push("/file/" ++ base);
                  onFileClick();
                }}>
                {base |> s}
              </button>;
            })
         |> React.array
       }}
    </>;
  };
};
