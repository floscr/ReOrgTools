open API__OrgDocument__Types;
open ReactUtils;
open Relude.Globals;
open ReOrga;

[@react.component]
let make = (~id, ~header, ~send, ~file, ~workspaceIndex) => {
  ReludeReact.Effect.useEffect1WithEq(
    () => {
      State.FetchPagesProgress(id) |> send;

      API__OrgDocument.Request.make(~workspaceIndex, ~file=id)
      |> IO.tap(
           Localforage.Localforage_IO.set(
             State.File.makeForageId(~id, ~workspace=workspaceIndex),
           )
           >> IO.unsafeRunAsync(ignore),
         )
      |> IO.unsafeRunAsync(
           fun
           | Ok(data) =>
             State.FetchPagesSuccess(id, workspaceIndex, data) |> send
           | Error(data) => State.FetchPagesFailure(id, data) |> send,
         )
      |> ignore;
    },
    String.eq,
    id,
  );

  file
  |> Option.map(x =>
       switch ((x: State.File.t)) {
       | State.File.Fetched({ast}) => <OrgDocument__Root ast header />
       | State.File.Cached({ast}) => <OrgDocument__Root ast header />
       | State.File.InProgress => "Loading" |> s
       | _ => React.null
       }
     )
  |> Option.getOrElseLazy(() => React.null);
};
