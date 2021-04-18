open ReactUtils;
open Relude.Globals;
open State;
open Sidebar__Utils;

let fileCompare =
  Ord.by(
    ({name}: Shared__API__Workspaces.File.t) => name |> String.toLowerCase,
    String.compare,
  );

let renderFiles = (~onClick, ~index, xs) =>
  xs
  |> Array.reject(({name}: Shared__API__Workspaces.File.t) =>
       String.contains(~search=".sync-conflict", name)
     )
  |> Array.sortBy(fileCompare)
  |> Array.map(({name}: Shared__API__Workspaces.File.t) => {
       let base = name |> Filename.chop_extension;
       <button
         key=name className=Styles.button onClick={_ => onClick(base, index)}>
         {base |> s}
       </button>;
     })
  |> React.array;

[@react.component]
let make = (~workspaceIndex, ~onFileClick) => {
  let workspaces = Store.useSelector(Selector.Workspaces.workspaces);

  let onClick = (id, workspaceIndex) => {
    API__Routes.Routes.openFile(~workspaceIndex, ~id);
    onFileClick();
  };

  switch (workspaces) {
  | [] => "No files found" |> s
  | xs =>
    xs
    |> List.mapWithIndex(((workspace, files), index) =>
         <li className=Styles.li key=workspace>
           <header className=Styles.header>
             {workspace |> Filename.basename |> s}
           </header>
           {renderFiles(~onClick, ~index, files)}
         </li>
       )
    |> List.toArray
    |> React.array
    |> (xs => <ul className=Styles.ul> xs </ul>)
  };
};
