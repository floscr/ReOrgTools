open Relude.Globals;
open ReOrga;
open OrgDocument__Utils;
open ReactUtils;

module Styles = {
  open Css;
  open FixedTheme;

  let root =
    style([
      display(`flex),
      flexDirection(column),
      position(relative),
      flexGrow(1.),
      flexShrink(1.),
      selector("h1", [fontSize(rem(1.4)), paddingBottom(Spacing.large)]),
      selector("h2", [fontSize(rem(1.3))]),
      selector("p, li", [lineHeight(`abs(1.55))]),
    ]);

  let section =
    style([
      selector(
        "& + &",
        [marginTop(Spacing.large), paddingTop(Spacing.large)],
      ),
      /* selector("> * + *", [marginTop(Spacing.large)]), */
    ]);

  let rootTodoSection =
    style([
      selector("h1", [paddingBottom(zero)]),
      selector(
        "* + &:first-of-type",
        [
          borderTop(px(1), solid, var(ThemeKeys.grey10)),
          marginTop(Spacing.xlarge),
          paddingTop(Spacing.xlarge),
        ],
      ),
      selector(
        "& + &",
        [
          borderTop(px(1), solid, var(ThemeKeys.grey10)),
          marginTop(Spacing.xlarge),
          paddingTop(Spacing.xlarge),
        ],
      ),
    ]);

  let todoSection =
    style([
      border(px(1), `solid, var(ThemeKeys.grey15)),
      padding(Spacing.medium),
      selector(
        "&",
        [
          borderTopLeftRadius(BorderRadius.small),
          borderTopRightRadius(BorderRadius.small),
          borderBottomWidth(zero),
          marginTop(zero),
        ],
      ),
      selector(
        "& + &",
        [
          borderTopWidth(zero),
          borderBottomWidth(zero),
          borderRadius(zero),
          marginTop(zero),
          paddingTop(Spacing.medium),
          boxShadow(Shadow.box(~y=px(-1), var(ThemeKeys.grey00))),
        ],
      ),
      selector(
        "& + &:last-of-type",
        [
          borderBottomLeftRadius(BorderRadius.small),
          borderBottomRightRadius(BorderRadius.small),
          borderBottomWidth(px(1)),
        ],
      ),
    ]);

  let mainWrapper =
    style([
      padding2(~h=Spacing.xxlarge, ~v=Spacing.xlarge),
      flexGrow(1.),
      flexShrink(1.),
      maxWidth(px(860)),
      wordWrap(breakWord),
      width(pct(100.)),
      margin2(~v=zero, ~h=auto),
    ]);
};

let rec renderItems = (~level=0, ~properties=?, ~hasTodoParent=false, xs) => {
  xs
  |> Array.mapWithIndex((x, i) => {
       switch (x |> getItem) {
       | Headline(headline) =>
         OrgDocument__Component__Headline.renderHeadline(
           ~properties,
           headline,
         )

       | Section({children, level, properties}) =>
         let isTodo =
           children
           |> Array.head
           |> Option.flatMap(x =>
                x
                |> getItem
                |> (
                  fun
                  | Headline({children}) => Some(children)
                  | _ => None
                )
              )
           |> Option.flatMap(
                Array.find(
                  getItem
                  >> (
                    fun
                    | Todo(_) => true
                    | _ => false
                  ),
                ),
              )
           |> Option.isSome;

         let className =
           ClassName.pure(Styles.section)
           |> ClassName.condAppend(
                isTodo && !hasTodoParent,
                Styles.rootTodoSection,
              )
           |> ClassName.condAppend(
                hasTodoParent && isTodo,
                Styles.todoSection,
              )
           |> ClassName.unwrap;

         <section className key={makeKey(level, i)}>
           {renderItems(
              ~level,
              ~properties,
              ~hasTodoParent=hasTodoParent || isTodo,
              children,
            )}
         </section>;

       | Paragraph({children}) =>
         OrgDocument__Component__Text.renderParagraphs(~properties, children)
         |> (x => <p key={makeKey(level, i)}> x </p>)

       | Block(_) as x =>
         OrgDocument__Component__Block.renderBlock(x)
         |> wrapWithKey(level, i)

       | List({children, ordered}) =>
         OrgDocument__Component__List.renderList(children, ordered)
         |> wrapWithKey(level, i)

       | Table({children}) =>
         OrgDocument__Component__Table.renderTable(children)
         |> wrapWithKey(level, i)

       | Hr(_) =>
         OrgDocument__Component__Etc.renderHr() |> wrapWithKey(level, i)

       | _ => React.null
       }
     })
  |> React.array;
};

[@react.component]
let make = (~ast: ReOrga.orgAst, ~queryParams: Types__URLSearchParams.t) => {
  let {children, properties} = ast;
  let {narrowToHeader, layoutType}: Types__URLSearchParams.t = queryParams;

  Js.log(ast);

  let xs =
    narrowToHeader
    |> Option.flatMap(text => Org.narrowToHeadlineWithText(~text, children))
    |> Option.map(x => [|x.parent|])
    |> Option.getOrElse(children);

  Js.Dict.get(properties, "reorg_view")
  |> Option.map(Types__URLSearchParams.Layouts.fromString)
  |> Option.getOrElse(layoutType)
  |> Types__URLSearchParams.(
       fun
       | Kanban => {
           <OrgDocument__ViewStyle__Kanban xs />;
         }
       | SimpleTodo =>
         <div className=Styles.mainWrapper>
           <OrgDocument__ViewStyle__SimpleTodo ast />
         </div>
       | _ => <div className=Styles.mainWrapper> {renderItems(xs)} </div>
     )
  |> (
    xs =>
      <div className=Styles.root>
        <OrgDocument__Toolbar ast layoutType />
        xs
      </div>
  );
};
