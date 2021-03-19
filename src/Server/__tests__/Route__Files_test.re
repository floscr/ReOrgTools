open Relude.Globals;

Jest.describe("IO basics", () => {
  Jest.test("length empty array", () =>
    Jest.Expect.expect(Array.length([||])) |> Jest.Expect.toEqual(0)
  );

  Config.workspaces |> Option.tap(List.toArray >> Js.log);

  Js.log("Test");
});
