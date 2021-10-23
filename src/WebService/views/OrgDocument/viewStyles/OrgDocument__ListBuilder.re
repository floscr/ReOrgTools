open Relude.Globals;
open ReOrga;
open OrgDocument__Utils;

let keepItem =
    (~conds=[], ~tags: array(string), headline: OrgTypes.Headline.t) => {
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
                 child
                 |> OrgGlobals.Getters.Tags.getTags
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
    /*
     We can either group by a string or a date
     Right now I can't find an easy way to reduce the duplication here without making it compare.
     */

    module ByDate = {
      open DateTime.Globals;

      type t = (Ungrouped.t, DateMap.t(array(innerT)));
      let empty = (Ungrouped.empty, DateMap.make());

      let append =
          (
            groupKey: option(DateMap.key),
            ast: innerT,
            (ungrouped, grouped): t,
          ) =>
        switch (groupKey) {
        | Some(key) => (
            ungrouped,
            grouped
            |> DateMap.update(key, xs =>
                 xs
                 |> Option.foldLazy(
                      _ => Some([|ast|]),
                      xs => Some(Array.append(ast, xs)),
                    )
               ),
          )
        | _ => (ungrouped |> Array.append(ast), grouped)
        };

      let rec make =
              (
                ~acc: t=empty,
                ~cond=_ => true,
                ~makeGroupKey: innerT => option(DateMap.key)=_ => None,
                rest,
              ) => {
        rest
        |> Array.foldLeft(
             (childAcc, child) =>
               switch (child |> getItem) {
               | Planning(_) =>
                 let group = makeGroupKey(child);
                 append(group, child.parent, childAcc);

               | Section({children} as x) =>
                 make(~acc=childAcc, ~cond, ~makeGroupKey, children)
               | _ => childAcc
               },
             acc,
           );
      };
    };

    module ByString = {
      type t = (Ungrouped.t, StringMap.t(array(innerT)));
      let empty = (Ungrouped.empty, StringMap.make());

      let append =
          (
            group: option(string),
            ast: innerT,
            (ungroupedAcc, groupedAcc): t,
          ) =>
        switch (group) {
        | Some(group) => (
            ungroupedAcc,
            groupedAcc
            |> StringMap.update(group, xs =>
                 xs
                 |> Option.foldLazy(
                      _ => Some([|ast|]),
                      xs => Some(Array.append(ast, xs)),
                    )
               ),
          )
        | _ => (ungroupedAcc |> Array.append(ast), groupedAcc)
        };

      type makeByT =
        | MakeTodo;

      let rec make =
              (
                ~acc: t=empty,
                ~cond=_ => true,
                ~inheritedTags: inheritedTagsT=[||],
                ~makeByT=MakeTodo,
                ~makeGroupKey: innerT => option(string)=_ => None,
                rest,
              ) => {
        rest
        |> Array.foldLeft(
             (childAcc, child) =>
               switch (makeByT, child |> getItem) {
               | (MakeTodo, Headline(headline))
                   when cond((inheritedTags, headline)) =>
                 let group = makeGroupKey(child);
                 append(group, child.parent, childAcc);

               | (_, Section({children} as x)) =>
                 let tags =
                   child
                   |> OrgGlobals.Getters.Tags.getTags
                   |> Option.getOrElse(inheritedTags);

                 make(
                   ~acc=childAcc,
                   ~cond,
                   ~inheritedTags=tags,
                   ~makeByT,
                   ~makeGroupKey,
                   children,
                 );
               | _ => childAcc
               },
             acc,
           );
      };
    };

    let makeByTodo =
      ByString.make(
        ~makeByT=MakeTodo,
        ~makeGroupKey=OrgGlobals.Getters.Headline.getKeyword,
      );

    let makeByDate =
      ByDate.make(~makeGroupKey=x =>
        switch (x |> ReOrga.getItem) {
        | Planning({start}) => start |> Option.map(DateTime.fromJSDate)
        | _ => None
        }
      );
  };
};
