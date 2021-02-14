open Orga;
open ReactUtils;

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

let wrapWithKey = (level, index, children) => {
  let key = {j|$level-$index|j};
  <React.Fragment key> children </React.Fragment>;
};

let renderTags = xs =>
  <ul>
    {Belt.Array.mapWithIndex(
       xs,
       (i, x) => {
         let key = string_of_int(i);
         <li key> {s(x)} </li>;
       },
     )
     |> React.array}
  </ul>;

type headlineProps = {
  stars: option(stars),
  content: array(sectionAst),
  tags: option(tags),
};

let renderHeadline = (xs, level) => {
  let {stars, content, tags} =
    Js.Array.reduce(
      (acc, cur) =>
        switch (getItem(cur)) {
        | Stars(x) => {...acc, stars: Some(x)}
        | Tags(x) => {...acc, tags: Some(x)}
        | _ => {...acc, content: Belt.Array.concat(acc.content, [|cur|])}
        },
      {stars: None, tags: None, content: [||]},
      xs,
    );

  <header>
    {switch (stars) {
     | Some({level}) =>
       <span>
         {Belt.Array.makeBy(level, _ => "*") |> Js.Array.joinWith("") |> s}
       </span>
     | _ => React.null
     }}
    {Belt.Array.mapWithIndex(content, (i, x) => {
       (
         switch (getItem(x)) {
         | Todo({keyword}) => s(keyword)
         | PlainText({value}) => s(value)
         | Link({value, description}) => <a href=value> {s(description)} </a>
         | _ => React.null
         }
       )
       |> wrapWithKey(x.level, i)
     })
     |> React.array
     |> (xs => <Heading level> xs </Heading>)}
    {switch (tags) {
     | Some({tags}) => renderTags(tags)
     | _ => React.null
     }}
  </header>;
};

let renderBlock = x => {
  switch (x) {
  | Block({attributes, name, params, value}) =>
    <section>
      {Js.Dict.entries(attributes)
       ->Belt.Array.mapWithIndex((i, (key, value)) => {
           <span key={string_of_int(i)}> {s({j|$key: $value|j})} </span>
         })
       |> (
         xs =>
           switch (Js.Array.length(xs)) {
           | 0 => React.null
           | _ => <div> {xs |> React.array} </div>
           }
       )}
      <code> {s(value)} </code>
    </section>
  | _ => React.null
  };
};

let renderParagraphs = xs => {
  Belt.Array.mapWithIndex(
    xs,
    (i, x) => {
      let key = string_of_int(i);
      switch (getItem(x)) {
      | PlainText({value}) => <p key> {s(value)} </p>
      | Link({value, description}) => <a href=value> {s(description)} </a>
      | _ => React.null
      };
    },
  )
  |> React.array;
};

let rec renderList = (xs, ordered) => {
  Belt.Array.mapWithIndex(
    xs,
    (i, x) => {
      let key = string_of_int(i);
      switch (getItem(x)) {
      | List({children, ordered}) =>
        <li key> {renderList(children, ordered)} </li>
      | ListItem({children}) => <li key> {renderParagraphs(children)} </li>
      | _ => React.null
      };
    },
  )
  |> React.array
  |> (
    xs =>
      switch (ordered) {
      | true => <ol> xs </ol>
      | _ => <ul> xs </ul>
      }
  );
};

let rec renderItems = xs => {
  Belt.Array.mapWithIndex(xs, (i, x) => {
    switch (getItem(x)) {
    | Headline({children, level}) =>
      renderHeadline(children, level) |> wrapWithKey(level, i)
    | Section({children, level}) =>
      renderItems(children) |> wrapWithKey(level, i)
    | Paragraph({children}) => renderParagraphs(children)
    | Block(_) as x => renderBlock(x)
    | List({children, ordered}) => renderList(children, ordered)
    | _ => React.null
    }
  })
  |> React.array;
};

let render = () => TestContent.orga.children |> Utils.log |> renderItems;

[@react.component]
let make = () => <> {render()} </>;
