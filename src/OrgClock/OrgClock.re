open ReOrga;

let timeframe = 10; // minutes

let f = Node.Fs.readFileAsUtf8Sync("./examples/Time.org");
let org = Org.parseOrga(f, Org.defaultOptions);

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

let now = Js.Date.make();

let scheduled = filterScheduled(org.children) |> filterUpcoming(now);

Belt.List.forEach(scheduled, ((x, _)) => {
  switch (x.children |> Array.to_list |> Belt.List.head) {
  | Some(x) =>
    switch (getItem(x)) {
    | Headline({content}) =>
      Js.log(content);
      Node.Child_process.execSync(
        {j| notify-send "$content" |j},
        Node.Child_process.option(),
      )
      |> ignore;
    | _ => ()
    }
  | _ => ()
  }
});

let debug = scheduled |> Belt.List.toArray;
/* debug |> Js.log; */

[%debugger];
