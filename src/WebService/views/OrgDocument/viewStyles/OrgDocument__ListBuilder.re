open Relude.Globals;
open ReOrga;
open OrgDocument__Utils;

let keepItem = (~conds=[], ~tags: array(string), headline: ReOrga.headline) => {
  conds |> List.find(cond => cond(tags, headline) === false) |> Option.isNone;
};

module Unfolded = {
  type innerT = ReOrga.sectionAst;
  type inheritedTagsT = array(string);

  module Ungrouped = {
    type t = array(innerT);
    let empty = [||];

    let rec make =
            (
              ~cond=_ => true,
              ~acc: array(innerT)=[||],
              ~inheritedTags: inheritedTagsT=[||],
              rest,
            ) => {
      rest
      |> Array.foldLeft(
           (childAcc, child) =>
             switch (child |> getItem) {
             | Headline(headline) when cond((inheritedTags, headline)) =>
               Array.append(child, childAcc)
             | Section({children} as x) =>
               let tags =
                 x
                 |> ReOrga.Org.Section.getTags
                 |> Option.getOrElse(inheritedTags);

               Array.concat(
                 childAcc,
                 make(~acc, ~cond, ~inheritedTags=tags, children),
               );
             | _ => childAcc
             },
           acc,
         );
    };
  };

  module Grouped = {
    type t = (Ungrouped.t, StringMap.t(array(innerT)));
    let empty = (Ungrouped.empty, StringMap.make());

    let append =
        (group: option(string), x: innerT, (ungroupedAcc, groupedAcc): t) =>
      switch (group) {
      | Some(group) => (
          ungroupedAcc,
          groupedAcc
          |> StringMap.update(group, xs =>
               xs
               |> Option.foldLazy(
                    _ => Some([|x|]),
                    xs => Some(Array.append(x, xs)),
                  )
             ),
        )
      | _ => (ungroupedAcc |> Array.append(x), groupedAcc)
      };

    let print = ((ungrouped, grouped)) => (
      ungrouped,
      grouped |> StringMap.toArray,
    );

    let rec make =
            (
              ~acc: t=empty,
              ~cond=_ => true,
              ~inheritedTags: inheritedTagsT=[||],
              ~makeGroupStr: innerT => option(string)=_ => None,
              rest,
            ) => {
      rest
      |> Array.foldLeft(
           (childAcc, child) =>
             switch (child |> getItem) {
             | Headline(headline) when cond((inheritedTags, headline)) =>
               let group = makeGroupStr(child);
               append(group, child, childAcc);

             | Section({children} as x) =>
               let tags =
                 x
                 |> ReOrga.Org.Section.getTags
                 |> Option.getOrElse(inheritedTags);

               make(
                 ~acc=childAcc,
                 ~cond,
                 ~inheritedTags=tags,
                 ~makeGroupStr,
                 children,
               );
             | _ => childAcc
             },
           acc,
         );
    };

    let makeByTodo =
      make(~makeGroupStr=x =>
        switch (x |> ReOrga.getItem) {
        | Headline({keyword}) => keyword
        | _ => None
        }
      );
  };
};
