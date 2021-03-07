open PageTypes;
open ReactUtils;
open Relude.Globals;
open ReOrga;

type state = {page: option(ReOrga.orgAst)};

let initialState = {page: None};

module Styles = {
  open Css;
};

type action =
  | FetchPagesSuccess(PageTypes.PageType.t)
  | FetchPagesFailure(ReludeFetch.Error.t(string))
  | NoOp;

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | FetchPagesSuccess({text}) =>
    Update({
      ...state,
      page: Some(Org.parseOrga(text, {todo: Some([|"TODO"|])})),
    })
  | NoOp => NoUpdate
  | _ => NoUpdate
  };

let testDoc = TestContent.orga |> Utils.log;

[@react.component]
let make = (~id) => {
  let ({page}: state, send) =
    ReludeReact.Reducer.useReducer(reducer, initialState);

  id
  |> Option.tap(id =>
       ReludeReact.Effect.useIOOnMount(
         PageAPI.PageRequest.getPageIO(id),
         options => FetchPagesSuccess(options)->send,
         error => FetchPagesFailure(error)->send,
       )
     );

  Js.log(page);

  switch (page) {
  | Some(doc) => <Page doc />
  | _ => React.null
  };
};
