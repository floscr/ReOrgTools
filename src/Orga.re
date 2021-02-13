type error;
type unified;

type sectionAst = {
  level: int,
  [@bs.as "type"]
  type_: string,
  children: array(sectionAst),
  content: Js.nullable(string),
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
      level: int,
      children: array(sectionAst),
    })
  | Headline({
      children: array(sectionAst),
      level: int,
      content: string,
    });

let getItem = item =>
  switch (item.type_) {
  | "section" => Section({level: item.level, children: item.children})
  | "headline" =>
    Headline({
      level: item.level,
      children: item.children,
      content: nullableOrEmptyStr(item.content),
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
