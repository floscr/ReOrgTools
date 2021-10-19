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
  };

  type t =
    | Ungrouped(Ungrouped.t)
    | Grouped(Grouped.t);

  let rec unfoldTreeGrouped =
          (
            ~cond=_ => true,
            ~makeGroupStr: innerT => option(string)=_ => None,
            ~acc: Grouped.t=Grouped.empty,
            ~inheritedTags: inheritedTagsT=[||],
            rest,
          ) => {
    rest
    |> Array.foldLeft(
         (childAcc, child) =>
           switch (child |> getItem) {
           | Headline(headline) when cond((inheritedTags, headline)) =>
             let group = makeGroupStr(child);
             Grouped.append(group, child, childAcc);
           | Section({children} as x) =>
             let tags =
               x
               |> ReOrga.Org.Section.getTags
               |> Option.getOrElse(inheritedTags);

             unfoldTreeGrouped(~acc, ~cond, ~inheritedTags=tags, children);
           | _ => childAcc
           },
         acc,
       )/*        | Headline(headline) when cond((inheritedTags, headline)) => */
        /*          let group = makeGroupStr(child); */
        /*          Grouped.append(group, child, childAcc); */
        /*        | Section({children} as x) => */
        /*          let tags = */
        /*            x */
        /*            |> ReOrga.Org.Section.getTags */
        /*            |> Option.getOrElse(inheritedTags); */
        /*          unfoldTreeGrouped(~acc, ~cond, ~inheritedTags=tags, children); */
        /*        | _ => childAcc */
        /*        }, */
        /*      acc, */
        ; /*        switch (child |> getItem) { */ /*      (childAcc, child) => */ /* |> Array.foldLeft( */
        /*    ); */
  };

  let rec unfoldTreeUngrouped =
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
