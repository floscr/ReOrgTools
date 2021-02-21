open ReOrga;

let timeframe = 10; // minutes

let rec filterScheduled = xs =>
  Js.Array.reduce(
    (acc, cur) => {
      switch (getItem(cur)) {
      | Section({children}) => List.append(acc, filterScheduled(children))
      | Planning({parent}) => List.append(acc, [(parent, cur)])
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
      switch (start) {
      | Some(x) =>
        ReDate.isAfter(now, x)
        && ReDate.differenceInMinutes(now, x) <= timeframe
      | _ => false
      }
    | _ => false
    }
  );
