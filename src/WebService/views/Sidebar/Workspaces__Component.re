open ReactUtils;
open Relude.Globals;

module Styles = {
  open Css;

  let workspaceList =
    style([listStyleType(none), margin(zero), padding(zero)]);
  let workspaceListItem = style([margin(zero)]);

  let button =
    style([
      padding2(~v=Theme.Spacing.xsmall, ~h=Theme.Spacing.large),
      lineHeight(`abs(1.)),
      borderRadius(px(50)),
      cursor(pointer),
      width(pct(100.)),
      fontWeight(medium),
      textAlign(`left),
      transition(~duration=Theme.Timing.hoverShort, "all"),
      selector(":hover", [backgroundColor(Theme.BaseTheme.grey2)]),
      selector(
        ":focus",
        [
          outlineStyle(none),
          borderRadius(px(50)),
          boxShadow(
            Shadow.box(~spread=px(3), ~inset=true, rgba(0, 0, 0, pct(7.))),
          ),
        ],
      ),
    ]);
};

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
       <button key=name className=Styles.button onClick={_ => onClick(base)}>
         {base |> s}
       </button>;
     })
  |> React.array;

[@react.component]
let make = (~workspaces, ~workspaceIndex, ~onFileClick) => {
  let onClick = file => {
    let index = Int.toString(workspaceIndex);
    ReasonReactRouter.push({j|/file/$index/$file|j});
    onFileClick();
  };
  switch (workspaces) {
  | [] => "No files found" |> s
  | xs =>
    xs
    |> List.mapWithIndex(((workspace, files), index) =>
         <li className=Styles.workspaceListItem key=workspace>
           {workspace |> s}
           {renderFiles(~onClick, ~index, files)}
         </li>
       )
    |> List.toArray
    |> React.array
    |> (xs => <ul className=Styles.workspaceList> xs </ul>)
  };
};
