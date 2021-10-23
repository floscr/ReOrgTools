open Relude.Globals;
open ReOrga__Types;

module Utils = {
  let findWithTypeString = (t: string, xs: array(sectionAst)) =>
    xs |> Array.find(({type_}: sectionAst) => type_ === t);
};

module Section = {
  let get = ({parent} as ast: sectionAst): sectionAst =>
    switch (ast |> getItem) {
    | Section(_) => ast
    | _ => parent
    };

  let getChildren = get >> (({children}) => children);
};

module Headline = {
  type t = OrgTypes.Headline.t;

  let get = (ast: sectionAst): option(t) => {
    ast
    |> Section.getChildren
    |> Utils.findWithTypeString("headline")
    |> Option.map(OrgTypes.Headline.make);
  };

  let getKeyword = get >> Option.flatMap(({keyword}: t) => keyword);
};

module Tags = {
  type t = OrgTypes.Tags.t;

  let get = (ast: sectionAst): option(t) => {
    ast
    |> Section.getChildren
    |> Utils.findWithTypeString("tags")
    |> Option.map(OrgTypes.Tags.make);
  };

  let getTags = get >> Option.map(({tags}: t) => tags);
};

module Planning = {
  type t = OrgTypes.Planning.t;

  let get = (ast: sectionAst): option(t) => {
    ast
    |> Section.getChildren
    |> Utils.findWithTypeString("planning")
    |> Option.map(OrgTypes.Planning.make);
  };

  let getStartTime =
    get
    >> Option.flatMap(({start}: t) => start)
    >> Option.map(Js.Date.getTime);
};
