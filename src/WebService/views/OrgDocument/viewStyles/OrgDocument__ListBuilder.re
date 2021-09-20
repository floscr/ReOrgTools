open Relude.Globals;
open ReOrga;
open OrgDocument__Utils;

let keepItem = (~conds=[], ~tags: array(string), headline: ReOrga.headline) => {
  conds |> List.find(cond => cond(tags, headline) === false) |> Option.isNone;
};

module Unfolded = {
  type t =
    | Ungrouped(array(ReOrga.sectionAst))
    | Grouped(StringMap.t(ReOrga.sectionAst));

  let rec unfoldTreeUngrouped =
          (~cond=_ => true, ~acc=[||], ~inheritedTags=[||], rest) => {
    rest
    |> Array.foldLeft(
         (childAcc, child) =>
           switch (child |> getItem) {
           | Headline(headline) when cond((inheritedTags, headline)) =>
             Array.append(child, childAcc)
           | Section({children}) =>
             let tags =
               Array.head(children)
               |> Option.map(getItem)
               |> Option.flatMap(
                    fun
                    | Headline({tags}) =>
                      Some(Array.concat(inheritedTags, tags))
                    | _ => None,
                  )
               |> Option.getOrElse(inheritedTags);

             Array.concat(
               childAcc,
               unfoldTreeUngrouped(
                 ~acc,
                 ~cond,
                 ~inheritedTags=tags,
                 children,
               ),
             );
           | _ => childAcc
           },
         acc,
       );
  };
};
