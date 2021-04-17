open Relude.Globals;
open ReOrga;
open OrgDocument__Utils;

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
    ]);

  let mainWrapper =
    style([
      padding2(~h=Spacing.xxlarge, ~v=Spacing.xlarge),
      flexGrow(1.),
      flexShrink(1.),
      maxWidth(px(750)),
      wordWrap(breakWord),
      width(pct(100.)),
      margin2(~v=zero, ~h=auto),
    ]);
};

let rec renderItems = (~level=0, ~properties=?, xs) => {
  xs
  |> Array.mapWithIndex((x, i) => {
       switch (x |> getItem) {
       | Headline({children, level, position}) =>
         OrgDocument__Component__Headline.renderHeadline(
           ~position,
           ~level,
           ~properties,
           children,
         )

       | Section({children, level, properties}) =>
         renderItems(~level, ~properties, children) |> wrapWithKey(level, i)

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
        <OrgDocument__Toolbar ast queryParams layoutType />
        xs
      </div>
  );
};
