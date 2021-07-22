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
            color(var(ThemeKeys.baseGray6)),
          ]),
          hover([textDecoration(underline)]),
        ],
      ),
      /* selector("a:hover, a:focus", [textDecoration(underline)]), */
    ]);

  let timeStamp =
    style([
      display(`flex),
      fontWeight(medium),
      color(var(ThemeKeys.baseGray11)),
      fontSize(rem(0.8)),
      marginTop(Spacing.xxsmall),
    ]);
  let timeStampIcon =
    style([
      selector(
        ".material-icons",
        [
          lineHeight(`abs(1.1)),
          fontSize(px(14)),
          marginRight(em(0.25)),
        ],
      ),
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
        | "DONE" => hex("bbfe98")
        | _ => var(ThemeKeys.green1)
        }
      )
      |> backgroundColor,
      color(var(ThemeKeys.green3)),
      fontWeight(medium),
      fontSize(rem(0.8)),
      borderRadius(FixedTheme.BorderRadius.small),
      lineHeight(`abs(1.)),
      padding2(~v=px(4), ~h=px(5)),
    ]);
};

let makeHeadlineKey = (position: ReOrga.positionAst) =>
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
  let {children, level, position, keyword, parent} = headline;
  let {content, tags} = children |> makeHeadlineProps;

  let planning =
    parent.children
    |> Array.find(
         getItem
         >> (
           fun
           | Planning(_) => true
           | _ => false
         ),
       )
    |> Option.flatMap(
         getItem
         >> (
           fun
           | Planning({type_, start}) =>
             Some(
               <span className=Styles.timeStamp>
                 {type_
                  |> (
                    fun
                    | ReOrga.PlanningType.Scheduled => "schedule"
                    | ReOrga.PlanningType.Deadline => "warning_amber"
                  )
                  |> (id => <IconButton id style=Styles.timeStampIcon />)}
                 {start
                  |> Option.map(DateTime.fromJSDate)
                  |> Option.map(x => {
                       let date = x |> DateTime.toFormat("ccc, dd.MM.yyyy");
                       // TODO: This is actually wrong as the scheduled time could be 00:00:00
                       // But right now we don't have access to check if there is a timestamp supplied or not
                       let hasTime =
                         x##hour
                         +
                         x##minute
                         +
                         x##second
                         +
                         x##millisecond
                         |> Int.notEq(0);

                       let hour =
                         switch (hasTime) {
                         | true => x |> DateTime.toFormat(" HH:mm")
                         | _ => ""
                         };

                       date ++ hour;
                     })
                  |> Option.fold(React.null, s)}
               </span>,
             )
           | _ => None
         ),
       );

  let atid = properties |> Relude.Option.flatMap(x => Js.Dict.get(x, "id"));
  let id = makeHeadlineKey(position);
  let urlProperty =
    properties |> Relude.Option.flatMap(x => Js.Dict.get(x, "url"));
  let hasFooter = keyword |> Option.isSome || tags |> Option.isSome;

  <header className=Styles.header key=id id>
    <div>
      {content
       |> Array.mapWithIndex((x, i) => {
            OrgDocument__Component__Text.(
              switch (x |> getItem, urlProperty) {
              | (PlainText({value}), Some(urlProperty)) =>
                renderUrlLinkSimple(~description=value, urlProperty)
              | (PlainText(_), _) => renderPlainText(x)
              | (Link(x), _) => renderLink(~attachmentId=atid, x)
              | _ => React.null
              }
            )
            |> wrapWithKey(x.level, i)
          })
       |> React.array
       |> (xs => <Heading level> xs </Heading>)}
      {planning |> Option.fold(React.null, x => x)}
    </div>
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
