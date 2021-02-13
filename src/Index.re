// Entry point

open Orga;

[@bs.val] external document: Js.t({..}) = "document";

let orga = Org.parseOrga("* Headline");

orga.children[0]
|> Orga.getMainItem
|> Array.map(x =>
     switch (getItem(x)) {
     | Headline({content}) => Js.log(content)
     | _ => ()
     }
   );
