open PageTypes;
open ReactUtils;
open Relude.Globals;
open ReOrga;

[@react.component]
let make = (~id, ~header, ~send, ~page) => {
  id
  |> Option.tap(id =>
       ReludeReact.Effect.useIOOnMount(
         PageAPI.PageRequest.getPageIO(id),
         data => State.FetchPagesSuccess(data)->send,
         error => State.FetchPagesFailure(error)->send,
       )
     );

  switch (page) {
  | State.FetchedPage({ast}) => <Page doc=ast header />
  | State.FetchingPage => "Loading" |> s
  | _ => React.null
  };
};
