open Utils;

type error;
type unified;

type position = {
  column: int,
  line: int,
};

type positionAst = {
  [@bs.as "end"]
  end_: position,
  start: position,
};

type timestamp = {
  date: Js.nullable(Js.Date.t),
  [@bs.as "end"]
  end_: Js.nullable(Js.Date.t),
};

type sectionAst = {
  [@bs.as "type"]
  type_: string,
  children: array(sectionAst),
  position: positionAst,
  // Document does not have this item
  // but for simplicitys sake we'll just assume it there everywhere
  parent: sectionAst,
  // Section
  properties: Js.nullable(Js.Dict.t(string)),
  // Headline
  actionable: bool,
  content: Js.nullable(string),
  level: int,
  tags: Js.nullable(array(string)),
  // Plaintext, Link, Block
  value: Js.nullable(string),
  // Planning
  timestamp: Js.nullable(timestamp),
  // Todo
  keyword: Js.nullable(string),
  // List
  indent: Js.nullable(int),
  ordered: Js.nullable(bool),
  // Link
  description: Js.nullable(string),
  protocol: Js.nullable(string),
  search: Js.nullable(string),
  // Blog
  attributes: Js.nullable(Js.Dict.t(string)),
  name: Js.nullable(string),
  params: Js.nullable(array(string)),
};

type propertiesAst = {
  date: string,
  template: string,
  title: string,
};

type orgAst = {
  children: array(sectionAst),
  properties: propertiesAst,
};

type stars = {level: int};

type style =
  | Plain
  | Bold
  | Italic
  | Underline
  | StrikeThrough
  | Verbatim;

let matchStyle = x =>
  switch (x) {
  | "plain" => Plain
  | "bold" => Bold
  | "italic" => Italic
  | "underline" => Underline
  | "StrikeThrough" => StrikeThrough
  | "Verbatim" => Verbatim
  | _ => Plain
  };

type plainText = {
  children: array(sectionAst),
  value: string,
  style,
};

type link = {
  value: string,
  description: string,
  protocol: string,
  search: string,
};

type tags = {tags: array(string)};

type orgItem =
  | Unmatched
  | Section({
      children: array(sectionAst),
      level: int,
      position: positionAst,
      properties: Js.Dict.t(string),
    })
  | Headline({
      children: array(sectionAst),
      content: string,
      level: int,
      position: positionAst,
      tags: array(string),
    })
  | Paragraph({children: array(sectionAst)})
  | PlainText(plainText)
  | Planning({
      type_: string,
      start: option(Js.Date.t),
      end_: option(Js.Date.t),
      parent: sectionAst,
    })
  | Stars(stars)
  | Todo({keyword: string})
  | Tags(tags)
  | Link(link)
  | List({
      children: array(sectionAst),
      indent: int,
      ordered: bool,
    })
  | ListItem({
      children: array(sectionAst),
      indent: int,
    })
  | ListItemBullet({
      children: array(sectionAst),
      indent: int,
      ordered: bool,
    })
  | Block({
      attributes: Js.Dict.t(string),
      name: string,
      params: array(string),
      value: string,
    })
  | Table({children: array(sectionAst)})
  | TableRow({children: array(sectionAst)})
  | TableCell({children: array(sectionAst)})
  | TableHr({children: array(sectionAst)})
  | Hr({position: positionAst})
  | Document({
      position: positionAst,
      children: array(sectionAst),
      properties: Js.Dict.t(string),
    });

let getItem = item => {
  let t = Js.String.split(".", item.type_) |> Array.to_list;

  switch (t) {
  | ["section"] =>
    Section({
      children: item.children,
      level: item.level,
      position: item.position,
      properties: nullableOrEmptyDict(item.properties),
    })
  | ["headline"] =>
    Headline({
      children: item.children,
      content: nullableOrEmptyStr(item.content),
      level: item.level,
      position: item.position,
      tags: nullableOrEmptyArray(item.tags),
    })
  | ["todo"] => Todo({keyword: nullableOrEmptyStr(item.keyword)})
  | ["stars"] => Stars({level: item.level})
  | ["planning"] =>
    switch (item.timestamp |> Js.Nullable.toOption) {
    | Some(x) =>
      Planning({
        type_: item.type_,
        start: x.date |> Js.Nullable.toOption,
        end_: x.end_ |> Js.Nullable.toOption,
        parent: item.parent,
      })
    | _ =>
      Planning({
        type_: item.type_,
        start: None,
        end_: None,
        parent: item.parent,
      })
    }
  | ["link"] =>
    Link({
      value: nullableOrEmptyStr(item.value),
      description: nullableOrEmptyStr(item.description),
      protocol: nullableOrEmptyStr(item.description),
      search: nullableOrEmptyStr(item.description),
    })
  | ["tags"] => Tags({tags: nullableOrEmptyArray(item.tags)})
  | ["paragraph"] => Paragraph({children: item.children})
  | ["text", style] =>
    PlainText({
      children: item.children,
      value: nullableOrEmptyStr(item.value),
      style: matchStyle(style),
    })
  | ["list"] =>
    List({
      children: item.children,
      indent: nullableOrZero(item.indent),
      ordered: nullableOrBool(item.ordered, false),
    })
  | ["list", "item"] =>
    ListItem({children: item.children, indent: nullableOrZero(item.indent)})
  | ["list", "item", "bullet"] =>
    ListItemBullet({
      children: item.children,
      indent: nullableOrZero(item.indent),
      ordered: nullableOrBool(item.ordered, false),
    })
  | ["block"] =>
    Block({
      attributes: nullableOrEmptyDict(item.attributes),
      name: nullableOrEmptyStr(item.name),
      params: nullableOrEmptyArray(item.params),
      value: nullableOrEmptyStr(item.value),
    })
  | ["table"] => Table({children: item.children})
  | ["table", "row"] => TableRow({children: item.children})
  | ["table", "cell"] => TableCell({children: item.children})
  | ["table", "hr"] => TableHr({children: item.children})
  | ["hr"] => Hr({position: item.position})
  | ["document"] =>
    Document({
      children: item.children,
      position: item.position,
      properties: nullableOrEmptyDict(item.properties),
    })
  | _ => Unmatched
  };
};

let getMainItem = item =>
  switch (getItem(item)) {
  | Section({children}) => children
  | _ => [||]
  };

type options = {todo: option(array(string))};

module Org = {
  let defaultOptions = {todo: Some([|"TODO"|])};
  [@bs.module "orga"]
  external parseOrga: (string, options) => orgAst = "parse";
};
