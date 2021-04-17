open Relude.Globals;
open ReOrga;
open OrgDocument__Utils;
open ReactUtils;

module Styles = {
  open Css;
  open FixedTheme;

  let pre =
    style([
      overflow(scroll),
      margin(Spacing.medium),
      padding(Spacing.medium),
      backgroundColor(var(ThemeKeys.grey00)),
    ]);
};

let renderBlock = x => {
  switch (x) {
  | Block({attributes, name, params, value}) =>
    switch (name) {
    | "QUOTE" => <pre className=Styles.pre> {value |> s} </pre>
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
