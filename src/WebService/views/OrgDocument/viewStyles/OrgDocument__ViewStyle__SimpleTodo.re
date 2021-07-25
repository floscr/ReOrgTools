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
  let orgRange: result(ReDate.interval, string) =
    switch (start, end_) {
    | (Some(start), Some(end_)) => Ok({start, end_})
    | (Some(start), _) => Ok({start, end_: start |> ReDate.endOfDay})
    | (_, Some(end_)) => Ok({start: end_ |> ReDate.startOfDay, end_})
    | _ => Error("Impossible Range")
    };

  let timeRangeInterval =
    State__Settings.Agenda.Time.timeRangeToInterval(timerange);

  orgRange
  |> Result.map(x =>
       ReDate.areIntervalsOverlapping(timeRangeInterval, x)
       || ReDate.isBefore(timeRangeInterval.start, x.end_)
     )
  |> Result.getOrElse(false);
};

let rec unfoldTree = (~cond=_ => true, ~acc=[||], rest) => {
  rest
  |> Array.foldLeft(
       (childAcc, child) =>
         switch (child |> getItem) {
         | Headline(headline) when cond(headline) =>
           Array.append(child, childAcc)
         | Section({children}) =>
           Array.concat(childAcc, unfoldTree(~acc, ~cond, children))
         | _ => childAcc
         },
       acc,
     );
};

let keepItem = (~conds=[], headline: ReOrga.headline) => {
  conds |> List.find(cond => cond(headline) === false) |> Option.isNone;
};

let rec renderItems = (~properties=?, xs) => {
  xs
  |> Array.mapWithIndex((x, i) => {
       switch (x |> getItem) {
       | Headline({children, position, keyword}) when keyword |> Option.isSome =>
         renderHeadline(~position, ~properties, children)
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
  let dateCompare =
    Ord.by(
      (x: ReOrga.sectionAst) =>
        (
          switch (getItem(x)) {
          | Headline(x) => Some(x)
          | _ => None
          }
        )
        |> Option.flatMap(Org.Headline.getPlanning)
        |> Option.flatMap(({start}: ReOrga.planning) => start)
        |> Option.map(Js.Date.getTime)
        |> Option.getOrElse(0.),
      Float.compare,
    );

  let conds =
    [
      // Only with TODO keyword
      (true, ({keyword}: ReOrga.headline) => keyword |> Option.isSome),
      // Planning items
      (
        timerange |> Option.isSome,
        (x: ReOrga.headline) => {
          Js.log(x);
          switch (
            timerange |> Option.flatMap(Result.toOption),
            Org.Headline.getPlanning(x),
          ) {
          | (Some(timerange), Some({start, end_})) =>
            isInTimeRange(~timerange, ~start, ~end_)
          | _ => false
          };
        },
      ),
    ]
    |> List.foldLeft(
         (acc, (keep, fn)) => keep ? List.append(fn, acc) : acc,
         [],
       );

  xs
  |> unfoldTree(~cond=keepItem(~conds))
  |> Array.sortBy(dateCompare)
  |> renderItems
  |> Wrappers.paddedWrapper;
};
