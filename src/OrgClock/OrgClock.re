open ReOrga;

let f = Node_fs.readFileAsUtf8Sync("./examples/Time.org");
let org = Org.parseOrga(f, Org.defaultOptions);

let rec filterScheduled = xs =>
  Js.Array.reduce(
    (acc, cur) => {
      switch (getItem(cur)) {
      | Section({children}) => List.append(acc, filterScheduled(children))
      | Planning({parent}) as x => List.append(acc, [(parent, x)])
      | _ => acc
      }
    },
    [],
    xs,
  );

let scheduled = filterScheduled(org.children) |> Belt.List.toArray;

Js.log(scheduled);

[%debugger];
