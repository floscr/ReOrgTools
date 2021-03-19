open API__OrgDocument__Types;
open ReactUtils;
open Relude.Globals;
open ReOrga;

[@react.component]
let make = (~id, ~header, ~send, ~file) => {
  ReludeReact.Effect.useEffect1WithEq(
    () => {
      State.FetchPagesProgress |> send;
      API__OrgDocument.Request.getPageIO(id)
      |> Relude.IO.unsafeRunAsync(
           fun
           | Ok(data) => State.FetchPagesSuccess(data) |> send
           | Error(data) => State.FetchPagesFailure(data) |> send,
         )
      |> ignore;
    },
    String.eq,
    id,
  );

  file
  |> Option.map((f: State.File.t) =>
       switch (f.status) {
       | State.File.Fetched({ast}) => <OrgDocument__Root ast header />
       | State.File.InProgress => "Loading" |> s
       }
     )
  |> Option.getOrElseLazy(() => React.null);
};
