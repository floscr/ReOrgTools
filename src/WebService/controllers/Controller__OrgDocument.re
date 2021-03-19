open API__OrgDocument__Types;
open ReactUtils;
open Relude.Globals;
open ReOrga;

[@react.component]
let make = (~id, ~header, ~send, ~ast) => {
  ReludeReact.Effect.useEffect1WithEq(
    () => {
      State.FetchPagesProgress->send;
      API__OrgDocument.Request.getPageIO(id)
      |> Relude.IO.unsafeRunAsync(
           fun
           | Ok(data) => State.FetchPagesSuccess(data)->send
           | Error(data) => State.FetchPagesFailure(data)->send,
         )
      |> ignore;
    },
    String.eq,
    id,
  );

  switch (ast) {
  | State.FetchedPage({ast}) => <OrgDocument__Root ast header />
  | State.FetchingPage => "Loading" |> s
  | _ => React.null
  };
};
