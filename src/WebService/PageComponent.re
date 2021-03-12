open PageTypes;
open ReactUtils;
open Relude.Globals;
open ReOrga;

type loadedState = {page: ReOrga.orgAst};

type state =
  | Loaded(loadedState)
  | Loading
  | Initial;

let initialState = Loading;

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
    Update(Loaded({page: Org.parseOrga(text, {todo: Some([|"TODO"|])})}))
  | FetchPagesSuccess({text}) =>
    Update(Loaded({page: Org.parseOrga(text, {todo: Some([|"TODO"|])})}))
  | NoOp => NoUpdate
  | _ => NoUpdate
  };

let testDoc = TestContent.orga |> Utils.log;

[@react.component]
let make = (~id, ~header) => {
  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);

  id
  |> Option.tap(id =>
       ReludeReact.Effect.useIOOnMount(
         PageAPI.PageRequest.getPageIO(id),
         data => FetchPagesSuccess(data)->send,
         error => FetchPagesFailure(error)->send,
       )
     );

  switch (state) {
  | Loaded({page}) => <Page doc=page header />
  | Loading => "Loading" |> s
  | Initial => React.null
  };
};
