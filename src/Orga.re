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
  // Headline
  actionable: bool,
  content: Js.nullable(string),
  level: int,
  tags: Js.nullable(array(string)),
  // Plaintext
  value: Js.nullable(string),
  // Todo
  keyword: Js.nullable(string),
  // List
  indent: Js.nullable(int),
  ordered: Js.nullable(bool),
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

type orgItem =
  | Unmatched
  | Section({
      children: array(sectionAst),
      level: int,
      position: positionAst,
    })
  | Headline({
      children: array(sectionAst),
      content: string,
      level: int,
      position: positionAst,
      tags: array(string),
    })
  | Paragraph({children: array(sectionAst)})
  | PlainText({
      children: array(sectionAst),
      value: string,
    })
  | Stars({level: int})
  | Todo({keyword: string})
  | Tags({tags: array(string)})
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

module Org = {
  [@bs.module "orga"] external parseOrga: string => orgAst = "parse";
};
