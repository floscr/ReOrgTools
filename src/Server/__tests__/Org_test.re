open Jest;
open Expect;
open Relude.Globals;
open ExpressUtils;

describe("Org.Archive", () => {
  test("extractArchiveItem should split string", () => {
    let text = "Foo";
    let bounds = (2, 5);
    let (from, _to_) = bounds;
    expect(Org.Archive.extractArchiveItem(~text, ~bounds))
    |> toEqual(Error(Org.Archive.CouldNotSplitTextAtIndex((from, text))));
  })
});
