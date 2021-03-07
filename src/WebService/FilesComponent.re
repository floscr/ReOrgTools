open FilesTypes;
open ReactUtils;

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

module Functor = (Request: FilesAPI.FilesRequest) => {
  [@react.component]
  let make = (~optionsUrl: option(string)=?) => {
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
         |> Array.mapi((i, {name}: FilesTypes.FilesType.t) =>
              <div key=name> {name |> s} </div>
            )
         |> React.array
       }}
    </>;
  };
};
