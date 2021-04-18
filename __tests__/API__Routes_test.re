open Jest;
open Expect;
open Relude.Globals;
open API__Routes;

describe("API__Routes", () => {
  test("Validate list: Success", () =>
    expect(
      validateList([String.isEmpty, String.isNonEmpty], ["", "NonEmpty"]),
    )
    |> toEqual(true)
  );

  test("Validate list: Failure", () =>
    expect(
      validateList([String.isNonEmpty, String.isEmpty], ["", "NonEmpty"]),
    )
    |> toEqual(false)
  );

  test("Validate list: Failure - Mismatching length items", () =>
    expect(
      validateList(
        [String.isNonEmpty, String.isEmpty],
        ["", "NonEmpty", ""],
      ),
    )
    |> toEqual(false)
  );

  test("Validate list: Failure - Mismatching length validations", () =>
    expect(validateList([String.isNonEmpty, String.isEmpty], ["fo"]))
    |> toEqual(false)
  );
});

describe("API__Routes.Routes", () => {
  test("isFileUrl: Success", () =>
    expect(Routes.isFileRoute("https://localhost:3000/file/0/filename"))
    |> toEqual(true)
  );

  test("isFileUrl: failure", () =>
    expect(Routes.isFileRoute("https://localhost:3000/0/0/filename"))
    |> toEqual(false)
  );
});
