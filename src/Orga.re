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

type sectionAst = {
  [@bs.as "type"]
  type_: string,
  children: array(sectionAst),
  position: positionAst,
  // Section
  properties: Js.nullable(Js.Dict.t(string)),
  // Headline
  actionable: bool,
  content: Js.nullable(string),
  level: int,
  tags: Js.nullable(array(string)),
  // Plaintext & Link
  value: Js.nullable(string),
  // Todo
  keyword: Js.nullable(string),
  // List
  indent: Js.nullable(int),
  ordered: Js.nullable(bool),
  // Link
  description: Js.nullable(string),
  protocol: Js.nullable(string),
  search: Js.nullable(string),
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

type plainText = {
  children: array(sectionAst),
  value: string,
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
    });

let getItem = item =>
  switch (item.type_) {
  | "section" =>
    Section({
      children: item.children,
      level: item.level,
      position: item.position,
      properties: nullableOrEmptyDict(item.properties),
    })
  | "headline" =>
    Headline({
      children: item.children,
      content: nullableOrEmptyStr(item.content),
      level: item.level,
      position: item.position,
      tags: nullableOrEmptyArray(item.tags),
    })
  | "todo" => Todo({keyword: nullableOrEmptyStr(item.keyword)})
  | "stars" => Stars({level: item.level})
  | "link" =>
    Link({
      value: nullableOrEmptyStr(item.value),
      description: nullableOrEmptyStr(item.description),
      protocol: nullableOrEmptyStr(item.description),
      search: nullableOrEmptyStr(item.description),
    })
  | "tags" => Tags({tags: nullableOrEmptyArray(item.tags)})
  | "paragraph" => Paragraph({children: item.children})
  | "text.plain" =>
    PlainText({
      children: item.children,
      value: nullableOrEmptyStr(item.value),
    })
  | "list" =>
    List({
      children: item.children,
      indent: nullableOrZero(item.indent),
      ordered: nullableOrBool(item.ordered, false),
    })
  | "list.item" =>
    ListItem({children: item.children, indent: nullableOrZero(item.indent)})
  | "list.item.bullet" =>
    ListItemBullet({
      children: item.children,
      indent: nullableOrZero(item.indent),
      ordered: nullableOrBool(item.ordered, false),
    })
  | _ => Unmatched
  };

let getMainItem = item =>
  switch (getItem(item)) {
  | Section({children}) => children
  | _ => [||]
  };

type options = {todo: option(array(string))};

module Org = {
  [@bs.module "orga"]
  external parseOrga: (string, options) => orgAst = "parse";
};
