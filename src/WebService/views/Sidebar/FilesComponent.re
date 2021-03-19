open FilesTypes;
open ReactUtils;
open Relude.Globals;

type state = {files: option(array(FilesType.t))};

let initialState = {files: None};

module Styles = {
  open Css;

  let button =
    style([
      padding2(~v=Theme.Spacing.xsmall, ~h=Theme.Spacing.large),
      lineHeight(`abs(1.)),
      borderRadius(px(50)),
      cursor(pointer),
      width(pct(100.)),
      fontWeight(medium),
      textAlign(`left),
      transition(~duration=Theme.Timing.hoverShort, "all"),
      selector(":hover", [backgroundColor(Theme.BaseTheme.grey2)]),
      selector(
        ":focus",
        [
          outlineStyle(none),
          borderRadius(px(50)),
          boxShadow(
            Shadow.box(~spread=px(3), ~inset=true, rgba(0, 0, 0, pct(7.))),
          ),
        ],
      ),
    ]);
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

    let fileCompare =
      Ord.by(
        ({name}: FilesTypes.FilesType.t) => name |> String.toLowerCase,
        String.compare,
      );

    <>
      {switch (files) {
       | None => "No files found" |> s
       | Some(files) =>
         files
         |> Array.reject(({name}: FilesTypes.FilesType.t) =>
              String.contains(~search=".sync-conflict", name)
            )
         |> Array.sortBy(fileCompare)
         |> Array.map(({name}: FilesTypes.FilesType.t) => {
              let base = name |> makeName;
              <button
                key=name
                className=Styles.button
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
