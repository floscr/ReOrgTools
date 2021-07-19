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

type orgRangeT =
  | Single(Js.Date.t)
  | FromTo(Js.Date.t, Js.Date.t);

let isInTimeRange =
    (
      ~timerange: State__Settings.Agenda.Time.timerangeT,
      ~start: option(Js.Date.t),
      ~end_: option(Js.Date.t),
    ) => {
  let orgRange =
    switch (start, end_) {
    | (Some(start), Some(end_)) => Ok(FromTo(start, end_))
    | (Some(x), _)
    | (_, Some(x)) => Ok(Single(x))
    | _ => Error("Impossible Range") // Should not be possible
    };

  State__Settings.Agenda.Time.(
    orgRange
    |> Result.map(orgRange =>
         switch (timerange, orgRange) {
         | (CurrentOnly(current), Single(date)) =>
           isWithinSingle(current, date)
         | _ => false
         }
       )
  )
  |> Result.getOrElse(false);
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
       | Headline({children, position, keyword}) when keyword |> Option.isSome =>
         renderHeadline(~position, ~properties, children)

       | Section({children, level, properties}) =>
         timerange
         |> Option.flatMap(Result.toOption)
         |> Option.reject(timerange =>
              children
              |> Array.find(x =>
                   switch (getItem(x) |> Utils.log) {
                   | Planning({start, end_}) =>
                     isInTimeRange(~timerange, ~start, ~end_)
                   | _ => false
                   }
                 )
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
