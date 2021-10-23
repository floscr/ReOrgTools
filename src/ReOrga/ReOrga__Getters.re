open Relude.Globals;
open ReOrga__Types;

let findWithTypeString = (t: string, xs: array(sectionAst)) =>
  xs |> Array.find(({type_}: sectionAst) => type_ === t);

let getSection = ({parent} as ast: sectionAst) =>
  switch (ast |> getItem) {
  | Section(_) => ast
  | _ => parent
  };

let getSectionChildren = getSection >> (({children}) => children);

module Headline = {
  type t = OrgTypes.Headline.t;

  let get = (ast: sectionAst): option(OrgTypes.Headline.t) => {
    ast
    |> getSectionChildren
    |> findWithTypeString("headline")
    |> Option.map(OrgTypes.Headline.make);
  };

  let getKeyword = get >> Option.flatMap(({keyword}: t) => keyword);
};
