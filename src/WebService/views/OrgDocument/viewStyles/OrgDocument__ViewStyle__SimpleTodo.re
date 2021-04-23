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
      textAlign(`left),
    ]);
  let checkbox = style([marginRight(FixedTheme.Spacing.small)]);
};

let renderHeadline =
    (~position, ~properties, ~showTodo=true, xs: array(ReOrga.sectionAst)) => {
  module P = OrgDocument__Component__Headline;

  let {content}: P.headlineProps = xs |> P.makeHeadlineProps;
  let id = P.makeHeadlineKey(position);
  let atid = properties |> Relude.Option.flatMap(x => Js.Dict.get(x, "id"));

  <button className=Styles.headline key=id id>
    {content
     |> Array.mapWithIndex((x, i) => {
          (
            switch (getItem(x)) {
            | Todo({keyword}) when showTodo =>
              <input
                type_="checkbox"
                defaultChecked={keyword == "DONE"}
                className=Styles.checkbox
              />
            | PlainText(_) => OrgDocument__Component__Text.renderPlainText(x)
            | Link(x) =>
              OrgDocument__Component__Text.renderLink(~attachmentId=atid, x)
            | _ => React.null
            }
          )
          |> wrapWithKey(x.level, i)
        })
     |> React.array}
  </button>;
};

let rec renderItems = (~properties=?, xs) => {
  xs
  |> Array.mapWithIndex((x, i) => {
       switch (x |> getItem) {
       | Headline({children, position, keyword}) when keyword |> Option.isSome =>
         renderHeadline(~position, ~properties, children)

       | Section({children, level, properties}) =>
         renderItems(~properties, children) |> wrapWithKey(level, i)

       | _ => React.null
       }
     })
  |> React.array;
};

[@react.component]
let make = (~ast: ReOrga.orgAst) => {
  renderItems(ast.children);
};
