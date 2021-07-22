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
  |> Result.map(ReDate.areIntervalsOverlapping(timeRangeInterval))
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
                   switch (getItem(x)) {
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
  let conds =
    [
      (true, ({keyword}: ReOrga.headline) => keyword |> Option.isSome),
      (
        timerange |> Option.flatMap(Result.toOption) |> Option.isSome,
        ({parent}: ReOrga.headline) =>
          parent
          |> getItem
          |> (
            fun
            | Section({children}) =>
              timerange
              |> Option.flatMap(Result.toOption)
              |> Option.reject(timerange =>
                   children
                   |> Array.find(x =>
                        switch (getItem(x)) {
                        | Planning({start, end_}) =>
                          isInTimeRange(~timerange, ~start, ~end_)
                        | _ => false
                        }
                      )
                   |> Option.isNone
                 )
              |> Option.isSome
            | _ => false
          ),
      ),
    ]
    |> List.foldLeft(
         (acc, (keep, fn)) => keep ? List.append(fn, acc) : acc,
         [],
       );

  Js.log(unfoldTree(~cond=keepItem(~conds), xs));

  renderItems(~timerange?, xs) |> Wrappers.paddedWrapper;
};
