open Relude.Globals;
open OrgDocument__Utils;
open ReOrga;
open ReactUtils;

module Styles = {
  open Css;
  open FixedTheme;

  let header =
    style([
      display(`flex),
      justifyContent(spaceBetween),
      alignItems(center),
      /* margin3(~h=zero, ~top=rem(1.38), ~bottom=rem(1.38)), */
      selector(
        "a",
        [
          textDecoration(none),
          after([
            unsafe("content", {j|" link"|j}),
            fontFamily(`custom("'Material Icons'")),
            color(var(ThemeKeys.grey20)),
          ]),
          hover([textDecoration(underline)]),
        ],
      ),
      /* selector("a:hover, a:focus", [textDecoration(underline)]), */
    ]);

  let footer =
    style([
      display(`flex),
      flexDirection(column),
      alignItems(flexEnd),
      justifyContent(spaceBetween),
    ]);

  let tags = style([alignSelf(flexEnd), marginLeft(auto)]);

  let headlineTodo = (x: string) =>
    style([
      (
        switch (x) {
        | "DONE" => "fe9898"
        | _ => "bbfe98"
        }
      )
      |> hex
      |> backgroundColor,
      borderRadius(px(5)),
      padding2(~v=px(2), ~h=px(5)),
    ]);
};

let makeHeadlineKey = position =>
  "headline"
  ++ "-"
  ++ (position.start.line |> string_of_int)
  ++ "-"
  ++ (position.start.column |> string_of_int)
  ++ "-"
  ++ (position.end_.line |> string_of_int)
  ++ "-"
  ++ (position.end_.column |> string_of_int);

module Heading = {
  [@react.component]
  let make = (~level: int, ~children) => {
    switch (level) {
    | 1 => <h1> children </h1>
    | 2 => <h2> children </h2>
    | 3 => <h3> children </h3>
    | 4 => <h4> children </h4>
    | 5 => <h5> children </h5>
    | _ => children
    };
  };
};

type headlineProps = {
  stars: option(stars),
  content: array(sectionAst),
  tags: option(tags),
};

let makeHeadlineProps =
  Array.foldLeft(
    (acc, cur) =>
      switch (getItem(cur)) {
      | Stars(x) when Config.showStars => {...acc, stars: Some(x)}
      | Tags(x) => {...acc, tags: Some(x)}
      | _ => {...acc, content: Array.concat(acc.content, [|cur|])}
      },
    {stars: None, tags: None, content: [||]},
  );

let renderHeadline = (~properties, headline: ReOrga.headline) => {
  let {children, content, level, position, keyword, actionable} = headline;
  let {content, tags} = children |> makeHeadlineProps;

  let atid = properties |> Relude.Option.flatMap(x => Js.Dict.get(x, "id"));
  let id = makeHeadlineKey(position);

  let hasFooter = keyword |> Option.isSome || tags |> Option.isSome;

  <header className=Styles.header key=id id>
    {content
     |> Array.mapWithIndex((x, i) => {
          (
            switch (x |> getItem) {
            | PlainText(_) => OrgDocument__Component__Text.renderPlainText(x)
            | Link(x) =>
              OrgDocument__Component__Text.renderLink(~attachmentId=atid, x)
            | _ => React.null
            }
          )
          |> wrapWithKey(x.level, i)
        })
     |> React.array
     |> (xs => <Heading level> xs </Heading>)}
    {hasFooter
       ? {
         <footer className=Styles.footer>
           {switch (keyword) {
            | Some(keyword) =>
              <span className={Styles.headlineTodo(keyword)}>
                {keyword |> s}
              </span>
            | _ => React.null
            }}
           {switch (tags) {
            | Some({tags}) =>
              <div className=Styles.tags>
                {OrgDocument__Component__Tags.renderTags(tags)}
              </div>
            | _ => React.null
            }}
         </footer>;
       }
       : React.null}
  </header>;
};
