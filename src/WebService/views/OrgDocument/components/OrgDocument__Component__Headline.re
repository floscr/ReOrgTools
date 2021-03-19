open Relude.Globals;
open OrgDocument__Utils;
open ReOrga;
open ReactUtils;

module Styles = {
  open Css;

  let headline =
    style([
      display(block),
      margin3(~h=zero, ~top=rem(1.38), ~bottom=rem(1.38)),
    ]);

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
      marginRight(rem(0.5)),
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

let renderHeadline = (~position, ~level, ~properties, xs) => {
  let {stars, content, tags} = xs |> makeHeadlineProps;

  let atid = properties |> Relude.Option.flatMap(x => Js.Dict.get(x, "id"));
  let id = makeHeadlineKey(position);

  <header className=Styles.headline key=id id>
    {switch (stars) {
     | Some({level}) =>
       <span>
         {Belt.Array.makeBy(level, _ => "*") |> Js.Array.joinWith("") |> s}
       </span>
     | _ => React.null
     }}
    {content
     |> Array.mapWithIndex((x, i) => {
          (
            switch (getItem(x)) {
            | Todo({keyword}) =>
              <span className={Styles.headlineTodo(keyword)}>
                {keyword |> s}
              </span>
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
    {switch (tags) {
     | Some({tags}) => OrgDocument__Component__Tags.renderTags(tags)
     | _ => React.null
     }}
  </header>;
};
