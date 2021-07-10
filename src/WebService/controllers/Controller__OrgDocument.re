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

type t =
  | Initial
  | Loading
  | AllFetched(array(ReOrga.sectionAst));

[@react.component]
let make =
    (
      ~identifiers: array(State__OrgDocuments.File.identifier),
      ~layoutType=Types__Org.Layout.default,
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
               let {children, properties} = ast;
               Js.log(ast);
               let ys =
                 narrowToHeader
                 |> Option.flatMap(text =>
                      Org.narrowToHeadlineWithText(~text, children)
                    )
                 |> Option.map((x: ReOrga.sectionAst) => [|x.parent|])
                 |> Option.getOrElse(children);

               AllFetched(Array.concat(xs, ys));
             /* let layoutType = */
             /*   Js.Dict.get(properties, "reorg_view") */
             /*   |> Option.map(Types__Org.Layout.fromString) */
             /*   |> Option.getOrElse(layoutType); */
             }
           },
           AllFetched([||]),
         ),
       )
    |> Option.flatMap(
         fun
         | Loading => Some("Loading" |> s)
         | AllFetched(xs) => Some(<OrgDocument__Root xs layoutType />)

         | _ => None,
       )
    |> Option.getOrElseLazy(() => React.null)
  );
};
