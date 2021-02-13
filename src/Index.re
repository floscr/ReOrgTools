// Entry point

open Orga;

[@bs.val] external document: Js.t({..}) = "document";

let orga = Org.parseOrga("* TODO Headline :FOO:FAA:");

orga.children[0]
|> Orga.getMainItem
|> Utils.log
|> Array.map(x =>
     switch (getItem(x)) {
     | Headline({content}) => Js.log(content)
     | _ => ()
     }
   );
