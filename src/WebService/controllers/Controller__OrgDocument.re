open API__OrgDocument__Types;
open ReactUtils;
open Relude.Globals;
open ReOrga;
open State;

let fetchDocument = (~id, ~user, ~dispatch, ~workspaceIndex) =>
  user
  |> Option.tap(user => {
       OrgDocumentsAction(State__OrgDocuments.Store.FetchProgress(id))
       |> dispatch;

       API__OrgDocument.Request.make(~workspaceIndex, ~file=id, ~user)
       |> IO.tap(data => {
            let id = API__Routes.Routes.fileUrl(~id, ~workspaceIndex);
            // Cache File
            Localforage.Localforage_IO.set(id, data)
            |> IO.unsafeRunAsync(ignore);

            // Save last seen file in settings
            SettingsAction(State__Settings.SaveLastViewdFile(id)) |> dispatch;
          })
       |> IO.unsafeRunAsync(
            fun
            | Ok(data) =>
              OrgDocumentsAction(
                State__OrgDocuments.Store.FetchSuccess(
                  id,
                  workspaceIndex,
                  data,
                ),
              )
              |> dispatch

            | Error(data) =>
              OrgDocumentsAction(
                State__OrgDocuments.Store.FetchFailure(id, data),
              )
              |> dispatch,
          )
       |> ignore;
     })
  |> ignore;

[@react.component]
let make =
    (
      ~id: string,
      ~narrowToHeader=None,
      ~layoutType=Types__Layouts.Layout.default,
      ~workspaceIndex: int,
      ~showToolbar=true,
    ) => {
  let dispatch = Store.useDispatch();
  let user = Store.useSelector(Selector.User.loggedInUser);

  let files = Store.useSelector(Selector.OrgDocuments.files);
  let file = StringMap.get(id, files);

  ReludeReact.Effect.useEffect1WithEq(
    () => fetchDocument(~id, ~user, ~workspaceIndex, ~dispatch),
    String.eq,
    id,
  );

  file
  |> Option.map(x =>
       State__OrgDocuments.(
         switch ((x: File.t)) {
         | File.Fetched({ast})
         | File.Cached({ast}) =>
           <OrgDocument__Root ast narrowToHeader layoutType showToolbar />
         | File.InProgress => "Loading" |> s
         | _ => React.null
         }
       )
     )
  |> Option.getOrElseLazy(() => React.null);
};
