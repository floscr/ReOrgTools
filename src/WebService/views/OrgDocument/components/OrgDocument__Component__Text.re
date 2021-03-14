open Relude.Globals;
open ReactUtils;
open ReOrga;
open OrgDocument__Utils;

let renderAttachment = (~attachmentId=None, {value, description}) => {
  let url =
    attachmentId
    |> Option.map(x =>
         x |> String.splitAt(2) |> (((a, b)) => {j|$a/$b/$value|j})
       )
    |> Option.getOrElse(value);

  <img src={j|http://localhost:4000/attachments/$url|j} />;
};

let renderLink = (~attachmentId=None, {protocol, description, value} as link) => {
  link
  |> Option.some
  |> Option.flatMap(({protocol}) => protocol)
  |> Option.flatMap(
       fun
       | "attachment" => renderAttachment(~attachmentId, link) |> Option.some
       | _ => None,
     )
  |> Option.getOrElseLazy(_ =>
       <a href=value> {description |> Option.getOrElse(value) |> s} </a>
     );
};

let renderPlainText = x =>
  switch (getItem(x)) {
  | PlainText({value, style}) =>
    let str = value |> s;
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

let renderParagraphs = (~properties=None, xs) => {
  let atid = properties |> Option.flatMap(x => Js.Dict.get(x, "id"));

  xs
  |> Array.mapWithIndex((x, i) => {
       let key = string_of_int(i);
       switch (getItem(x)) {
       | PlainText(_) =>
         <React.Fragment key> {renderPlainText(x)} </React.Fragment>
       | Link(x) =>
         <React.Fragment key>
           {renderLink(~attachmentId=atid, x)}
         </React.Fragment>
       | _ => React.null
       };
     })
  |> React.array;
};
