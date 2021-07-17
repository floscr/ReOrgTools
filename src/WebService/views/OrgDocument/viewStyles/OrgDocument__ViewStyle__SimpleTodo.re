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
      border(px(1), `solid, var(ThemeKeys.baseGray3)),
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

let rec renderItems =
        (
          ~properties=?,
          ~timerange: option(State__Settings.Agenda.Time.t)=?,
          xs,
        ) => {
  xs
  |> Array.mapWithIndex((x, i) => {
       switch (x |> getItem) {
       | Headline({children, position, keyword} as x)
           when keyword |> Option.isSome =>
         /* Js.log(x); */
         renderHeadline(~position, ~properties, children)

       | Section({children, level, properties}) =>
         timerange
         |> Option.map(Result.toOption)
         |> Option.flatten
         |> Option.reject(_ =>
              children
              |> Array.find(x =>
                   switch (getItem(x)) {
                   | Planning({start, end_}) => true
                   | _ => false
                   }
                 )
              |> Utils.log
              |> Option.isSome
            )
         |> Option.foldLazy(
              _ =>
                renderItems(~timerange?, ~properties, children)
                |> wrapWithKey(level, i),
              _ => React.null,
            )

       | _ => React.null
       }
     })
  |> React.array;
};

[@react.component]
let make =
    (
      ~xs: array(ReOrga.sectionAst),
      ~timerange: option(State__Settings.Agenda.Time.t)=?,
    ) => {
  renderItems(~timerange?, xs) |> Wrappers.paddedWrapper;
};
