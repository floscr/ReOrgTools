open FilesTypes;
open ReactUtils;
open Relude.Globals;
open ReactUtils;
open ReOrga;

let rec renderItems = (~level=0, xs) => {
  Belt.Array.mapWithIndex(xs, (i, x) => {
    switch (getItem(x)) {
    | Headline({children, level, position}) =>
      Page.renderHeadline(~position, ~level, ~index=i, children)
    | Section({children, level}) =>
      renderItems(~level, children) |> Page.wrapWithKey(level, i)
    | _ => React.null
    }
  })
  |> React.array;
};

[@react.component]
let make = (~ast: ReOrga.orgAst) => {
  let items = ast.children;
  <div> {ast |> (x => x.children |> (xs => renderItems(xs)))} </div>;
};
