open ReOrga;
open OrgClockUtil;

let checkFile = file => {
  let f = Node.Fs.readFileAsUtf8Sync(file);
  let org = Org.parseOrga(f, Org.defaultOptions);
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
};

checkFile("/home/floscr/Documents/Org/Main/inbox.org");
checkFile("/home/floscr/Documents/Org/Work/work.org");

/* let debug = scheduled |> Belt.List.toArray; */
/* [%debugger]; */
