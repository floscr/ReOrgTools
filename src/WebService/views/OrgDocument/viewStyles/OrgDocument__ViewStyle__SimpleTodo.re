open Relude.Globals;
open ReOrga;
open OrgDocument__Utils;

module Styles = {
  open Css;

  let headline =
    style([
      width(pct(100.)),
      marginBottom(FixedTheme.Spacing.medium),
      padding(FixedTheme.Spacing.medium),
      border(px(1), `solid, var(ThemeKeys.grey10)),
      borderRadius(FixedTheme.BorderRadius.small),
      display(`flex),
      alignItems(center),
      justifyContent(flexStart),
      cursor(`pointer),
      backgroundColor(var(ThemeKeys.bgColor)),
      textAlign(left),
    ]);
  let checkbox = style([marginRight(FixedTheme.Spacing.small)]);
};

let rec renderHeadline =
        (
          ~position,
          ~level,
          ~properties,
          ~showTodo=true,
          xs: array(ReOrga.sectionAst),
        ) => {
  module P = OrgDocument__Component__Headline;

  let {content}: P.headlineProps = xs |> P.makeHeadlineProps;
  let id = P.makeHeadlineKey(position);
  let atid = properties |> Relude.Option.flatMap(x => Js.Dict.get(x, "id"));

  <button className=Styles.headline key=id id>
    {content
     |> Array.mapWithIndex((x, i) => {
          OrgDocument__Component__Text.(
            switch (getItem(x)) {
            | Todo({keyword}) when showTodo =>
              <input type_="checkbox" className=Styles.checkbox />
            | PlainText(_) => renderPlainText(x)
            | Link(x) => renderLink(~attachmentId=atid, x)
            | _ => React.null
            }
          )
          |> wrapWithKey(x.level, i)
        })
     |> React.array}
  </button>;
};

let rec renderItems = (~level=0, ~properties=?, xs) => {
  xs
  |> Array.mapWithIndex((x, i) => {
       switch (x |> getItem) {
       | Headline({children, level, position}) =>
         renderHeadline(~position, ~level, ~properties, children)

       | Section({children, level, properties}) =>
         renderItems(~level, ~properties, children) |> wrapWithKey(level, i)

       | _ => React.null
       }
     })
  |> React.array;
};

[@react.component]
let make = (~ast: ReOrga.orgAst) => {
  renderItems(ast.children);
};
