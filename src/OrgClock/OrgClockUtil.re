open ReOrga;
open Relude.Globals;

let timeframe = 10; // minutes

let rec filterScheduled = xs =>
  Array.foldLeft(
    (acc, cur) => {
      switch (getItem(cur)) {
      | Section({children}) => List.concat(acc, filterScheduled(children))
      | Planning({parent}) => List.concat(acc, [(parent, cur)])
      | _ => acc
      }
    },
    [],
    xs,
  );

let filterUpcoming = now =>
  List.filter(((section, date)) =>
    switch (section, getItem(date)) {
    | (_, Planning({start})) =>
      start
      |> Option.filter(ReDate.isAfter(now))
      |> Option.filter(x => ReDate.differenceInMinutes(now, x) <= timeframe)
      |> Option.isSome
    | _ => false
    }
  );
