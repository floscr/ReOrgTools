open Relude.Globals;
open ReOrga;
open OrgDocument__Utils;

module Styles = {
  open Css;

  let mainWrapper =
    style([
      margin2(~v=zero, ~h=auto),
      maxWidth(px(750)),
      wordWrap(breakWord),
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
         OrgDocument__Block.renderBlock(x) |> wrapWithKey(level, i)

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
let make = (~ast: ReOrga.orgAst, ~header: option(string)) => {
  let {children, properties} = ast;

  Js.log(ast);

  Js.Dict.get(properties, "reorg_view")
  |> (
    fun
    | Some(x) when x === "SIMPLE_TODO" =>
      <div className=Styles.mainWrapper>
        <OrgDocument__ViewStyle__SimpleTodo ast />
      </div>
    | _ => <div className=Styles.mainWrapper> {renderItems(children)} </div>
  );
};
