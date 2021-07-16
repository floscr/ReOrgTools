open API__OrgDocument__Types;
open ReactUtils;
open Relude.Globals;
open ReOrga;
open State;

let fetchDocument = (~id, ~user, ~dispatch, ~workspaceIndex) =>
  user
  |> Option.tap(_ =>
       OrgDocumentsAction(State__OrgDocuments.Store.FetchProgress(id))
       |> dispatch
     )
  |> Option.tap(user => {
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
       |> ignore
     })
  |> ignore;

type t =
  | Initial
  | Loading
  | AllFetched(array(ReOrga.sectionAst));

[@react.component]
let make =
    (
      ~identifiers: array(State__OrgDocuments.File.identifier),
      ~layoutType=Types__Org.Layout.default,
      ~timerange: option(State__Settings.Agenda.Time.t)=?,
      ~narrowToHeader=None,
    ) => {
  let dispatch = Store.useDispatch();
  let user = Store.useSelector(Selector.User.loggedInUser);

  let filesStore = Store.useSelector(Selector.OrgDocuments.files);
  let files =
    StringMap.filter(
      (key, _value) =>
        identifiers
        |> Array.find(({id}: State__OrgDocuments.File.identifier) =>
             String.eq(id, key)
           )
        |> Option.isSome,
      filesStore,
    );

  ReludeReact.Effect.useEffect1WithEq(
    () =>
      identifiers
      |> Array.forEach(
           ({id, workspace}: State__OrgDocuments.File.identifier) =>
           fetchDocument(~id, ~user, ~workspaceIndex=workspace, ~dispatch)
         ),
    Array.eqBy(
      (
        a: State__OrgDocuments.File.identifier,
        b: State__OrgDocuments.File.identifier,
      ) =>
      String.eq(a.id, b.id) && Int.eq(a.workspace, b.workspace)
    ),
    identifiers,
  );

  State__OrgDocuments.(
    files
    |> Option.some
    |> Option.map(StringMap.valueArray)
    |> Option.map(
         Array.foldLeft(
           (acc, cur) => {
             switch (acc, cur) {
             // Wait for other files to be finished
             | (Loading, _) => Loading
             | (_, File.InProgress) => Loading
             // File Fetched
             | (AllFetched(xs), File.Fetched({ast}))
             | (AllFetched(xs), File.Cached({ast})) =>
               let {children}: ReOrga.orgAst = ast;

               let ys =
                 children
                 // Narrow to header when given
                 |> (
                   xs =>
                     narrowToHeader
                     |> Option.flatMap(text =>
                          Org.narrowToHeadlineWithText(~text, xs)
                        )
                     |> Option.map((x: ReOrga.sectionAst) => [|x.parent|])
                     |> Option.getOrElse(xs)
                 );

               AllFetched(Array.concat(xs, ys));
             }
           },
           AllFetched([||]),
         ),
       )
    |> Option.flatMap(
         fun
         | Loading => Some("Loading" |> s)
         | AllFetched(xs) =>
           (
             switch (layoutType) {
             | Kanban => <OrgDocument__ViewStyle__Kanban xs />
             | SimpleTodo => <OrgDocument__ViewStyle__SimpleTodo xs />
             | _ => <OrgDocument__Root xs />
             }
           )
           |> Option.some

         | _ => None,
       )
    |> Option.getOrElseLazy(() => React.null)
  );
};
