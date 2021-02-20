open ReOrga;

let f = [%raw {| require("../../examples/Test.org").default|}];

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
