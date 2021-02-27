open ReOrga;
open ReactUtils;

module Styles = {
  open Css;

  let mainWrapper = style([margin2(~v=zero, ~h=auto), maxWidth(px(750))]);

  let headline = style([display(`flex)]);

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

let makeKey = (level, index) => {j|$level-$index|j};
let wrapWithKey = (level, index, children) => {
  <React.Fragment key={makeKey(level, index)}> children </React.Fragment>;
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

let renderPlainText = x =>
  switch (getItem(x)) {
  | PlainText({value, style}) =>
    let str = s(value);
    switch (style) {
    | Plain => str
    | Bold => <b> str </b>
    | Italic => <i> str </i>
    | Underline => <u> str </u>
    | StrikeThrough => <del> str </del>
    | Verbatim => <code> str </code>
    | _ => str
    };
  | _ => React.null
  };

let renderHeadline = (~position, ~level, ~index, xs) => {
  let {stars, content, tags} =
    Js.Array.reduce(
      (acc, cur) =>
        switch (getItem(cur)) {
        | Stars(x) when Env.showStars => {...acc, stars: Some(x)}
        | Tags(x) => {...acc, tags: Some(x)}
        | _ => {...acc, content: Belt.Array.concat(acc.content, [|cur|])}
        },
      {stars: None, tags: None, content: [||]},
      xs,
    );

  let id =
    "headline"
    ++ "-"
    ++ (position.start.line |> string_of_int)
    ++ "-"
    ++ (position.start.column |> string_of_int)
    ++ "-"
    ++ (position.end_.line |> string_of_int)
    ++ "-"
    ++ (position.end_.column |> string_of_int);

  <header className=Styles.headline key=id id>
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
         | Todo({keyword}) =>
           <span className={Styles.headlineTodo(keyword)}>
             {keyword |> s}
           </span>
         | PlainText(_) => renderPlainText(x)
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

let renderHr = () => <hr />;

let renderBlock = x => {
  switch (x) {
  | Block({attributes, name, params, value}) =>
    switch (name) {
    | "QUOTE" => <pre> {value |> s} </pre>
    | "SRC" =>
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
        <ReactSyntaxHighlighter
          className=""
          language={
            switch (Belt.Array.get(params, 0)) {
            | Some("elisp") => "lisp"
            | Some("emacs-lisp") => "lisp"
            | Some("js") => "javascript"
            | Some(x) => x
            | _ => ""
            }
          }
          style=ReactSyntaxHighlighter.Style.tomorrow>
          {s(value)}
        </ReactSyntaxHighlighter>
      </section>

    | _ => React.null
    }
  | _ => React.null
  };
};

let renderParagraphs = xs => {
  Relude.Globals.Array.mapWithIndex(
    (x, i) => {
      let key = string_of_int(i);
      switch (getItem(x)) {
      | PlainText(_) =>
        <React.Fragment key> {renderPlainText(x)} </React.Fragment>
      | Link({value, description}) =>
        <a href=value key>
          {s(
             x.description
             ->Js.Nullable.toOption
             ->Belt.Option.getWithDefault(value),
           )}
        </a>
      | _ => React.null
      };
    },
    xs,
  )
  |> React.array;
};

let renderTable = xs => {
  let hasTableHead =
    switch (Js.Array.slice(~start=0, ~end_=2, xs) |> Array.map(getItem)) {
    | [|TableRow(_), TableHr(_)|] => true
    | _ => false
    };

  /* let lastIndex = Array.length(xs) - 1; */
  /* let hasTableFoot = */
  /*   switch (Array.sub(xs, lastIndex - 3, lastIndex) |> Array.map(getItem)) { */
  /*   | [|TableHr(_), TableRow(_)|] => true */
  /*   | _ => false */
  /*   }; */

  <table>
    <tbody>
      {Belt.Array.mapWithIndex(
         xs,
         (rowIndex, x) => {
           let rowKey = string_of_int(rowIndex);
           switch (getItem(x)) {
           | TableRow({children}) =>
             <tr key=rowKey>
               {Belt.Array.mapWithIndex(
                  children,
                  (cellIndex, x) => {
                    let cellKey = string_of_int(cellIndex);
                    let key = {j|$rowKey-$cellKey|j};
                    switch (getItem(x)) {
                    | TableCell({children}) =>
                      <th key> {renderParagraphs(children)} </th>
                    | _ => React.null
                    };
                  },
                )
                |> React.array}
             </tr>
             |> (
               x =>
                 switch (rowIndex, hasTableHead) {
                 | (0, true) => <thead> x </thead>
                 /* | (lastIndex, _, true) => <tfoot> x </tfoot> */
                 | _ => x
                 }
             )
           | _ => React.null
           };
         },
       )
       |> React.array}
    </tbody>
  </table>;
};

let renderListItem = xs => {
  Relude.Globals.(
    xs
    |> Array.mapWithIndex((x, i) => {
         let key = string_of_int(i);
         (
           switch (x |> getItem) {
           | ListItemCheckBox({checked}) =>
             <input type_="checkbox" defaultChecked=checked />
           | _ => renderParagraphs([|x|])
           }
         )
         |> (x => <React.Fragment key> x </React.Fragment>);
       })
    |> React.array
  );
};

let rec renderList = (xs, ordered) => {
  Relude.Globals.(
    xs
    |> Array.mapWithIndex((x, i) => {
         let key = string_of_int(i);
         let next = Array.at(i + 1, xs) |> Option.map(getItem);
         switch (getItem(x), next) {
         | (ListItem({children}), Some(List(sublist))) =>
           <li key>
             {renderListItem(children)}
             {renderList(sublist.children, sublist.ordered)}
           </li>
         | (ListItem({children}), _) =>
           <li key> {renderListItem(children)} </li>
         /* | (List(_), _) => React.null */
         | _ => React.null
         };
       })
    |> React.array
    |> (
      xs =>
        switch (ordered) {
        | true => <ol> xs </ol>
        | _ => <ul> xs </ul>
        }
    )
  );
};

let rec renderItems = (~level=0, xs) => {
  Belt.Array.mapWithIndex(xs, (i, x) => {
    switch (getItem(x)) {
    | Headline({children, level, position}) =>
      renderHeadline(~position, ~level, ~index=i, children)
    | Section({children, level}) =>
      renderItems(~level, children) |> wrapWithKey(level, i)
    | Paragraph({children}) =>
      renderParagraphs(children) |> (x => <p key={makeKey(level, i)}> x </p>)
    | Block(_) as x => renderBlock(x) |> wrapWithKey(level, i)
    | List({children, ordered}) =>
      renderList(children, ordered) |> wrapWithKey(level, i)
    | Table({children}) => renderTable(children) |> wrapWithKey(level, i)
    | Hr(_) => renderHr() |> wrapWithKey(level, i)
    | _ => React.null
    }
  })
  |> React.array;
};

let render = () =>
  TestContent.orga |> Utils.log |> (x => x.children |> renderItems);

[@react.component]
let make = () => <div className=Styles.mainWrapper> {render()} </div>;
