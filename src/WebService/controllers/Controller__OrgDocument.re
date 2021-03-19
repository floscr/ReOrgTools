open API__OrgDocument__Types;
open ReactUtils;
open Relude.Globals;
open ReOrga;
open ReductiveStore;

[@react.component]
let make = (~id, ~header, ~workspaceIndex) => {
  let dispatch = Store.useDispatch();

  let files = Store.useSelector(Selector.OrgDocuments.files);
  let file = StringMap.get(id, files);

  ReludeReact.Effect.useEffect1WithEq(
    () => {
      OrgDocumentsAction(
        ReductiveStore__OrgDocuments.Store.FetchProgress(id),
      )
      |> dispatch;

      API__OrgDocument.Request.make(~workspaceIndex, ~file=id)
      |> IO.tap(
           Localforage.Localforage_IO.set(
             Stores.OrgDocument.File.makeForageId(
               ~id,
               ~workspace=workspaceIndex,
             ),
           )
           >> IO.unsafeRunAsync(ignore),
         )
      |> IO.unsafeRunAsync(
           fun
           | Ok(data) =>
             OrgDocumentsAction(
               ReductiveStore__OrgDocuments.Store.FetchSuccess(
                 id,
                 workspaceIndex,
                 data,
               ),
             )
             |> dispatch

           | Error(data) =>
             OrgDocumentsAction(
               ReductiveStore__OrgDocuments.Store.FetchFailure(id, data),
             )
             |> dispatch,
         )
      |> ignore;
    },
    String.eq,
    id,
  );

  file
  |> Option.map(x =>
       ReductiveStore__OrgDocuments.(
         switch ((x: File.t)) {
         | File.Fetched({ast}) => <OrgDocument__Root ast header />
         | File.Cached({ast}) => <OrgDocument__Root ast header />
         | File.InProgress => "Loading" |> s
         | _ => React.null
         }
       )
     )
  |> Option.getOrElseLazy(() => React.null);
};
