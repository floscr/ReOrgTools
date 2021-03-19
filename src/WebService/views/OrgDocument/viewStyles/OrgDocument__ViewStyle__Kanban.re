open Relude.Globals;
open ReOrga;
open ReactUtils;
open OrgDocument__Utils;

module Styles = {
  open Css;
  open FixedTheme;

  let root =
    style([
      position(absolute),
      top(zero),
      left(zero),
      right(zero),
      bottom(zero),
      overflowX(scroll),
      overflowY(hidden),
      display(`flex),
      selector(
        "> * + *",
        [borderRight(px(1), `solid, var(ThemeKeys.grey10))],
      ),
    ]);

  let column =
    style([
      overflow(hidden),
      flexGrow(1.),
      minWidth(px(300)),
      display(`flex),
      flexDirection(column),
      backgroundColor(var(ThemeKeys.grey00)),
    ]);

  let columnContent =
    style([padding(Spacing.medium), flexGrow(1.), overflowY(auto)]);

  let columnHeader =
    style([
      padding2(~h=Spacing.medium, ~v=Spacing.large),
      borderBottom(px(1), `solid, var(ThemeKeys.grey10)),
      backgroundColor(var(ThemeKeys.bgColor)),
    ]);

  let item =
    style([
      width(pct(100.)),
      marginBottom(FixedTheme.Spacing.medium),
      padding(FixedTheme.Spacing.medium),
      border(px(1), `solid, var(ThemeKeys.grey10)),
      borderRadius(FixedTheme.BorderRadius.small),
      display(`flex),
      alignItems(center),
      cursor(`pointer),
      backgroundColor(var(ThemeKeys.bgColor)),
    ]);
};

type mapT = StringMap.t(array(ReOrga.sectionAst));

let rec groupByTodo = (map: mapT, xs) =>
  Array.foldLeft(
    (acc, cur) => {
      switch (getItem(cur)) {
      | Headline({keyword}) when Option.isSome(keyword) =>
        acc
        |> StringMap.update(
             keyword |> Option.getOrElse("TODO"),
             Option.foldLazy(
               _ => Some([|cur|]),
               xs => Some(Array.append(cur, xs)),
             ),
           )

      | Section({children}) => groupByTodo(acc, children)
      | _ => acc
      }
    },
    map,
    xs,
  );

[@react.component]
let make = (~xs: array(ReOrga.sectionAst)) => {
  let acc: mapT =
    Config.todoKeyWords |> Array.map(x => (x, [||])) |> StringMap.fromArray;

  <div className=Styles.root>
    {groupByTodo(acc, xs)
     |> StringMap.toArray
     |> Array.map(((head, tail)) => {
          <div className=Styles.column>
            <header className=Styles.columnHeader> {head |> s} </header>
            <section className=Styles.columnContent>
              {tail
               |> Array.map((cur: ReOrga.sectionAst) =>
                    switch (cur |> getItem) {
                    | Headline({content}) =>
                      <div className=Styles.item> {content |> s} </div>
                    | _ => React.null
                    }
                  )
               |> React.array}
            </section>
          </div>
        })
     |> React.array}
  </div>;
};
