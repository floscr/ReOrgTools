open API__OrgDocument__Types;
open ReactUtils;
open Relude.Globals;
open ReOrga;
open ReductiveStore;

[@react.component]
let make = (~id, ~header, ~workspaceIndex) => {
  open ReductiveStore__OrgDocuments;

  let dispatch = ReductiveStore.Wrapper.useDispatch();

  let files = Wrapper.useSelector(Selector.OrgDocumentsStore.files);
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
             File.makeForageId(~id, ~workspace=workspaceIndex),
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
       switch ((x: File.t)) {
       | ReductiveStore__OrgDocuments.File.Fetched({ast}) =>
         <OrgDocument__Root ast header />
       | ReductiveStore__OrgDocuments.File.Cached({ast}) =>
         <OrgDocument__Root ast header />
       | ReductiveStore__OrgDocuments.File.InProgress => "Loading" |> s
       | _ => React.null
       }
     )
  |> Option.getOrElseLazy(() => React.null);
};
