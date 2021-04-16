open Relude.Globals;
open ReOrga;
open ReactUtils;
open OrgDocument__Utils;

module Styles = {
  open Css;
  open FixedTheme;

  let root =
    style([
      /* position(absolute), */
      /* top(zero), */
      /* left(zero), */
      /* right(zero), */
      /* bottom(zero), */
      overflowX(auto),
      overflowY(hidden),
      display(`flex),
      flexGrow(1.),
      flexShrink(1.),
      backgroundColor(var(ThemeKeys.grey20)),
    ]);

  let column =
    style([
      overflow(hidden),
      flexGrow(1.),
      width(vw(80.)),
      maxWidth(px(300)),
      display(`flex),
      flexDirection(column),
      backgroundColor(var(ThemeKeys.grey00)),
      selector(
        "> * + *",
        [borderRight(px(1), `solid, var(ThemeKeys.grey20))],
      ),
    ]);

  let columnContent =
    style([
      padding(Spacing.medium),
      flexGrow(1.),
      overflowY(auto),
      backgroundColor(var(ThemeKeys.grey00)),
    ]);

  let columnHeader =
    style([
      padding2(~h=Spacing.medium, ~v=Spacing.large),
      borderBottom(px(1), `solid, var(ThemeKeys.grey10)),
      borderRight(px(1), `solid, var(ThemeKeys.grey10)),
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

  let todos = groupByTodo(acc, xs);

  let tags = () =>
    todos
    |> StringMap.valueArray
    |> Array.flatten
    |> Array.foldLeft(
         (acc, cur) => {
           cur
           |> getItem
           |> (
             fun
             | Headline({tags}) => Array.concat(acc, tags)
             | _ => acc
           )
         },
         [||],
       )
    |> Array.distinctBy(String.eq)
    |> Utils.log;

  <div className=Styles.root>
    {todos
     |> StringMap.toArray
     |> Array.reject(((_, tail)) => Array.isEmpty(tail))
     |> Array.map(((head, tail)) => {
          <div key={"kanban-column-" ++ head} className=Styles.column>
            <header className=Styles.columnHeader> {head |> s} </header>
            <section className=Styles.columnContent>
              {tail
               |> Array.mapWithIndex((cur: ReOrga.sectionAst, i) =>
                    switch (cur |> getItem) {
                    | Headline({content, level, position, children}) =>
                      OrgDocument__ViewStyle__SimpleTodo.renderHeadline(
                        ~position,
                        ~level,
                        ~properties=None,
                        ~showTodo=false,
                        children,
                      )
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
