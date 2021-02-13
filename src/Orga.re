type error;
type unified;

type position = {
  line: int,
  column: int,
};

type positionAst = {
  start: position,
  [@bs.as "end"]
  end_: position,
};

type sectionAst = {
  level: int,
  [@bs.as "type"]
  type_: string,
  children: array(sectionAst),
  content: Js.nullable(string),
  position: positionAst,
};

type propertiesAst = {
  title: string,
  date: string,
  template: string,
};

type orgAst = {
  properties: propertiesAst,
  children: array(sectionAst),
};

let nullableOrEmptyStr = x =>
  switch (Js.Nullable.toOption(x)) {
  | Some((x: string)) => x
  | _ => ""
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
    });

let getItem = item =>
  switch (item.type_) {
  | "section" =>
    Section({
      level: item.level,
      children: item.children,
      position: item.position,
    })
  | "headline" =>
    Headline({
      level: item.level,
      children: item.children,
      content: nullableOrEmptyStr(item.content),
      position: item.position,
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
