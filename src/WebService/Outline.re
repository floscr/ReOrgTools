open FilesTypes;
open ReactUtils;
open Relude.Globals;
open ReactUtils;
open ReOrga;

module Styles = {
  open Css;

  let headline = (level: int) =>
    style([
      paddingLeft(px(5 * level)),
      margin(zero),
      marginTop(level == 1 ? rem(0.5) : zero),
    ]);
};

let renderHeadline = (~position, ~level, ~index, xs) => {
  let key = Page.makeHeadlineKey(position);

  let onClick = _ =>
    Webapi.Dom.document
    |> Webapi.Dom.Document.getElementById(key)
    |> Option.tap(Webapi.Dom.Element.scrollIntoView)
    |> Option.tap(_ => ReasonReactRouter.push({j|#$key|j}))
    |> ignore;

  let text =
    Array.foldLeft(
      ((i, str) as acc, cur) => {
        (
          switch (getItem(cur)) {
          | PlainText({value}) => Some(value)
          | Link({value}) => Some(value)
          | _ => None
          }
        )
        |> Option.map(x => (i + 1, str ++ x))
        |> Option.getOrElse(acc)
      },
      (0, ""),
      xs,
    )
    |> (((a, b)) => b);

  <p key className={Styles.headline(level)} onClick> {text |> s} </p>;
};

let rec renderItems = (~level=0, xs) => {
  Belt.Array.mapWithIndex(xs, (i, x) => {
    switch (getItem(x)) {
    | Headline({children, level, position}) =>
      renderHeadline(~position, ~level, ~index=i, children)
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

  <> {ast |> (x => x.children |> (xs => renderItems(xs)))} </>;
};
