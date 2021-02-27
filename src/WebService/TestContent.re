open ReOrga;

let f = [%raw
  {| require("/home/floscr/Documents/Org/Main/cooking.org").default|}
];

let orga =
  Org.parseOrga(
    f,
    {
      todo:
        Some([|
          "ACTIVE",
          "NEXT",
          "DONE",
          "WAITING",
          "SOMEDAY",
          "CANCELLED",
          "PROJECT",
          "NOTE",
        |]),
    },
  );
